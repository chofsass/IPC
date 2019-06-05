#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define BUF_SIZE 512

const char *server_path = "server.sock";
const char *client_path = "client.sock";

int main() {
    struct sockaddr_un servaddr, cliaddr;
    socklen_t len = sizeof(struct sockaddr_un);
    char buf[BUF_SIZE];
    int fd;
    int n; 


    if ( (fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 

    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, server_path, sizeof(servaddr.sun_path)-1);
    unlink(server_path);

    if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind error");
        exit(-1);
    }

    n = recvfrom(fd, buf, BUF_SIZE, MSG_WAITALL, ( struct sockaddr *)&cliaddr, &len); 
    buf[n] = '\0'; 
    printf("Client : %s\n", buf); 

    n = sendto(fd, "msg received", strlen("msg received"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
    if(n == -1)
    {
        perror("bind error");
        exit(-1);
    }

    printf("message sent.\n");  


    return 0;
}