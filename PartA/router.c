/**
 * Part A of assignment 1.6
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <time.h>						//random
#include <stdio.h>						//printf
#include <stdlib.h>						//exit
#include <sys/socket.h>					//socket
#include <sys/time.h>					//select
#include <netinet/in.h>					//struct sockaddr_in, struct sockaddr
#include <string.h>						//memset

//define variables
#define SERVERPORT 7890
#define CLIENTPORT 7891
#define IPROUTER   "127.0.0.1"
#define IPSERVER   "127.0.0.1"
#define SIZE 		1024
int main(int argc, char *argv[])
{
  int udpSocket, nBytes;
  char buffer[SIZE];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage routerStorage, clientStorage;
  socklen_t addr_size, client_addr_size;
  double RandX=0,X=0;
  if(argc != 2)
  {
	printf("You need entery number between 0 and 1");
	exit(1); 
  }
  //transformation a string to a number
  X=atof(argv[1]);
  char output[strlen(argv[1])+1];
  snprintf(output, strlen(argv[1])+1, "%f", X);
  //checking that the number is between 0 and 1, and also input string is number
  if (X<0 || X>1 || strcmp(output,argv[1]))
  {
    printf("You need entery number between 0 and 1");
	exit(1);
  }
  
  //сreate UDP socket
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  //сonfigure settings in address struct
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_port = htons(CLIENTPORT);
  clientAddr.sin_addr.s_addr = inet_addr(IPROUTER);
  memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);  

  //bind socket with address struct
  bind(udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr));

  //сonfigure settings in address struct
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVERPORT);
  serverAddr.sin_addr.s_addr = inet_addr(IPSERVER);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); 
  //initialize size variable to be used later on
  addr_size = sizeof routerStorage;
  client_addr_size = sizeof clientStorage;

  srand(time(NULL));   // should only be called once
  while(1)
  {
    /* Try to receive any incoming UDP datagram. Address and port of 
        requesting client will be stored on routerStorage variable */
    nBytes = recvfrom(udpSocket,buffer,SIZE,0,(struct sockaddr *)&routerStorage, &addr_size);
    RandX=((double) rand() / (RAND_MAX));     // returns a pseudo-random double between 0 and 1
    printf("\n %f \n",RandX);
    if(RandX>X)
    {
        //send message to server
        sendto(udpSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr,client_addr_size);

        //receive message from server
        nBytes = recvfrom(udpSocket,buffer,SIZE,0,NULL, NULL);
    
        //send uppercase message back to client, using routerStorage as the address
        sendto(udpSocket,buffer,nBytes,0,(struct sockaddr *)&routerStorage,addr_size);
    }
  }
  return 0;
}