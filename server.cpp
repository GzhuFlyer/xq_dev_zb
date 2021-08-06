#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

#include "server.hpp"
#include "client_data_process.hpp"
#include "zmq/SyncService.hpp"
#include "ZB_data_processor.hpp"

using namespace std;
using namespace xq;

extern char    *p_map;
int g_server_conn;
extern int g_buff_fd[2];
// extern int g_server_conn_fd[2];
static void do_service(int conn)
{
    // SetServerWriteConnFd(conn);

    // close(g_server_conn_fd[0]);
    // write(g_server_conn_fd[1],&conn,sizeof(int));
    // cout << "server + conn = " << conn << endl;
    g_server_conn = conn;
    SyncService srv("127.0.0.1", 5556, &ZMQSyncServiceRecvCBForCommand, nullptr);
    char recvbuf[1024];
    while(1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = read(conn,recvbuf,sizeof(recvbuf));
        // printf("ret=%d\n",ret);
        if(ret == 0)    //当返回0表示客户端关闭了
        {
            printf("client close\n");
            break;//退出函数，关闭该通信
        }else if(ret == -1)
        {
           ERR_EXIT("read");
        }
        
        // fputs(recvbuf,stdout);
        printf("\n******************************************\n");
        printf("\n\nrecvbuf--->%s\n\n",recvbuf);
        printf("\n******************************************\n");

        // memcpy(p_map,recvbuf,strlen(recvbuf)+1);
        // *(p_map+1023)=1;
        // printf("*(p_map+1023)------>%d\n",*(p_map+1023));

        
        write(g_buff_fd[1],recvbuf,strlen(recvbuf)+1);
        // StoreDataInZbDB(recvbuf);
        // write(conn,recvbuf,sizeof(recvbuf));
    }   
}

static void handle_sigchld(int sig)
{
    //wait(NULL);   //只能捕获一个信号
    //循环调用，捕获信号，直至进程退出
    while (waitpid(-1,NULL,WNOHANG) > 0);
}

void startTcpServer(void)
{
    signal(SIGCHLD,handle_sigchld); 
    int listenfd;
    if((listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = PF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
        ERR_EXIT("setsockopt");
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    if(listen(listenfd,SOMAXCONN) < 0)
        ERR_EXIT("listen");
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    int conn;
    pid_t pid;
    while(1)
    {
        if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen)) < 0)
            ERR_EXIT("accept");
        //将ip地址转换成点分十进制的ip地址
        printf("ip=%s,port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
        pid = fork();   
        if(pid == -1)
            ERR_EXIT("fork");
        if(pid == 0)  
        {   
            close(listenfd);
            do_service(conn);
            exit(EXIT_SUCCESS);
        }else
        {
            close(conn);
        }
    }

}       