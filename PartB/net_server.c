/**
 * Part B of assignment 2
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <sys/types.h>
#include <unistd.h>          
#include <stdio.h>				//printf
#include <string.h>				//memset
#include <stdlib.h>				//exit
#include <signal.h>     
#include <sys/socket.h>			//socket, connect
#include <sys/un.h>
#include <netinet/in.h>			//struct sockaddr_in, struct sockaddr
//define variables
#define SIM_LENGTH 10 
#define PORT 1337 

/**
  This method is used by the server to clean up before existing, 
  only prints and closes the socket 
*/
void clean_up(int cond, int *sock)
{ 
  printf("Exiting now.\n");
  close(*sock); 
  exit(cond);
} 

int main(void)
{ 
  //variables
  int sock; 
  int connect_sock; 
  struct sockaddr_in serv_name; 
  socklen_t len; 
  int count;
  //create a socket of stream type
  sock = socket(AF_INET, SOCK_STREAM, 0);  
  if (sock < 0)
  { 
    perror ("Error opening channel");
    clean_up(1, &sock);
  }
  //reset for serv_name 
  memset((char *)&serv_name,0, sizeof(serv_name));
  //define socket's properties so the socket could listen  
  serv_name.sin_family = AF_INET;                                   
  serv_name.sin_port = htons(PORT); 
  //bind that socket to the address given
  if (bind(sock, (struct sockaddr *)&serv_name, sizeof(serv_name)) < 0)
  { 
    perror ("Error naming channel");
    clean_up(1, &sock);
  }
      
  printf("Server is alive and waiting for socket connection from client.\n");
  //listen to the socket and accept messages
  listen(sock, 1); 
  len = sizeof(serv_name);
  //get a socket to send responses to
  connect_sock = accept(sock, (struct sockaddr *)&serv_name, &len);
  //loop SIM_LENGTH times
  for (count = 1; count <= SIM_LENGTH; count++)
  { 
	  //write a response on the connected socket
    write(connect_sock, &count, 4);
    printf("Server has written %d to socket.\n", count);
  }

  //close the connected socket
  close(connect_sock); 
  //perform a clean up before termination
  clean_up(0, &sock);  
}