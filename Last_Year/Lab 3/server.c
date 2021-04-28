#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/wait.h>
#define MAXPENDING 5
#define BUFSIZE 32

int main()
{
	int i = 0;
	int clientSocket;
	/*Step 1: Create a socket on the server side*/
	int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverSocket < 0)
	{
		printf("Error in opening the server socket\n");
		exit(-1);
	}
	printf("Server socket opened successfully\n");

	/*Step 2: Create a local address structure for the server, it should be same as that given to the client*/
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(12345);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Server address assigned successfully\n");

	/*Step 3: Assign the port number to the socket using bind()*/
	int temp = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	if(temp<0)
	{
		printf("Error in binding the server socket\n");
		exit(-1);
	}
	printf("Binding successful\n");

	/*Step 4: Tell the system to allow connections to be made to the port using listen()*/
	int temp1 = listen(serverSocket, MAXPENDING);
	if(temp1<0)
	{
		printf("Error in listen\\n");
		exit(-1);
	}
	printf("Listening ...\n");

	pid_t pid;
	pid = fork();

	if(pid == 0)	//The 1st child
	{
		i = 0;
	}
	else if (pid > 0)	//The parent
	{
		int status = 0;
		pid_t childPid;

		childPid = wait(&status);

		pid = fork();

		if(pid == 0)
		{
			i = 1;
		}
		else if (pid > 0)
		{
			childPid = wait(&status);
			i = 2;
		}
		else
		{
			printf("Error while forking() for the 2nd child\n");
			exit(-1);
		}
	}
	else
	{
		printf("Error while forking() for the 1st child\n");
		exit(-1);
	}

	/*Step 5: Call accept() to get a new socket for each client connection*/
	char msg[BUFSIZE];
	int clientLength = sizeof(clientAddress);
	clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
	close(serverSocket);
	if(clientLength < 0)
	{
		printf("Error in client socket\n");
		exit(-1);
	}
	/*Step 6: Communicate with the client using send() and recv()*/
	printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));
	int temp2 = recv(clientSocket, msg, BUFSIZE-1, 0);
	if(temp2 < 0)
	{
		printf("Error while receiving the msg from the client\n");
		exit(-1);
	}
	msg[temp2] = '\0';
	printf("Message received : %s\n",msg);
	printf("Enter message for the client:\t");
	scanf("%[^\n]%*c", msg);
	int bytesSent = send(clientSocket, msg, strlen(msg), 0);
	if(bytesSent != strlen(msg))
	{
		printf("Error while sending the data to the client\n");
		exit(-1);
	}
	printf("Message successfully sent to the client\n");

	/*Step 7: Close the server and the client socket*/
	int temp3 = close(clientSocket);
	if(temp3<0)
	{
		printf("Error in closing the clientSocket\n");
		exit(-1);
	}
	printf("clientSocket closed successfully\n");
	/*int temp4 = close(serverSocket);
	if(temp4<0)
	{
		printf("Error in closing the serverSocket\n");
		exit(-1);
	}
	printf("serverSocket closed successfully\n");*/
	return 0;
}
