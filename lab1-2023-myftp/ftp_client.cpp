#include <iostream>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAGIC_NUMBER_LENGTH 6

struct myftpHeader{
    uint8_t m_protocol[MAGIC_NUMBER_LENGTH]; /* protocol magic number (6 bytes) */
    uint8_t m_type;                          /* type (1 byte) */
    uint8_t m_status;                      /* status (1 byte) */
    uint32_t m_length;                    /* length (4 bytes) in Big endian*/
} __attribute__ ((packed));

myftpHeader OPEN_CONN_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xA1,0xFF,htonl(12)};
myftpHeader LIST_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xA3,0xFF,htonl(12)};
myftpHeader GET_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xA5,0xFF,htonl(12)};
myftpHeader PUT_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xA7,0xFF,htonl(12)};
myftpHeader SHA_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xA9,0xFF,htonl(12)};
myftpHeader QUIT_REQUEST={{0xc1,0xa1,0x10,'f','t','p'},0xAB,0xFF,htonl(12)};
myftpHeader FILE_DATA={{0xc1,0xa1,0x10,'f','t','p'},0xFF,0xFF,htonl(12)};

int mysend(int sock,unsigned char* buffer, int len,int aa){
    size_t ret = 0;
    while (ret < len) {
        ssize_t b = send(sock, buffer + ret, len - ret, 0);
        if (b == 0) printf("socket Closed"); // 当连接断开
        if (b < 0) printf("Error ?"); // 这里可能发生了一些意料之外的情况
        ret += b; // 成功将b个byte塞进了缓冲区
    }
    return 1;
}

int myrecv(int sock,unsigned char* buffer, int len,int aa){
    size_t ret = 0;
    while (ret < len) {
        ssize_t b = recv(sock, buffer + ret, len - ret, 0);
        if (b == 0) printf("socket Closed"); // 当连接断开
        if (b < 0) printf("Error ?"); // 这里可能发生了一些意料之外的情况
        ret += b; // 成功将b个byte塞进了缓冲区
    }
    return 1;
}

