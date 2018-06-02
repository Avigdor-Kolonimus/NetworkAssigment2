/**
 * Part A of assignment 2
 * @author Alexey Titov
 * @version 1
 */
//libraries
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>					//struct sockaddr_in, struct sockaddr
#include <inttypes.h>
#include <netdb.h>						//struct hostent
#include <sys/types.h>
#include <sys/socket.h>					//socket
#include <strings.h>
#include <unistd.h>
#include <stdio.h>						//printf
#include <string.h>						//memset
//./send_udp localhost
int main(int argc, char *argv[])
{
  //define varibales
  int socket_fd,cc=0,fsize;
  struct sockaddr_in  dest,from;
  struct hostent *hostptr;
  struct { char head; u_long body; char tail; } msgbuf;
  //create a new socket of type ipv4 and DGRAM (UDP), without stating the protocol and returning its ptr
  socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
  //set all values in the buffer to zero
  memset((char *) &dest,0, sizeof(dest));  //or bzero((char *) &dest,sizeof(dest));
  //resolve the dest hostname recieved as an argument
  hostptr = gethostbyname(argv[1]);										  
  dest.sin_family = (short) AF_INET;
  //copy the dest address to our hostent struct
  memcpy(hostptr->h_addr, (char *)&dest.sin_addr,hostptr->h_length);    //bcopy
  //convert the decimal 13107 to TCP/IP network byte order port value
  dest.sin_port = htons((u_short)0x3333);
  //create the message
  msgbuf.head = '<';
  msgbuf.body = htonl(getpid());          /* IMPORTANT! */   //this process ID of this program
  msgbuf.tail = '>';
  //sending a message to the socket
  sendto(socket_fd,&msgbuf,sizeof(msgbuf),0,(struct sockaddr *)&dest,sizeof(dest));
  //question 1.5
  //The client send its name to the server and then will receive the server response
  char Name_Client[80]="I_am_Client";
  sendto(socket_fd,Name_Client,(strlen(Name_Client) + 1),0,(struct sockaddr *)&dest,sizeof(dest));
  while(!cc) {
    fsize = sizeof(from);
    char name_msg[80];
    //receives a message and store it into msg varibale
    cc = recvfrom(socket_fd,&name_msg,sizeof( name_msg),0,(struct sockaddr *)&from,&fsize);      
    //print the message of the socket 
    printf("%s\n", name_msg);
    fflush(stdout);
  }
  return 0;
}