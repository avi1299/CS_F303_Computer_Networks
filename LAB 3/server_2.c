#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAXPENDING 5
#define BUFFERSIZE 32
int main ()
{
    /*CREATE A TCP SOCKET*/
    int serverSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) 
    { 
        printf ("Error while server socketcreation"); 
        exit (0); 
    }
    printf ("Server Socket Created\n");

    /*CONSTRUCT LOCAL ADDRESS STRUCTURE*/
    struct sockaddr_in serverAddress, clientAddress;
    memset (&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    printf ("Server Address %s\n", inet_ntoa(serverAddress.sin_addr));
    printf ("Server address assigned\n");
    int temp = bind(serverSocket, (struct sockaddr*) &serverAddress,sizeof(serverAddress));
    if (temp < 0)
    { 
        printf ("Error while binding\n");
        exit (0);
    }
    printf ("Binding successful\n");
    int temp1 = listen(serverSocket, MAXPENDING);
    if (temp1 < 0)
    { 
        printf ("Error in listen");
        exit (0);
    }
    printf ("Now Listening\n");
    char msg[BUFFERSIZE];
    float* numb;
    int clientLength = sizeof(clientAddress);
    int clientSocket = accept (serverSocket, (struct sockaddr*) &clientAddress, &clientLength);
    if (clientLength < 0) 
    {
        printf ("Error in client socket"); 
        exit(0);
    }
    printf ("Handling Client %s\n", inet_ntoa(clientAddress.sin_addr));
    int temp2 = recv(clientSocket, numb, sizeof(float), 0);
    if (temp2 < 0)
    { 
        printf ("problem in temp 2");
        exit (0);
    }
    printf ("%f\n", *numb);
    *numb+=1;
    //printf ("Length of message i.e strlen(msg): %ld\n",strlen(msg));
    printf ("Message Length received i.e temp2: %d\n", temp2);
    printf("ENTER MESSAGE FOR CLIENT\n");
    //gets (msg);
    int bytesSent = send (clientSocket,numb,sizeof(float),0);
    if (bytesSent != sizeof(float))
    { 
        printf ("Error while sending message to client");
        exit(0);
    }
    close(serverSocket);
    close(clientSocket);
}