/**
 * Part A of assignment 2
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <sys/types.h>
#include <netinet/in.h>     		//struct sockaddr_in, struct sockaddr
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>					//struct hostent
#include <sys/types.h>
#include <sys/socket.h>				//socket
#include <strings.h>
#include <unistd.h>
#include <stdio.h>					//printf
#include <string.h>					//memset

//question 1.4
/*
The function will print the string indicated by pname ended with a line break, 
then it will print the string indicated by msg, then the string "ip= " follow by 
the IP address of the host associated with this socket address structure in dotted-decimal notation,
a comma, and finally the string "port= " followed by the port number associated with this socket address structure.
*/
void printsin(struct sockaddr_in *sin, char *pname, char *msg)
{
  char fromip[INET_ADDRSTRLEN];
  printf ("%s\n%s", pname, msg);
  printf (" ip=%s, port=%d\n", inet_ntop(AF_INET, &(sin->sin_addr.s_addr), fromip, sizeof(fromip)),
          ntohs((unsigned short)(sin -> sin_port)));
}
//
int main(int argc, char *argv[])
{
  //define varibales
  int socket_fd, cc, fsize;
  int count=0;                    //counter for question 1.5
  struct sockaddr_in  s_in, from;
  struct { char head; u_long  body; char tail;} msg;
  //create a new socket of type ipv4 and DGRAM (UDP), without stating the protocol and returning its ptr
  socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
  //set all values in the buffer to zero
  memset((char *) &s_in,0, sizeof(s_in));  /* They say you must do this */
  //initilization socket 
  //define the address used by the socket
  s_in.sin_family = (short)AF_INET;
  s_in.sin_addr.s_addr = htonl(INADDR_ANY);    /* WILDCARD */
  s_in.sin_port = htons((u_short)0x3333);      // convert the decimal 13107 to TCP/IP network byte order port value
  
  //question 1.4
  printsin( &s_in, "UDP-SERVER:", "Local socket is:"); 
  fflush(stdout);

  //binds the unbound socket to the address given---this is system call
  bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in));
  //loops infinitly
  while(1) {
    fsize = sizeof(from);
    //receives a message and store it into msg varibale
    cc = recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr *)&from,&fsize);
    if (count==0)
    {
      //question 1.4
      printsin( &from, "UDP-SERVER: ", "Packet from:");
      
      //print the message of the socket 
      printf("Got data ::%c%ld%c\n",msg.head,(long) ntohl(msg.body),msg.tail); 
      fflush(stdout);
      count++;
    }
    else                    //question 1.5
    {
      //the server receive the client name and will send back its name
      char Name_Server[80]="I_am_Server";
      sendto(socket_fd,Name_Server,(strlen(Name_Server) + 1),0,(struct sockaddr *)&from,sizeof(from));
      printf("%s\n",msg); 
      fflush(stdout);
      count=0;
    }
  }
  
  return 0;
}
