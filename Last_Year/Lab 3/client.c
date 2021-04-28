#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define BUFSIZE 32

int main()
{

	/*STEP 1: Create a TCP socket*/
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sock<0)
	{
		printf("Error in opening the socket\n");
		exit(0);
	}
	printf("Client socket created successfully\n");

	/*Step 2: Create address structure for server*/
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(12345);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Address assigned\n");

	/*Step 3: Establish connection*/
	int c = connect(sock, (struct sockaddr *) (&serverAddr), sizeof(serverAddr));

	if(c<0)
	{
		printf("Error in connecting the socket to the server\n");
		exit(0);
	}

	printf("Connection established successfully\n");

	/*Step 4: Send/recv data*/
	printf("Enter the message for server (max 32 chars): \t");
	char msg[BUFSIZE];
	scanf("%[^\n]%*c",msg);
	int bytesSent = send(sock, (void *)msg, strlen(msg), 0);

	if(bytesSent != strlen(msg))
	{
		printf("Error in sending the message to the server\n");
		exit(0);
	}
	printf("Data sent to the server successfully\n");

	char recvBuffer[BUFSIZE];

	int bytesRecvd = recv(sock, recvBuffer, BUFSIZE-1, 0);
	recvBuffer[bytesRecvd] = '\0';
	printf("%s is the message from the server\n", recvBuffer);

	/*Step 5: Close the socket*/
	int x = close(sock);

	if(x<0)
	{
		printf("Error in closing the socket\n");
		exit(0);
	}
	printf("Socket closed successfully\n");
}
