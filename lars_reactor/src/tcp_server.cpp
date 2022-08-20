#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "tcp_server.h"

void lars_hello()
{
    std::cout <<"lars Hello" <<std::endl;
}


//构造函数
tcp_server::tcp_server(const char *ip, uint16_t port)
{
    //0. 忽略一些信号  SIGHUP, SIGPIPE
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR)  {
        fprintf(stderr, "signal ignore SIGHUB\n");
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)  {
        fprintf(stderr, "signal ignore SIGHUB\n");
    }
    
    //1. 创建socket
    _sockfd = socket(AF_INET, SOCK_STREAM |SOCK_CLOEXEC, IPPROTO_TCP) ;
    if (_sockfd == -1) {
        fprintf(stderr, "tcp::server :socket()\n");
        exit(1);
    }


    //2 初始化服务器的地址
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);


    //2.5 设置sock可以重复监听
    int op = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        fprintf(stderr, "set socketopt reuse error\n");
    }
    
    //3 绑定端口
    if (bind(_sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "bind error\n");
        exit(1);
    }
    
    //4 监听
    if (listen(_sockfd, 500) == -1) {
        fprintf(stderr , "listen error\n");
        exit(1);
    }
}

//开始提供创建链接的服务
void tcp_server::do_accept()
{
    int connfd;
    while (true) {
        //1 accept
        connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
        if (connfd == -1) {
            if (errno == EINTR)  {
                //中断错误
                fprintf(stderr, "accept errno = EINTR\n");
                continue;
            }
            else if (errno == EAGAIN) {
                fprintf(stderr, "accept errno = EAGAIN\n");
                break;
            }
            else if (errno == EMFILE) {
                //建立链接过多， 资源不够
                fprintf(stderr, "accept errno = EMFILE\n");
                continue;
            }
            else {
                fprintf(stderr, "accept error");
                exit(1);
            }
        }
        else {
            //accept succ!
            //TODO 添加一些心跳机制
            
            //TODO 添加消息队列机制
            
            //写一个回显业务
            int writed;
            const char* data = "hello  Lars!\n";

            do {
                writed = write(connfd, data, strlen(data)+1);
            } while (writed == -1 && errno == EINTR);//表示非阻塞失败

            if (writed > 0) {
                //succ
                printf("write succ!\n");
            }
        }
    }
}

//析构函数  资源的释放
tcp_server::~tcp_server()
{
    close(_sockfd);
}
