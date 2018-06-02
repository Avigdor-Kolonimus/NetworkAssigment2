/**
 * Part A of assignment 1.6
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <stdio.h>						//printf
#include <stdlib.h>						//exit
#include <sys/socket.h>					//socket
#include <sys/time.h>					//select
#include <netinet/in.h>					//struct sockaddr_in, struct sockaddr
#include <string.h>						//memset
//define variables
#define EXIT "exit"                     //the magic word to disconnect
#define ROUTERPORT 7891
#define IPROUTER   "127.0.0.1"
#define SIZE 		1024
int main()
{
  int clientSocket, nBytes;
  char buffer[SIZE];
  struct sockaddr_in routerAddr;
  socklen_t addr_size;
  //create UDP socket
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  //configure settings in address struct
  routerAddr.sin_family = AF_INET;
  routerAddr.sin_port = htons(ROUTERPORT);
  routerAddr.sin_addr.s_addr = inet_addr(IPROUTER);
  memset(routerAddr.sin_zero, '\0', sizeof routerAddr.sin_zero);  

  //initialize size variable to be used later on
  addr_size = sizeof routerAddr;
    
  while(1)
  {
    //timeout variables
    struct timeval timeout = {1, 0 };
    fd_set in_set;
    int cnt;

    printf("Type a sentence to send to server:\n");
    fgets(buffer,SIZE,stdin);
    nBytes = strlen(buffer) + 1;
    if ((strlen(buffer)-1)==strlen(EXIT) && (strstr(buffer,EXIT)!=NULL))
    {
        printf("You choose exit");
        exit(1);
    }
    //send message to server
    sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&routerAddr,addr_size);
    
    //track whether anything has been entered into the stdin(clientSocket)
    FD_ZERO(&in_set);
    FD_SET(clientSocket, &in_set);
    //select the set
    cnt = select(clientSocket + 1, &in_set, NULL, NULL, &timeout);
    //FD_ISSET, which returns zero if the descriptor is present in the set, and a nonzero value, if not present
    if (FD_ISSET(clientSocket, &in_set))
    {
        //receive message from server
        nBytes = recvfrom(clientSocket,buffer,SIZE,0,NULL, NULL);
        if (nBytes>=0)
            printf("Received from server: %s\n",buffer);
    }
    else
        printf("message Receive Timeout or other error\n");
  }

  return 0;
}