#include <iostream>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define LISTENQ 1
#define MAGIC_NUMBER_LENGTH 6

struct myftpHeader{
    uint8_t m_protocol[MAGIC_NUMBER_LENGTH]; /* protocol magic number (6 bytes) */
    uint8_t m_type;                          /* type (1 byte) */
    uint8_t m_status;                      /* status (1 byte) */
    uint32_t m_length;                    /* length (4 bytes) in Big endian*/
} __attribute__ ((packed));

myftpHeader OPEN_CONN_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xA2,0x1,htonl(12)};
myftpHeader LIST_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xA4,0xFF,htonl(12)};
myftpHeader GET_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xA6,0x00,htonl(12)};
myftpHeader PUT_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xA8,0xFF,htonl(12)};
myftpHeader SHA_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xAA,0x00,htonl(12)};
myftpHeader QUIT_REPLY={{0xc1,0xa1,0x10,'f','t','p'},0xAC,0xFF,htonl(12)};
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
    std::cout << "hello from ftp server" << std::endl;
    
    int listenfd,connfd;
    int nowstat=1;
    socklen_t clilen;
    struct sockaddr_in clientaddr,serveraddr;
    in_addr_t SERVER_IP=inet_addr(argv[1]);
    in_port_t SERVER_PORT=htons(atoi(argv[2]));

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=SERVER_IP;
    serveraddr.sin_port=SERVER_PORT;

    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1){
        std::cout << "socket error" << std::endl;
        return 0;
    }else{
        std::cout << "socket created" << std::endl;
    }

    int bindres=bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(bindres==-1){
        std::cout << "bind error" << std::endl;
        return 0;
    }else{
        std::cout << "bind created" << std::endl;
    }

    int listenres=listen(listenfd,LISTENQ);
    if(listenres==-1){
        std::cout << "listen error" << std::endl;
        return 0;
    }else{
        std::cout << "listen created" << std::endl;
    }

    while(1){
        if(nowstat==1){
            clilen=sizeof(clientaddr);
            connfd=accept(listenfd,(struct sockaddr *)&clientaddr,&clilen);
            if(connfd==-1){
                std::cout << "connect error" << std::endl;
                return 0;
            }else{
                std::cout << "connect created" << std::endl;
                nowstat=2;
            }
        }

        char recvbuffer[2048];
        myftpHeader recvbuf;
        int recvres=recv(connfd,recvbuffer,sizeof(myftpHeader),0);
        if(recvres==-1){
            std::cout << "receive failure" << std::endl;
            close(connfd);
            return 0;
        }
        //else{
        //    std::cout << "receive success" << std::endl;
        //}
        std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));

        if(recvbuf.m_type==0xa1&&nowstat==2){
            int sendres=send(connfd,&OPEN_CONN_REPLY,sizeof(OPEN_CONN_REPLY),0);
            if(sendres==-1){
                std::cout << "send open failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send open success" << std::endl;
                nowstat=3;
            }
        }else if(recvbuf.m_type==0xa3&&nowstat==3){
            const char* cmd="ls";
            unsigned char listbuffer[2048]={0};
            FILE *fp=popen(cmd, "r");
            if(fp==NULL){
                std::cout << "list failure" << std::endl;
                close(connfd);
                return 0;
            }
            long file_length=fread(listbuffer, 1, 2048, fp);
            listbuffer[file_length]='\0';
            pclose(fp);
            LIST_REPLY.m_length=htonl(13+file_length);
            int sendres1=send(connfd,&LIST_REPLY,sizeof(LIST_REPLY),0);
            int sendres2=mysend(connfd,listbuffer,file_length+1,0);
            if(sendres1==-1||sendres2==-1){
                std::cout << "send list failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send list success" << std::endl;
                std::cout << listbuffer;
            }
        }else if(recvbuf.m_type==0xa9&&nowstat==3){
            char recvbuffer[2048];
            int recvres=recv(connfd,recvbuffer,ntohl(recvbuf.m_length)-12,0);
            if(recvres==-1){
                std::cout << "receive sha failure" << std::endl;
                close(connfd);
                return 0;
            }
            else{
                std::cout << "receive sha success:"<<recvbuffer << std::endl;
            }

            const char* cmd="ls";
            char lstbuffer[2048]={0};
            FILE *fp=popen(cmd, "r");
            if(fp==NULL){
                std::cout << "list failure" << std::endl;
                close(connfd);
                return 0;
            }
            long file_length=fread(lstbuffer, 1, 2048, fp);
            lstbuffer[file_length]='\0';
            pclose(fp);

            char word[128];
            int wordcount=0;
            int myflag=0;
            SHA_REPLY.m_status=0;
            for(int i=0;i<=file_length;i++){
                if(lstbuffer[i]=='\n'){
                    if(strcmp(word,recvbuffer)==0){
                        myflag=1;
                        SHA_REPLY.m_status=1;
                        break;
                    }
                    memset(word,0,sizeof(word));
                    wordcount=0;
                }else{
                    word[wordcount]=lstbuffer[i];
                    wordcount++;
                }
            }

            int sendres=send(connfd,&SHA_REPLY,sizeof(SHA_REPLY),0);
            if(sendres==-1){
                std::cout << "send sha failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send sha success" << std::endl;
            }

            if(myflag){
                char command[128]="sha256sum ";
                strcat(command,word);
                const char* cmd=command;
                unsigned char shabuffer[2048]={0};

                FILE *fp=popen(cmd, "r");
                if(fp==NULL){
                    std::cout << "sha256sum failure" << std::endl;
                    close(connfd);
                    return 0;
                }
                long file_length=fread(shabuffer, 1, 2048, fp);
                shabuffer[file_length]='\0';
                pclose(fp);

                FILE_DATA.m_length=htonl(13+file_length);
                int sendres1=send(connfd,&FILE_DATA,sizeof(FILE_DATA),0);
                int sendres2=mysend(connfd,shabuffer,file_length+1,0);
                if(sendres1==-1||sendres2==-1){
                    std::cout << "send filedata failure" << std::endl;
                    close(connfd);
                    return 0;
                }else{
                    std::cout << "send filedata success" << std::endl;
                    std::cout << shabuffer;
                }
            }
        }else if(recvbuf.m_type==0xa5&&nowstat==3){
            char recvbuffer[2048];
            int recvres=recv(connfd,recvbuffer,ntohl(recvbuf.m_length)-12,0);
            if(recvres==-1){
                std::cout << "receive get failure" << std::endl;
                close(connfd);
                return 0;
            }
            else{
                std::cout << "receive get success:"<<recvbuffer << std::endl;
            }

            const char* cmd="ls";
            char lstbuffer[2048]={0};
            FILE *fp=popen(cmd, "r");
            if(fp==NULL){
                std::cout << "list failure" << std::endl;
                close(connfd);
                return 0;
            }
            long file_length=fread(lstbuffer, 1, 2048, fp);
            lstbuffer[file_length]='\0';
            pclose(fp);

            char word[128];
            int wordcount=0;
            int myflag=0;
            GET_REPLY.m_status=0;
            for(int i=0;i<=file_length;i++){
                if(lstbuffer[i]=='\n'){
                    if(strcmp(word,recvbuffer)==0){
                        myflag=1;
                        GET_REPLY.m_status=1;
                        break;
                    }
                    memset(word,0,sizeof(word));
                    wordcount=0;
                }else{
                    word[wordcount]=lstbuffer[i];
                    wordcount++;
                }
            }

            int sendres=send(connfd,&GET_REPLY,sizeof(GET_REPLY),0);
            if(sendres==-1){
                std::cout << "send get failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send get success" << std::endl;
            }

            if(myflag){
                const char* myfile=word;
                //unsigned char filebuffer[2048]={0};
                unsigned char* filebuffer=(unsigned char*)malloc((1025*1025)*sizeof(unsigned char));

                FILE *fp=fopen(myfile, "rb");
                if(fp==NULL){
                    std::cout << "read file failure" << std::endl;
                    close(connfd);
                    return 0;
                }
                long file_length=fread(filebuffer, 1, 1024*1024, fp);
                pclose(fp);

                FILE_DATA.m_length=htonl(12+file_length);
                int sendres1=send(connfd,&FILE_DATA,sizeof(FILE_DATA),0);
                int sendres2=mysend(connfd,filebuffer,file_length+1,0);
                if(sendres1==-1||sendres2==-1){
                    std::cout << "send getfiledata failure" << std::endl;
                    close(connfd);
                    free(filebuffer);
                    return 0;
                }else{
                    std::cout << "send getfiledata success" << std::endl;
                    std::cout << filebuffer;
                    free(filebuffer);
                }
            }
        }else if(recvbuf.m_type==0xa7&&nowstat==3){
            char recvbuffername[2048];
            int recvres=recv(connfd,recvbuffername,ntohl(recvbuf.m_length)-12,0);
            if(recvres==-1){
                std::cout << "receive get failure" << std::endl;
                close(connfd);
                return 0;
            }
            else{
                std::cout << "receive get success:"<<recvbuffername << std::endl;
            }

            int sendres=send(connfd,&PUT_REPLY,sizeof(PUT_REPLY),0);
            if(sendres==-1){
                std::cout << "send put failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send put success" << std::endl;
            }

            char recvbuffer[2048];
            myftpHeader recvbuf;
            int recvres1=recv(connfd,recvbuffer,sizeof(myftpHeader),0);
            if(recvres1==-1){
                std::cout << "receive filedata failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::memcpy(&recvbuf,recvbuffer,sizeof(myftpHeader));
                if(recvbuf.m_type==0xff){
                    std::cout << "receive filedata success" << std::endl;
                    //unsigned char recvpayload[2048]={0};
                    unsigned char* recvpayload=(unsigned char*)malloc((1025*1025)*sizeof(unsigned char));
                    int recvpayloadlength=ntohl(recvbuf.m_length)-12;
                    int recvres2=myrecv(connfd,recvpayload,recvpayloadlength,0);
                    if(recvres2==-1){
                        std::cout << "receive putpayload failure" << std::endl;
                        close(connfd);
                        free(recvpayload);
                        return 0;
                    }else{
                        std::cout << "receive putpayload success" << std::endl;
                        std::cout << recvpayload;
                        const char* myfilename=recvbuffername;
                        FILE *fpp=fopen(myfilename, "w+b");
                        if(fpp==NULL){
                            std::cout << "write file failure" << std::endl;
                            close(connfd);
                            return 0;
                        }
                        long file_length=fwrite(recvpayload, 1,recvpayloadlength , fpp);
                        pclose(fpp);
                        free(recvpayload);
                        std::cout << "write file success" << std::endl;
                    }
                }
                else std::cout << "receive filedata error" << std::endl;
            }
        }else if(recvbuf.m_type==0xab&&nowstat==3){
            int sendres=send(connfd,&QUIT_REPLY,sizeof(QUIT_REPLY),0);
            if(sendres==-1){
                std::cout << "send quit failure" << std::endl;
                close(connfd);
                return 0;
            }else{
                std::cout << "send quit success" << std::endl;
            }
            if(connfd!=-1)close(connfd);
            nowstat=1;
        }
    }
    return 0;
}