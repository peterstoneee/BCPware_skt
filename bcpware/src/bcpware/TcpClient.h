#pragma once

#define TCPC_RCVBUFSIZE 0x80000   

class TcpClient
{
protected:
	unsigned int recvMsgSize;                    /* Size of received message */
	char *body_start;			// pointer to the start of the http body
	bool decode;				// flag indicating the body should be decoded if received in chunked format

public:
	TcpClient(bool decode_body);
	char recvBuffer[TCPC_RCVBUFSIZE];        /* Buffer for received url FIXME. NEED to redo sizing */
	char decodedBuffer[TCPC_RCVBUFSIZE];     /* decoded buffer FIXME. NEED to redo sizing*/

	int TcpClient::send_tcpip_request(char *sendBuffer, unsigned int sendMsgSize, char *ip, unsigned int PORT);
	int TcpClient::send_tcpip_request(char *sendBuffer, unsigned int sendMsgSize, char *ip, unsigned int PORT, unsigned int timeouts);
	int TcpClient::recvSize(void);	// size of received message
	int TcpClient::bodySize(void);	// size of decoded body
	char * TcpClient::getHeader(void);	// received message
	char * TcpClient::getBody(void);	// received message body
	char * TcpClient::getDecodedBody(void);	// received message body decoded
	bool isChunked;				// flag indicating the data is chunk encoded
	bool isDecoded;				// flag indicating it has been successfully decoded
	int  decodeLength;			// decoded length


private:
	int TcpClient::recvTimeOutTCP( int socket, long sec, long usec );

};