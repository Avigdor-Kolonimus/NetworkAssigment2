/**
 * Part B of assignment 2
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <sys/types.h>
#include <unistd.h>          
#include <stdio.h>					//printf
#include <string.h>					//memset
#include <stdlib.h>					//exit
#include <signal.h>    
#include <sys/socket.h>				//socket, connect
#include <sys/un.h>
#include <netinet/in.h>				//struct sockaddr_in, struct sockaddr
#include <netdb.h>					//struct hostent
#include <arpa/inet.h>
//define variables
#define SIM_LENGTH 10 
//#define IP_ADDRESS "10.12.2.115"        //static ip
#define PORT 1337 

int main(int argc, char* argv[])
{ 
  //variables
  int sock; 
  struct sockaddr_in cli_name; 
  int count;
  int value; 
  //question 2.7
  struct addrinfo* res;
  char* hostname;
  char* hostaddr;
  struct sockaddr_in* saddr;

  //check if hostname was sent
  if (argc != 2) 
  {
    perror("Usage: hostnamelookup <hostname>\n");
    exit(1);
  }

  //get hostname
  hostname = argv[1];
  
  //get address by hostname string
  if (0 != getaddrinfo(hostname, NULL, NULL, &res))
  {
    fprintf(stderr, "Error in resolving hostname %s\n", hostname);
    exit(1);
  }

  //resolve host address as string
  saddr = (struct sockaddr_in*)res->ai_addr;
  hostaddr = inet_ntoa(saddr->sin_addr);

  printf("Client is alive and establishing socket connection.\n");
  //create a socket of stream type
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  { 
    perror ("Error opening channel");
    close(sock);
    exit(1);
  }
      
  //reset for cli_name
  memset((char *)&cli_name,0, sizeof(cli_name));
  //define properties required to connect to the socket 
  cli_name.sin_family = AF_INET; 
  cli_name.sin_addr.s_addr = inet_addr(hostaddr);        
  //cli_name.sin_addr.s_addr = inet_addr(IP_ADDRESS); 			//for static ip
  cli_name.sin_port = htons(PORT);
  //connect to the socket, if return 0 then succeeded, if not, print error
  if (connect(sock, (struct sockaddr *)&cli_name, sizeof(cli_name)) < 0)
  { 
    perror ("Error establishing communications");
    close(sock);
    exit(1);
  }
  //loop SIM_LENGTH times
  for (count = 1; count <= SIM_LENGTH; count++)
  { 
    //get values from the socket into value variable
    read(sock, &value, 4);
    printf("Client has received %d from socket.\n", value);
  }

  printf("Exiting now.\n");
  //close the socket
  close(sock); 
  exit(0); 
} 