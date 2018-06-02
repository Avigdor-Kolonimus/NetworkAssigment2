/**
 * Part C of assignment 2
 * @author Alexey Titov
 * @version 1
 */
#include <stdio.h>          //printf, sprintf
#include <stdlib.h>         //exit, atoi, malloc, free
#include <unistd.h>         //read, write, close
#include <string.h>         //memcpy, memset
#include <sys/socket.h>     //socket, connect
#include <netinet/in.h>     //struct sockaddr_in, struct sockaddr
#include <netdb.h>          //struct addrinfo, getaddrinfo
#include <netinet/tcp.h>    //tcp
#include <sys/types.h>
#include <fcntl.h>
//define variables
#define BUFFER_SIZE 4096
#define HTTPPORT    80						//http
#define HOSTLENGTH  100
#define PATHLENGTH  200
//the function prints an error and exits the program
void error(const char *msg)
{ 
    perror(msg);
    exit(0);
}
/*
 code to connect the client to the server,
 namely the streaming socket to URL on port 80 (http)
 IPv4 or IPv6 */
int socket_connect(char *host, in_port_t port)
{
	//variables
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
    char strPORT[12];
    sprintf(strPORT,"%d",port);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 						//for IPv4 and IPv6
	hints.ai_socktype = SOCK_STREAM;					//tcp socket
	//returns information about the name of an individual host (such as an IP address) 
	//and fills in struct sockaddr
	if ((rv = getaddrinfo(host,strPORT, &hints, &servinfo)) != 0) 
	{
	 	error("ERROR, no such host");
 		exit(1);
	}
	//loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
 		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{ 
	 		error("ERROR socket");
 			continue;
 		}
 		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
 			close(sockfd);
 			error("ERROR connecting");
 			continue;
 		}
 		break; 					//if we get here, we must have connected successfully
	}
	if (p == NULL)
	{
 		//looped off the end of the list with no connection
        error("ERROR failed to connect");
 		exit(1);
	}
	freeaddrinfo(servinfo); 			//all done with this structure
	return sockfd;
}
//
int main(int argc,char *argv[])
{
    //variables
    int sockfd, bytes, sent, received, total;
    char    response[BUFFER_SIZE];
    int succ_parsing = 0;       //whether the parsing has been
    char hostname[HOSTLENGTH];  //hostname field of the HTTP uri
    int port;                   //port field of the HTTP uri if found
    char path[PATHLENGTH];      //path field of the uri if found
    //check that the entered parameters are correct
    if (argc != 2)
    {
        error("Usage: protocol://hostname/path or protocol://hostname:port/path");
        exit(1);
    }
    //initilization
    memset(hostname,'\0', HOSTLENGTH);
    memset(path,'\0', PATHLENGTH);
    port =HTTPPORT;
    succ_parsing = 0;

    //parsing the argv[1] char*
    if (sscanf(argv[1], "http://%99[^:]:%i/%199[^\n]", hostname, &port, path) == 3) { succ_parsing = 1;}
    else if (sscanf(argv[1], "http://%99[^/]/%199[^\n]", hostname, path) == 2) { succ_parsing = 1;}
    else if (sscanf(argv[1], "http://%99[^:]:%i[^\n]", hostname, &port) == 2) { succ_parsing = 1;}
    else if (sscanf(argv[1], "http://%99[^/]/[^\n]", hostname) == 1) { succ_parsing = 1;}
    else if (sscanf(argv[1], "http://%99[^\n]", hostname) == 1) { succ_parsing = 1;}
    
    //check that the user entered correct url
    if (succ_parsing==0) 
    {
        error("Wrong input, usage: protocol://hostname/path or protocol://hostname:port/path");
        exit(1);
    }
    sprintf(response, "GET /%s HTTP/1.0\nHOST:%s\n\n", path, hostname); 
    //what are we going to send?
    printf("Request:\n%s\n",response);

    //call function to get the socket 
    sockfd=socket_connect(hostname,port);

    //send the request
    total = strlen(response);
    sent = 0;
    do {
        bytes = write(sockfd,response+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);
    //receive the response
    memset(response,'\0',sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    //close the socket
    close(sockfd);
    //process response
    printf("Response:\n%s\n",response);
    return 0;
}