int main(int argc,char** argv) {
    std::cout << "hello from ftp client" << std::endl;
    int sockfd=-1;
    while(1){
        std::cout << ">>>>";
        char cmdline[128];
        char cmd[4][128];
        int cnt=0,num=0;
        fgets(cmdline, sizeof(cmdline), stdin);
        int cmdlen=std::strlen(cmdline);
        if(cmdline[cmdlen-1]=='\n')cmdline[cmdlen-1]='\0';
        for(int i=0;i<cmdlen;i++){
            if(cmdline[i]==' '){
                cmd[num][cnt]='\0';
                cnt=0;
                num++;
                continue;
            }
            cmd[num][cnt]=cmdline[i];
            cnt++;
        }

        if(std::strcmp(cmd[0],"open")==0){
            struct sockaddr_in serveraddr;
            in_addr_t SERVER_IP=inet_addr(cmd[1]);
            in_port_t SERVER_PORT=htons(atoi(cmd[2]));
            sockfd=socket(AF_INET,SOCK_STREAM,0);
            if(sockfd==-1){
                std::cout << "socket error" << std::endl;
                return 0;
            }else{
                std::cout << "socket created" << std::endl;
            }
            bzero(&serveraddr,sizeof(serveraddr));
            serveraddr.sin_family=AF_INET;
            serveraddr.sin_addr.s_addr=SERVER_IP;
            serveraddr.sin_port=SERVER_PORT;

            int connectres=connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
            if(connectres==-1){
                std::cout << "connect error" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "connect created" << std::endl;
            }

            int sendres=send(sockfd,&OPEN_CONN_REQUEST,sizeof(OPEN_CONN_REQUEST),0);
            if(sendres==-1){
                std::cout << "send open failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "send open success" << std::endl;
            }
            
            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres==-1){
                std::cout << "receive open failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xa2){
                    std::cout << "receive open success" << std::endl;
                }
                else std::cout << "receive open error" << std::endl;
            }
        }else if(strcmp(cmd[0],"ls")==0){
            if(sockfd==-1){
                std::cout << "socket error" << std::endl;
                return 0;
            }else{
                std::cout << "socket right" << std::endl;
            }

            int sendres=send(sockfd,&LIST_REQUEST,sizeof(LIST_REQUEST),0);
            if(sendres==-1){
                std::cout << "send list failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "send list success" << std::endl;
            }
            
            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres1=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres1==-1){
                std::cout << "receive list failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xa4){
                    unsigned char recvpayload[2048]={0};
                    int recvpayloadlength=ntohl(recvbuf.m_length)-12;
                    int recvres2=myrecv(sockfd,recvpayload,recvpayloadlength,0);
                    if(recvres2==-1){
                        std::cout << "receive list failure" << std::endl;
                        close(sockfd);
                        return 0;
                    }else{
                        std::cout << "receive list success" << std::endl;
                        std::cout << recvpayload;
                    }
                }
                else std::cout << "receive list error" << std::endl;
            }

        }else if(strcmp(cmd[0],"get")==0){
            if(sockfd==-1){
                std::cout << "socket error" << std::endl;
                return 0;
            }else{
                std::cout << "socket right" << std::endl;
            }

            GET_REQUEST.m_length=htonl(13+strlen(cmd[1]));
            int sendres1=send(sockfd,&GET_REQUEST,sizeof(GET_REQUEST),0);
            int sendres2=send(sockfd,cmd[1],strlen(cmd[1])+1,0);
            if(sendres1==-1||sendres2==-1){
                std::cout << "send get failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "send get success" << std::endl;
            }

            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres==-1){
                std::cout << "receive get failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xa6&&recvbuf.m_status==0){
                    std::cout << "wrong filename" << std::endl;
                    continue;
                }
                if(recvbuf.m_type==0xa6&&recvbuf.m_status==1){
                    char recvbuffer[2048];
                    myftpHeader recvbuf;
                    int recvres1=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
                    if(recvres1==-1){
                        std::cout << "receive filedata failure" << std::endl;
                        close(sockfd);
                        return 0;
                    }else{
                        std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                        if(recvbuf.m_type==0xff){
                            std::cout << "receive filedata success" << std::endl;
                            //unsigned char recvpayload[2048]={0};
                            unsigned char* recvpayload=(unsigned char*)malloc((1025*1025)*sizeof(unsigned char));
                            int recvpayloadlength=ntohl(recvbuf.m_length)-12;
                            int recvres2=myrecv(sockfd,recvpayload,recvpayloadlength,0);
                            if(recvres2==-1){
                                std::cout << "receive getpayload failure" << std::endl;
                                close(sockfd);
                                free(recvpayload);
                                return 0;
                            }else{
                                std::cout << "receive getpayload success" << std::endl;
                                std::cout << recvpayload;
                                FILE *fpp=fopen(cmd[1], "w+b");
                                if(fpp==NULL){
                                    std::cout << "write file failure" << std::endl;
                                    close(sockfd);
                                    return 0;
                                }
                                long file_length=fwrite(recvpayload, 1, recvpayloadlength, fpp);
                                pclose(fpp);
                                std::cout << "write file success" << std::endl;
                                free(recvpayload);
                            }
                        }
                        else std::cout << "receive filedata error" << std::endl;
                    }
                }
                else std::cout << "receive get error" << std::endl;
            }

        }else if(strcmp(cmd[0],"put")==0){
            if(sockfd==-1){
                std::cout << "socket error" << std::endl;
                return 0;
            }else{
                std::cout << "socket right" << std::endl;
            }

            const char* cmdls="ls";
            char lstbuffer[2048]={0};
            FILE *fp=popen(cmdls, "r");
            if(fp==NULL){
                std::cout << "list failure" << std::endl;
                close(sockfd);
                return 0;
            }
            long file_length=fread(lstbuffer, 1, 2048, fp);
            lstbuffer[file_length]='\0';
            pclose(fp);

            char word[128];
            int wordcount=0;
            int myflag=0;
            for(int i=0;i<=file_length;i++){
                if(lstbuffer[i]=='\n'){
                    if(strcmp(word,cmd[1])==0){
                        myflag=1;
                        break;
                    }
                    memset(word,0,sizeof(word));
                    wordcount=0;
                }else{
                    word[wordcount]=lstbuffer[i];
                    wordcount++;
                }
            }

            if(myflag==0){
                std::cout << "wrong filename" << std::endl;
                continue;
            }else{
                PUT_REQUEST.m_length=htonl(13+strlen(cmd[1]));
                int sendres1=send(sockfd,&PUT_REQUEST,sizeof(PUT_REQUEST),0);
                int sendres2=send(sockfd,cmd[1],strlen(cmd[1])+1,0);
                if(sendres1==-1||sendres2==-1){
                    std::cout << "send put failure" << std::endl;
                    close(sockfd);
                    return 0;
                }else{
                    std::cout << "send put success" << std::endl;
                }

                char recvbuffer[2048];
                myftpHeader recvbuf;
                int recvres=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
                if(recvres==-1){
                    std::cout << "receive put failure" << std::endl;
                    close(sockfd);
                    return 0;
                }else{
                    std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                    if(recvbuf.m_type==0xa8){
                        const char* myfile=cmd[1];
                        //unsigned char filebuffer[2048]={0};
                        unsigned char* filebuffer=(unsigned char*)malloc((1025*1025)*sizeof(unsigned char));

                        FILE *fp=fopen(myfile, "rb");
                        if(fp==NULL){
                            std::cout << "read file failure" << std::endl;
                            close(sockfd);
                            return 0;
                        }
                        long file_length=fread(filebuffer, 1, 1024*1024, fp);
                        pclose(fp);

                        FILE_DATA.m_length=htonl(12+file_length);
                        int sendres1=send(sockfd,&FILE_DATA,sizeof(FILE_DATA),0);
                        int sendres2=mysend(sockfd,filebuffer,file_length+1,0);
                        if(sendres1==-1||sendres2==-1){
                            std::cout << "send putfiledata failure" << std::endl;
                            close(sockfd);
                            free(filebuffer);
                            return 0;
                        }else{
                            std::cout << "send putfiledata success" << std::endl;
                            std::cout << filebuffer;
                            free(filebuffer);
                        }
                        
                    }else std::cout << "receive put error" << std::endl;
                }
            }
        }else if(strcmp(cmd[0],"sha256")==0){
            if(sockfd==-1){
                std::cout << "socket error" << std::endl;
                return 0;
            }else{
                std::cout << "socket right" << std::endl;
            }

            SHA_REQUEST.m_length=htonl(13+strlen(cmd[1]));
            int sendres1=send(sockfd,&SHA_REQUEST,sizeof(SHA_REQUEST),0);
            int sendres2=send(sockfd,cmd[1],strlen(cmd[1])+1,0);
            if(sendres1==-1||sendres2==-1){
                std::cout << "send sha failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "send sha success" << std::endl;
            }

            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres==-1){
                std::cout << "receive sha failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xaa&&recvbuf.m_status==0){
                    std::cout << "wrong filename" << std::endl;
                    continue;
                }
                if(recvbuf.m_type==0xaa&&recvbuf.m_status==1){
                    char recvbuffer[2048];
                    myftpHeader recvbuf;
                    int recvres1=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
                    if(recvres1==-1){
                        std::cout << "receive filedata failure" << std::endl;
                        close(sockfd);
                        return 0;
                    }else{
                        std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                        if(recvbuf.m_type==0xff){
                            std::cout << "receive filedata success" << std::endl;
                            unsigned char recvpayload[2048]={0};
                            int recvpayloadlength=ntohl(recvbuf.m_length)-12;
                            int recvres2=myrecv(sockfd,recvpayload,recvpayloadlength,0);
                            if(recvres2==-1){
                                std::cout << "receive shapayload failure" << std::endl;
                                close(sockfd);
                                return 0;
                            }else{
                                std::cout << "receive shapayload success" << std::endl;
                                std::cout << recvpayload;
                            }
                        }
                        else std::cout << "receive filedata error" << std::endl;
                    }
                }
                else std::cout << "receive sha error" << std::endl;
            }

        }else if(strcmp(cmd[0],"quit")==0){
            if(sockfd==-1){
                return 0;
            }else{
                std::cout << "socket right" << std::endl;
            }

            int sendres=send(sockfd,&QUIT_REQUEST,sizeof(QUIT_REQUEST),0);
            if(sendres==-1){
                std::cout << "send quit failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::cout << "send quit success" << std::endl;
            }
            
            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres=recv(sockfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres==-1){
                std::cout << "receive quit failure" << std::endl;
                close(sockfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xac){
                    std::cout << "receive quit success" << std::endl;
                }
                else std::cout << "receive quit error" << std::endl;
            }
            if(sockfd!=-1)close(sockfd);

        }
    }
    return 0;
}