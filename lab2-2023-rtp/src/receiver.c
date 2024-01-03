#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include "rtp.h"
#include "util.h"
#define MAXLINE 2048
#define MAXFILESIZE 11*1024*1024

int main(int argc, char **argv) {
    if (argc != 5) {
        LOG_FATAL("Usage: ./receiver [listen port] [file path] [window size] "
                  "[mode]\n");
    }

    // your code here
//connect
    int sockfd=-1;
    struct sockaddr_in sendaddr,recvaddr;
    in_port_t CMD_PORT=htons(atoi(argv[1]));
    const char* CMD_FILEPATH=argv[2];
    uint16_t CMD_WINDOWSIZE=atoi(argv[3]);
    uint8_t CMD_MODE=atoi(argv[4]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&recvaddr,sizeof(recvaddr));
    recvaddr.sin_family = AF_INET;
    recvaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // receiver accepts incoming data from any address
    recvaddr.sin_port = CMD_PORT;
    int bindres=bind(sockfd, (struct sockaddr *)&recvaddr, sizeof(recvaddr));
    if(bindres==-1)LOG_FATAL("UDP ERROR\n"); 

    //recv RTP_SYN
    struct timeval tval;
    tval.tv_sec=5;
    tval.tv_usec=0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tval,sizeof(struct timeval));
    rtp_packet_t conn;
    while(1){
        socklen_t sendlen=sizeof(sendaddr);
        if(recvfrom(sockfd,&conn,sizeof(rtp_packet_t),0,(struct sockaddr *)&sendaddr,&sendlen)==-1)return 0;
        LOG_DEBUG("r-2-recv-seqsum:%d\n",conn.rtp.seq_num);
        LOG_DEBUG("r-2-recv-checksum:%d\n",conn.rtp.checksum);
        uint32_t temp=conn.rtp.checksum;
        conn.rtp.checksum=0;
        conn.rtp.checksum=compute_checksum(&conn,sizeof(rtp_header_t));
        if(conn.rtp.checksum==temp){
            LOG_DEBUG("r-2-recv-yes\n");
            break;
        }
    }

    //send RTP_ACK and recv RTP_ACK
    rtp_packet_t connect,connection;
    connect.rtp.seq_num=conn.rtp.seq_num+1;
    connect.rtp.length=0;
    connect.rtp.flags=RTP_SYN|RTP_ACK;
    connect.rtp.checksum=0;
    connect.rtp.checksum=compute_checksum(&connect,sizeof(rtp_header_t));
    int count=0;
    struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=100;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    while(1){
        sendto(sockfd,&connect,sizeof(connect),0,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
        LOG_DEBUG("r-3-send-seqsum:%d\n",connect.rtp.seq_num);
        LOG_DEBUG("r-3-send-checksum:%d\n",connect.rtp.checksum);
        socklen_t sendlen=sizeof(sendaddr);
        recvfrom(sockfd,&connection,sizeof(rtp_packet_t),0,(struct sockaddr *)&sendaddr,&sendlen);
        LOG_DEBUG("r-6-recv-seqsum:%d\n",connection.rtp.seq_num);
        LOG_DEBUG("r-6-recv-checksum:%d\n",connection.rtp.checksum);
        uint32_t temp=connection.rtp.checksum;
        connection.rtp.checksum=0;
        connection.rtp.checksum=compute_checksum(&connection,sizeof(rtp_header_t));
        if(connection.rtp.checksum==temp){
            if(connection.rtp.flags!=0b0010)continue;
            if(connection.rtp.seq_num!=connect.rtp.seq_num)continue;
            LOG_DEBUG("r-6-recv-yes\n");
            break;
        }
        count++;
        if(count>50){
            LOG_FATAL("r-3-send-ERROR-timeout\n");
            return 0;
        }
    }
    sleep(3.3);
    LOG_DEBUG("--------------\n");

//receive
    uint32_t base=connection.rtp.seq_num;
    uint32_t curptr=0;
    char* filebuffer=(char*)malloc(PAYLOAD_MAX*sizeof(char));
    FILE *fp=fopen(CMD_FILEPATH,"w+b");
    if(fp==NULL)LOG_FATAL("openfilepath error\n");
    if(CMD_MODE==0){
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tval,sizeof(struct timeval));
        while(1){
            rtp_packet_t recvdata,senddata;
            recvdata.rtp.flags=0;
            recvdata.rtp.length=0;
            recvdata.rtp.seq_num=0;
            recvdata.rtp.checksum=0;
            recvdata.payload[0]='\0';

            socklen_t sendlen=sizeof(sendaddr);
            if(recvfrom(sockfd,&recvdata,sizeof(rtp_packet_t),0,(struct sockaddr *)&sendaddr,&sendlen)==-1)return 0;
            
/*             LOG_DEBUG("1:%d\n",recvdata.rtp.length);
            LOG_DEBUG("2:%d\n",recvdata.rtp.flags);
            LOG_DEBUG("3:%d\n",recvdata.rtp.seq_num);
            LOG_DEBUG("4:%d\n",recvdata.rtp.checksum); */

            if(base+curptr+CMD_WINDOWSIZE-1<recvdata.rtp.seq_num||base+curptr-CMD_WINDOWSIZE>recvdata.rtp.seq_num)continue;
            if(recvdata.rtp.length>PAYLOAD_MAX)continue;
            if(recvdata.rtp.length==sizeof(recvdata.payload)||recvdata.rtp.length==strlen(recvdata.payload)){
                uint32_t temp=recvdata.rtp.checksum;
                recvdata.rtp.checksum=0;
                recvdata.rtp.checksum=compute_checksum(&recvdata,sizeof(rtp_header_t)+recvdata.rtp.length);
                if(recvdata.rtp.checksum==temp){
                    if(recvdata.rtp.flags==0b0000)LOG_DEBUG("r-recv-seqsum:%d,checksum:%d,length:%d,flag:%d\n",
                            recvdata.rtp.seq_num,recvdata.rtp.checksum,recvdata.rtp.length,recvdata.rtp.flags);
                    else if(recvdata.rtp.flags==0b0100){
                        LOG_DEBUG("r-recv-ending\n");
                        break;
                    }
                    else continue;
                }else continue;
            }

            if(recvdata.rtp.seq_num==curptr+base){
                curptr++;
                memcpy(filebuffer,recvdata.payload,sizeof(recvdata.payload));
                fwrite(filebuffer, 1, recvdata.rtp.length, fp);
                
                senddata.rtp.flags=RTP_ACK;
                senddata.rtp.length=0;
                senddata.rtp.seq_num=curptr+base;
                senddata.rtp.checksum=0;
                senddata.rtp.checksum=compute_checksum(&senddata,sizeof(rtp_header_t)+senddata.rtp.length);
                sendto(sockfd,&senddata,sizeof(senddata),0,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
                LOG_DEBUG("r-send-seqsum:%d\n",senddata.rtp.seq_num);
            }
        }
    }else if(CMD_MODE==1){
        uint8_t windowlist[CMD_WINDOWSIZE];
        uint16_t filelength[CMD_WINDOWSIZE];
        char *filebuffers=(char*)malloc(PAYLOAD_MAX*CMD_WINDOWSIZE*sizeof(char));
        memset(windowlist,0,sizeof(windowlist));
        memset(filelength,0,sizeof(filelength));
        //memset(filebuffers,'\0',PAYLOAD_MAX*CMD_WINDOWSIZE*sizeof(char));
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tval,sizeof(struct timeval));
        while(1){
            rtp_packet_t recvdata,senddata;
            recvdata.rtp.flags=0;
            recvdata.rtp.length=0;
            recvdata.rtp.seq_num=0;
            recvdata.rtp.checksum=0;
            recvdata.payload[0]='\0';

            socklen_t sendlen=sizeof(sendaddr);
            if(recvfrom(sockfd,&recvdata,sizeof(rtp_packet_t),0,(struct sockaddr *)&sendaddr,&sendlen)==-1)return 0;
            
/*             LOG_DEBUG("1:%d\n",recvdata.rtp.length);
            LOG_DEBUG("2:%d\n",recvdata.rtp.flags);
            LOG_DEBUG("3:%d\n",recvdata.rtp.seq_num);
            LOG_DEBUG("4:%d\n",recvdata.rtp.checksum);
            LOG_DEBUG("5:%d\n",curptr+base); */


            if(base+curptr+CMD_WINDOWSIZE>recvdata.rtp.seq_num&&base+curptr-CMD_WINDOWSIZE<=recvdata.rtp.seq_num){

                //if(recvdata.rtp.length==sizeof(recvdata.payload)||recvdata.rtp.length==strlen(recvdata.payload)){
                if(recvdata.rtp.length<=PAYLOAD_MAX){
                    uint32_t temp=recvdata.rtp.checksum;
                    recvdata.rtp.checksum=0;
                    recvdata.rtp.checksum=compute_checksum(&recvdata,sizeof(rtp_header_t)+recvdata.rtp.length);
                    if(recvdata.rtp.checksum==temp){
                        if(recvdata.rtp.flags==0b0000)LOG_DEBUG("r-recv-seqsum:%d,checksum:%d,length:%d,flag:%d\n",
                                recvdata.rtp.seq_num,recvdata.rtp.checksum,recvdata.rtp.length,recvdata.rtp.flags);
                        else if(recvdata.rtp.flags==0b0100){
                            LOG_DEBUG("r-recv-ending\n");
                            free(filebuffers);
                            break;
                        }
                        else continue;
                    }else continue;
                }else continue;
                senddata.rtp.flags=RTP_ACK;
                senddata.rtp.length=0;
                senddata.rtp.seq_num=recvdata.rtp.seq_num;
                senddata.rtp.checksum=0;
                senddata.rtp.checksum=compute_checksum(&senddata,sizeof(rtp_header_t)+senddata.rtp.length);
                sendto(sockfd,&senddata,sizeof(senddata),0,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
                LOG_DEBUG("r-send-seqsum:%d\n",senddata.rtp.seq_num);

                if(recvdata.rtp.seq_num>=curptr+base&&recvdata.rtp.seq_num<curptr+base+CMD_WINDOWSIZE){
                    uint16_t filenumber=recvdata.rtp.seq_num-curptr-base;
                    memcpy(filebuffers+filenumber*PAYLOAD_MAX*sizeof(char),recvdata.payload,sizeof(recvdata.payload));
                    filelength[filenumber]=recvdata.rtp.length;
                    windowlist[filenumber]=1;
                }
/*                 uint8_t maxcount=0;
                while(windowlist[0]==1){
                    if(maxcount==CMD_WINDOWSIZE)break;
                    
                    curptr++;
                    fwrite(filebuffers, 1, filelength[0], fp);
                    for(uint8_t i=1;i<CMD_WINDOWSIZE;i++){
                        filelength[i-1]=filelength[i];
                        windowlist[i-1]=windowlist[i];
                        memcpy(filebuffers+(i-1)*PAYLOAD_MAX*sizeof(char),filebuffers+i*PAYLOAD_MAX*sizeof(char),PAYLOAD_MAX*sizeof(char));
                    }
                    windowlist[CMD_WINDOWSIZE-1]=0;
                    filelength[CMD_WINDOWSIZE-1]=0;
                    maxcount++;
                    //memset(filebuffers+PAYLOAD_MAX*(CMD_WINDOWSIZE-1)*sizeof(char),'\0',PAYLOAD_MAX*sizeof(char));
                } */
                uint32_t thecount=0;
                uint32_t totalfilelength=0;
                while(windowlist[thecount]==1&&thecount<CMD_WINDOWSIZE){
                    totalfilelength+=filelength[thecount];
                    thecount++;
                }
                curptr+=thecount;
                fwrite(filebuffers,1,totalfilelength,fp);
                for(uint32_t i=thecount;i<CMD_WINDOWSIZE;i++){
                    filelength[i-thecount]=filelength[i];
                    windowlist[i-thecount]=windowlist[i];
                    memcpy(filebuffers+(i-thecount)*PAYLOAD_MAX*sizeof(char),filebuffers+i*PAYLOAD_MAX*sizeof(char),PAYLOAD_MAX*sizeof(char));
                }
                for(uint32_t i=0;i<thecount;i++){
                    windowlist[CMD_WINDOWSIZE-thecount+i]=0;
                    filelength[CMD_WINDOWSIZE-thecount+i]=0;
                }


                LOG_DEBUG("11:%d\n",curptr+base);

            }

        }

    }else LOG_FATAL("MODE error\n");
    LOG_DEBUG("+++++++++++++\n");
    fclose(fp);
    free(filebuffer);

    LOG_DEBUG("ssss:%d\n",base);

//ending
    tval.tv_sec=1;
    tval.tv_usec=0;
    rtp_packet_t ending,ended;
    ending.rtp.seq_num=curptr+base;
    ending.rtp.length=0;
    ending.rtp.flags=RTP_FIN|RTP_ACK;
    ending.rtp.checksum=0;
    ending.rtp.checksum=compute_checksum(&ending,sizeof(rtp_header_t));
    count=0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tval,sizeof(struct timeval));
    while(1){
        sendto(sockfd,&ending,sizeof(ending),0,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
        LOG_DEBUG("r-send-seqsum:%d\n",ending.rtp.seq_num);
        LOG_DEBUG("r-send-checksum:%d\n",ending.rtp.checksum);
        socklen_t sendlen=sizeof(sendaddr);
        if(recvfrom(sockfd,&ended,sizeof(rtp_packet_t),0,(struct sockaddr *)&sendaddr,&sendlen)==-1){
            LOG_DEBUG("success\n");
            break;
        }
        count++;
        if(count>50){
            LOG_FATAL("r-send-ERROR-timeout\n");
            return 0;
        }
    }
    LOG_DEBUG("Receiver: exiting...\n");
    return 0;
}
