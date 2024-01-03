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
    if (argc != 6) {
        LOG_FATAL("Usage: ./sender [receiver ip] [receiver port] [file path] "
                  "[window size] [mode]\n");
    }

    // your code here
//connect
    int sockfd=-1;
    struct sockaddr_in recvaddr;
    in_addr_t CMD_IP=inet_addr(argv[1]);
    in_port_t CMD_PORT=htons(atoi(argv[2]));
    const char* CMD_FILEPATH=argv[3];
    uint16_t CMD_WINDOWSIZE=atoi(argv[4]);
    uint8_t CMD_MODE=atoi(argv[5]);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    bzero(&recvaddr,sizeof(recvaddr));
    recvaddr.sin_family=AF_INET;
    recvaddr.sin_addr.s_addr=CMD_IP;
    recvaddr.sin_port=CMD_PORT;

    //send RTP_SYN and recv RTP_ACK
    rtp_packet_t conn,connect;
    srand((unsigned)time(NULL));
    uint32_t x=rand();
    conn.rtp.seq_num=x;
    conn.rtp.length=0;
    conn.rtp.flags=RTP_SYN;
    conn.rtp.checksum=0;
    conn.rtp.checksum=compute_checksum(&conn,sizeof(rtp_header_t));

    int count=0;
    struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=100;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    while(1){
        sendto(sockfd,&conn,sizeof(conn),0,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
        LOG_DEBUG("s-1-send-seqsum:%d\n",conn.rtp.seq_num);
        LOG_DEBUG("s-1-send-checksum:%d\n",conn.rtp.checksum);
        socklen_t recvlen=sizeof(recvaddr);
        recvfrom(sockfd,&connect,sizeof(rtp_packet_t),0,(struct sockaddr *)&recvaddr,&recvlen);
        uint32_t temp=connect.rtp.checksum;
        connect.rtp.checksum=0;
        connect.rtp.checksum=compute_checksum(&connect,sizeof(rtp_header_t));
        LOG_DEBUG("s-4-recv-seqsum:%d\n",conn.rtp.seq_num);
        LOG_DEBUG("s-4-recv-checksum:%d\n",conn.rtp.checksum);
        if(connect.rtp.checksum==temp){
            if(connect.rtp.flags!=0b0011)continue;
            if(connect.rtp.seq_num!=x+1)continue;
            LOG_DEBUG("s-4-recv-yes\n");
            break;
        }
        count++;
        if(count>50){
            LOG_FATAL("s-1-send-ERROR-timeout\n");
            return 0;
        }
    }

    //send RTP_ACK
    rtp_packet_t connection,checkconn;
    connection.rtp.seq_num=connect.rtp.seq_num;
    connection.rtp.length=0;
    connection.rtp.flags=RTP_ACK;
    connection.rtp.checksum=0;
    connection.rtp.checksum=compute_checksum(&connection,sizeof(rtp_header_t));
    count=0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    while(1){
        sendto(sockfd,&connection,sizeof(connection),0,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
        LOG_DEBUG("s-5-send-seqsum:%d\n",connection.rtp.seq_num);
        LOG_DEBUG("s-5-send-checksum:%d\n",connection.rtp.checksum);
        sleep(2);
        socklen_t recvlen=sizeof(recvaddr);
        int recvres=recvfrom(sockfd,&checkconn,sizeof(rtp_packet_t),0,(struct sockaddr *)&recvaddr,&recvlen);
        LOG_DEBUG("s-7-handshaking-res:%d\n",recvres);
        if(recvres==-1){
            LOG_DEBUG("s-7-handshaking-completed\n");
            break;
        }
        count++;
        if(count>50){
            LOG_FATAL("s-5-send-ERROR-timeout\n");
            return 0;
        }
    }
    LOG_DEBUG("--------------\n");

//send
    //readfile
    char* filebuffer=(char*)malloc((MAXFILESIZE)*sizeof(char));
    FILE *fp=fopen(CMD_FILEPATH,"r+b");
    if(fp==NULL)LOG_FATAL("openfilepath error\n");
    uint32_t file_length=fread(filebuffer, 1, MAXFILESIZE*sizeof(char), fp);
    fclose(fp);
    LOG_DEBUG("filelength:%d\n",file_length);
    LOG_DEBUG("filebuffer:%s\n",filebuffer);
    //sendfile
    uint32_t totalnum=file_length/PAYLOAD_MAX+1;
    uint32_t curptr=0;
    uint32_t base=connection.rtp.seq_num;
    if(CMD_MODE==0){
        while(1){
            uint32_t bound;
            if(CMD_WINDOWSIZE<totalnum-curptr)bound=CMD_WINDOWSIZE;
            else bound=totalnum-curptr;
            for(uint32_t i=0;i<bound;i++){
                rtp_packet_t senddata;
                memcpy(senddata.payload,filebuffer+(curptr+i)*PAYLOAD_MAX*sizeof(char),PAYLOAD_MAX*sizeof(char));
                senddata.rtp.seq_num=base+curptr+i;
                if(curptr+i+1==totalnum)senddata.rtp.length=file_length%PAYLOAD_MAX;
                else senddata.rtp.length=sizeof(senddata.payload);
                senddata.rtp.flags=0;
                senddata.rtp.checksum=0;
                senddata.rtp.checksum=compute_checksum(&senddata,sizeof(rtp_header_t)+senddata.rtp.length);
                sendto(sockfd,&senddata,sizeof(senddata),0,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
                LOG_DEBUG("s-send-seqsum:%d,checksum:%d,length:%d,flag:%d\n",senddata.rtp.seq_num,
                                senddata.rtp.checksum,senddata.rtp.length,senddata.rtp.flags);
            }
            while(1){
                socklen_t recvlen=sizeof(recvaddr);
                rtp_packet_t recvdata;
                if(recvfrom(sockfd,&recvdata,sizeof(rtp_packet_t),0,(struct sockaddr *)&recvaddr,&recvlen)==-1)break;

                if(recvdata.rtp.flags==0b0010&&recvdata.rtp.length==0){
                    uint32_t temp=recvdata.rtp.checksum;
                    recvdata.rtp.checksum=0;
                    recvdata.rtp.checksum=compute_checksum(&recvdata,sizeof(rtp_header_t)+recvdata.rtp.length);
                    if(recvdata.rtp.checksum==temp){
                        LOG_DEBUG("s-recv-seqsum:%d\n",recvdata.rtp.seq_num);
                        if(recvdata.rtp.seq_num>curptr+base&&recvdata.rtp.seq_num<=curptr+base+CMD_WINDOWSIZE)
                            curptr=recvdata.rtp.seq_num-base;
                        else continue;
                    }else continue;
                }else continue;
            }
            LOG_DEBUG("s-curptr:%d,totalnum:%d\n",curptr,totalnum);
            if(curptr==totalnum)break;
        }
    }else if(CMD_MODE==1){
        uint8_t checklist[totalnum];
        uint8_t quitflag=0;
        memset(checklist,0,sizeof(checklist));
        while(quitflag==0){
            uint32_t bound;
            if(CMD_WINDOWSIZE<totalnum-curptr)bound=CMD_WINDOWSIZE;
            else bound=totalnum-curptr;
            for(uint32_t i=0;i<bound;i++){
                if(checklist[i+curptr])continue;
                rtp_packet_t senddata;
                memcpy(senddata.payload,filebuffer+(curptr+i)*PAYLOAD_MAX*sizeof(char),PAYLOAD_MAX*sizeof(char));
                senddata.rtp.seq_num=base+curptr+i;
                if(curptr+i+1==totalnum)senddata.rtp.length=file_length%PAYLOAD_MAX;
                else senddata.rtp.length=sizeof(senddata.payload);
                senddata.rtp.flags=0;
                senddata.rtp.checksum=0;
                senddata.rtp.checksum=compute_checksum(&senddata,sizeof(rtp_header_t)+senddata.rtp.length);
                sendto(sockfd,&senddata,sizeof(senddata),0,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
                LOG_DEBUG("s-send-seqsum:%d,checksum:%d,length:%d,flag:%d\n",senddata.rtp.seq_num,
                                senddata.rtp.checksum,senddata.rtp.length,senddata.rtp.flags);
            }
            while(1){
                socklen_t recvlen=sizeof(recvaddr);
                rtp_packet_t recvdata;
                if(recvfrom(sockfd,&recvdata,sizeof(rtp_packet_t),0,(struct sockaddr *)&recvaddr,&recvlen)==-1)break;
                
                if(recvdata.rtp.flags==0b0010&&recvdata.rtp.length==0){
                    uint32_t temp=recvdata.rtp.checksum;
                    recvdata.rtp.checksum=0;
                    recvdata.rtp.checksum=compute_checksum(&recvdata,sizeof(rtp_header_t)+recvdata.rtp.length);
                    if(recvdata.rtp.checksum==temp){
                        LOG_DEBUG("s-recv-seqsum:%d\n",recvdata.rtp.seq_num);
                        if(recvdata.rtp.seq_num==curptr+base){
                            checklist[curptr]=1;
                            curptr++;
                            if(curptr==totalnum){
                                quitflag=1;
                                break;
                            }
                        }
                        else if(recvdata.rtp.seq_num>curptr+base&&recvdata.rtp.seq_num<curptr+base+CMD_WINDOWSIZE)checklist[recvdata.rtp.seq_num-base]=1;
                    }
                }
            }
            if(quitflag||curptr==totalnum)break;
            while(checklist[curptr]&&curptr<totalnum)curptr++;
            LOG_DEBUG("s-curptr:%d,totalnum:%d\n",curptr,totalnum);
            if(curptr==totalnum)break;
        }


    }else LOG_FATAL("MODE error\n");
    LOG_DEBUG("+++++++++++++++\n");
    LOG_DEBUG("ssss:%d\n",base);

    sleep(3);

//ending
    free(filebuffer);
    rtp_packet_t ending,ended;
    ending.rtp.seq_num=curptr+base;
    ending.rtp.length=0;
    ending.rtp.flags=RTP_FIN;
    ending.payload[0]='\0';
    ending.rtp.checksum=0;
    ending.rtp.checksum=compute_checksum(&ending,sizeof(rtp_header_t));
    count=0;
/*     struct timeval tval;
    tval.tv_sec=5;
    tval.tv_usec=0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tval,sizeof(struct timeval)); */
    while(1){
        sendto(sockfd,&ending,sizeof(ending),0,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
        LOG_DEBUG("s-send-seqsum:%d\n",ending.rtp.seq_num);
        LOG_DEBUG("s-send-checksum:%d\n",ending.rtp.checksum);
        socklen_t recvlen=sizeof(recvaddr);
        recvfrom(sockfd,&ended,sizeof(rtp_packet_t),0,(struct sockaddr *)&recvaddr,&recvlen);

            LOG_DEBUG("1:%d\n",ended.rtp.length);
            LOG_DEBUG("2:%d\n",ended.rtp.flags);
            LOG_DEBUG("3:%d\n",ended.rtp.seq_num);
            LOG_DEBUG("4:%d\n",ended.rtp.checksum);
            LOG_DEBUG("5:%d\n",curptr+base); 

        if(ended.rtp.length!=0)continue;
        if(ended.rtp.flags!=0b0110)continue;
        if(ended.rtp.seq_num!=curptr+base)continue;
        uint32_t temp=ended.rtp.checksum;
        ended.rtp.checksum=0;
        ended.rtp.checksum=compute_checksum(&ended,sizeof(rtp_header_t));
        LOG_DEBUG("s-recv-seqsum:%d\n",ended.rtp.seq_num);
        LOG_DEBUG("s-recv-checksum:%d\n",ended.rtp.checksum);
        if(ended.rtp.checksum==temp){
            LOG_DEBUG("s-recv-yes\n");
            break;
        }
        count++;
        if(count>50){
            LOG_FATAL("s-send-ERROR-timeout\n");
            return 0;
        }
    }
    sleep(2);
    
    LOG_DEBUG("Sender: exiting...\n");
    return 0;
}
