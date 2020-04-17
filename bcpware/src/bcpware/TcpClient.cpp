#include "TcpClient.h"
#include <winsock2.h>
#include <cctype>
#include <cstdio>

TcpClient::TcpClient(bool decode_body)
{
	WSADATA wsaData;
	WORD version;
	int error;

	version = MAKEWORD( 2, 0 );

	error = WSAStartup( version, &wsaData );

	recvMsgSize = 0;
	/* check for error */
	//if ( error != 0 ) addEditL("wsastartup fail");

	decode = decode_body;
}


int TcpClient::recvTimeOutTCP( int socket, long sec, long usec )
{
  TIMEVAL  timeout;
  FD_SET fd0,fd1,fd2;

  timeout.tv_sec = sec;
  timeout.tv_usec = usec;
  FD_ZERO( &fd0 );
  FD_SET( socket, &fd0 );
  FD_ZERO( &fd1 );
  FD_SET( socket, &fd1 );  
  FD_ZERO( &fd2 );
  FD_SET( socket, &fd2 );
  // Possible return values:
  // -1: error occurred
  // 0: timed out
  // > 0: data ready to be read
//  if (DEBUG > 0) printf("Waiting on fd %d\n",socket);
  return select(1, &fd0, &fd1, &fd2, &timeout);
}

/*
 * size of received message
 */
int TcpClient::recvSize(void)
{
	return recvMsgSize;
}

// size of decoded body
int TcpClient::bodySize(void)
{
	return decodeLength;
}


/*
 * get pointer to returned buffer Header
 */
char * TcpClient::getHeader(void)
{
	return recvBuffer;
}

/*
 * received message body
 */
char * TcpClient::getBody(void)
{
	return body_start;
}


/*
 * received message body decoded
 */
char * TcpClient::getDecodedBody(void)
{
	return decodedBuffer;
}

/*
* function to send data to a tcp port/address
*	returns 1 on error.  0 on success
*/
int TcpClient::send_tcpip_request(char *sendBuffer, unsigned int sendMsgSize, char *ip, unsigned int PORT)
{
	return send_tcpip_request(sendBuffer, sendMsgSize, ip, PORT, 2);
}

/*
* function to send data to a tcp port/address
*	returns 1 on error.  0 on success
*/
int TcpClient::send_tcpip_request(char *sendBuffer, unsigned int sendMsgSize, char *ip, unsigned int PORT, unsigned int timeouts)
{
	int sockfd; // main socket file descriptor
	struct sockaddr_in host; // connector's address information
	int i;
	unsigned long iMode = 1;
	fd_set read_fs;
	timeval tvread = { timeouts, 0 };	// 5 second
	timeval tvread2 = { 0, 500 };	// 50 msecond
	int end_found = 0;

	recvMsgSize = 0;

	//printf("socket \n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
        perror("socket open");
		fprintf(stderr,"%d\n",sockfd);
		return 1;
    }
   	//printf("socket \n");
 
	//-------------------------
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.

	if (ioctlsocket(sockfd, FIONBIO, &iMode) != NO_ERROR)
	{
		perror("socket io");
        return 1;
	}

    host.sin_family = AF_INET;
    host.sin_port = htons(PORT); // short, network byte order
    //host.sin_addr = *((struct in_addr *)he->h_addr);
	host.sin_addr.s_addr = inet_addr(ip);
    memset(&(host.sin_zero), '\0', 8); // zero the rest of the struct

	connect(sockfd, (struct sockaddr *)&host, sizeof(struct sockaddr));	// non blocking
	i = recvTimeOutTCP( sockfd, timeouts, 0); // 5 sec
	//printf("recvTimeOutTCP  %d\n", i);
	if ( i == 0  || i == SOCKET_ERROR)
	{
		perror("connect");
        return 1;
	}

 //   if (DEBUG>0) printf("Connected to %s  %d\n", inet_ntoa(host.sin_addr));

	iMode = 2;
	if (ioctlsocket(sockfd, FIONBIO, &iMode) != NO_ERROR)
	{
		perror("socket");
        return 1;
	}

	send(sockfd, sendBuffer, sendMsgSize, 0);
	//printf("\n");
	//fflush(stdout);

	// now perfrom a recv using timeouts as mutliple packets may be received and socket not closed properly
	FD_ZERO(&read_fs); 
	FD_SET(sockfd, &read_fs);
	recvMsgSize = 0;
	for (;;) {
		int	res = select(1, &read_fs, 0, 0, (end_found == 0) ? &tvread : &tvread2);

		if (SOCKET_ERROR == res) 
		{
			fprintf(stderr, "\nselect() error\n"); 
			closesocket(sockfd);
			return 1;
		}
		if (!res) 
		{
//			if (DEBUG > 1) fprintf(stderr, "\nselect() read timeout\n");
			break;
		}
		
		if (!(res = recv(sockfd, &recvBuffer[recvMsgSize], TCPC_RCVBUFSIZE-recvMsgSize, 0))) 
		{
//			if (DEBUG > 1) fprintf(stderr, "\nserver closed the connection\n");
			break;
		}

		//check for socket error
		if (SOCKET_ERROR == res) 
		{
			fprintf(stderr, "\nrecv() error\n"); 
			closesocket(sockfd);
			return 1;
		}

		//printf("recv res %d\n",res);
		//fflush(stdout);
		// check for known termination of chunked transfer
		recvMsgSize += res;
		recvBuffer[recvMsgSize] = '\0';
		if (strstr(&recvBuffer[recvMsgSize-8], "0000\r\n\r\n")) 
		{
			//printf("end found\n");
			//fflush(stdout);
			end_found = 1;
		}

	}

	recvBuffer[recvMsgSize] = '\0';

	//printf("%d ",recvMsgSize);
	//for (i=0;i<10;i++) printf("%02x ",recvBuffer[recvMsgSize-10+i]);
	//printf("\n");

    closesocket(sockfd);

	// now parse content looking for he http body
	body_start = strstr(recvBuffer,"\r\n\r\n");
	if (body_start != NULL) 
	{ 
		*body_start = '\0';	// terminate header
		body_start += 4;
	}
	fflush(stdout);

	isChunked = isDecoded = false;

	// decode body if required
	if (decode && body_start != NULL)
	{
		if (strstr(recvBuffer, "chunked"))
		{
			char *ps,*p,*pc;
			int value=0;

			ps = body_start;
			isChunked = true;

			p = ps;

			pc = decodedBuffer;
			decodeLength = 0;
			while (*p !='\0') 
			{
				// parse through body removing chunked headers
				// for chunked format xxxxxxx\r\n where xxxx is a hex digit 0-9A-F
				ps = p;
				while (isxdigit(*p)) p++;
				if ((p[0] != '\r') || (p[1] != '\n')) return 0; 

				value = strtol(ps,NULL,16);
				if (value == 0) 
				{
					isDecoded = true;
					return 0;
				}
				p += 2;	// skip \r\n at end of chunk length
				ps = p; // store start of string
				p += value; // skip string
				// make sure string ends with \r\n
				if ((p[0] != '\r') || (p[1] != '\n')) return 0;

				// all ok so copy data to decoded buffer
				memcpy(pc, ps, value );
				decodeLength += value;
				pc += value;
				*pc = '\0';
				p += 2;	// skip \r\n
			}
			if (decodeLength>0) isDecoded = true;
		}
	}

    return 0;

}