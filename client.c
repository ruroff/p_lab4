#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define RECEIVE_BUFFER_SIZE 1024
#define def_port 3425
#define def_ip ""  //"192.168.0.120" //enter the ip server in "ip" or leave "" for local use



int main()
{
    int size;
    int sock;
    struct sockaddr_in addr;
    char message[1025];
    char buf[RECEIVE_BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);         //create a socket
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(def_port); 
    if (sizeof(def_ip) > 0)                         
    {
      inet_aton(def_ip, &addr.sin_addr.s_addr);  
    }
    else{
     addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   
    }
    
    
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)   //connect for server
    {
        perror("connect");
        exit(2);
    }


    printf("Please input command:" );
    fgets(message, sizeof(message), stdin);
    send(sock, message, sizeof(message), 0);                            //send command for server
    while ((size = recv(sock, buf, RECEIVE_BUFFER_SIZE - 1, 0)) > 1) {  //read and print the response from the server
            buf[size] = '\0';
            printf("%s", buf);
           
        }
    
    close(sock);

    return 0;
}