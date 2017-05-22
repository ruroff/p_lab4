#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define def_port 3425 
    

void receive_transmit(int sock, int mode)           //function for transmit and receive 
{
    char buf_in[128];
    char buf_out[512];
    int bytes_read;
    FILE *ptr; 

    struct timeval time_v;
    time_v.tv_sec = 2;
    time_v.tv_usec = 0;

    fd_set fdrt;
    FD_ZERO(&fdrt);
    FD_SET(sock, &fdrt);


//-------------------------------------------------

    select(sock + 1, &fdrt, NULL, NULL, &time_v);
                
    bytes_read = recv(sock, buf_in, 100, 0);                    //reading command from socket
    printf("%s\n", buf_in);
    if(bytes_read <= 0) _exit(0);
                
    if ((ptr = popen(buf_in, "r")) > 0){                        //run command

        while (fgets(buf_out, sizeof(buf_out) -1, ptr) > 0){    //write the response to the socket
            send(sock, buf_out, strlen(buf_out), 0);
            usleep(1);
           }
    }
    pclose(ptr);                                                
    close(sock);
    if(mode == 0){
        _exit(0);
    }
} 

//-------------------------------------------------

void * thread_func(void *arg)                                   //new thread function
{

    receive_transmit(*(int*) arg, 1);
    
}

//-------------------------------------------------

int main(int argc, char *argv[])
{

    int sock;  
    struct sockaddr_in addr;
    int listener;
    struct timeval time_v;
    fd_set sockfd;
    struct sockaddr_in client;
    int sockaddr_size, *sock_info;
        
    
    listener = socket(AF_INET, SOCK_STREAM, 0);                 //create a socket
    
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(def_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
   
    sockaddr_size = sizeof(struct sockaddr_in);

    while(1)
    { 
        FD_ZERO(&sockfd);
        FD_SET(listener, &sockfd);
        time_v.tv_sec = 1; 
        time_v.tv_usec = 0;

        
        if ((select(listener + 1, &sockfd, NULL, NULL, &time_v)) < 0) {
            perror("select()");
            exit(3);
        }

        sock = accept(listener, (struct sockaddr*) &client, (socklen_t*) &sockaddr_size);
        
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }

    
        if((strcmp(argv[1], "-process") == 0)) 
        {
              switch(fork())                                                    //create new process
            {
            case -1:
                perror("fork");
                break;
                
            case 0:
                
                receive_transmit(sock, 0);
                
            default:
                close(sock);
            }  
        }
        else if((strcmp(argv[1], "-pthreads") == 0))
        {

            pthread_t thread1;                                                   //create new thread
            if(pthread_create(&thread1, NULL, thread_func, &sock) < 0)
            {
                perror("thread");
                exit(2);
            }

        }

    }
    
    

    return 0;
}