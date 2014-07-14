#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define SH_UX_SOCK_PATH   "/data/local/sh_ux_sock"

#define ERR_EXIT(m) \
 do \
 { \
  perror(m); \
  exit(EXIT_FAILURE); \
 }while(0)

//It's a interface or task left for every modules to finish.
void create_task(int sock)
{
     char sendbuf[1024] = {0};
     char recvbuf[1024] = {0};
     while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
     {
          write(sock, sendbuf, strlen(sendbuf));
          read( sock, recvbuf, sizeof(recvbuf));
          fputs(recvbuf, stdout);
          memset(sendbuf, 0, sizeof(sendbuf));
          memset(recvbuf, 0, sizeof(recvbuf));
     }
     close(sock);
}


void module_task(int sock, void (*create_task)(int) )
{
     create_task(sock);
}


int main(void)
{
     int sock;
     //cearte a unix socket
     if( (sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0 )
     {
          ERR_EXIT("socket");
     }
 
     //init addr.
     struct sockaddr_un servaddr;
     memset(&servaddr, 0, sizeof(servaddr));
     servaddr.sun_family = AF_UNIX;
     strcpy(servaddr.sun_path, SH_UX_SOCK_PATH);

     //connect to service
     if( connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr) ) < 0)
     {
          ERR_EXIT("connect");
     }
    
     //finish its job
     module_task(sock);

     return 0;
}
