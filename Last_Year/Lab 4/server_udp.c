/*  Simple udp server */

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<unistd.h> //close()
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
int generateRandom() 
{ 
    int count = 1;
    int lower = 1;
    int upper = 10;
    int i; 
    for (i = 0; i < count; i++) { 
        int num = (rand() % 
           (upper - lower + 1)) + lower; 
       	return num; 
    } 
} 
int main(void)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    int myNumber, clientGuess;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    int what = 0;
    for(int x=10; x>=1; x--)
    {
	myNumber = generateRandom();
	
        printf("Waiting for client to guess...");
        fflush(stdout);
	
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
	
	//print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %d\n" , (int)buf[0]-'0');

	clientGuess = (int)buf[0]-'0';
	
	memset(buf, '\0', BUFLEN);

	if(clientGuess == myNumber)
	{
		strcpy(buf, "Correct Guess! Client wins\n\0");
		what = 1;
		break;
	}
	else
	{
		strcpy(buf, "Wrong Guess!\n\0");
	}
         
        //now reply the client with the same data

        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
    if(what == 0)
    {
	memset(buf, '\0', BUFLEN);
	strcpy(buf, "Client loses! No more chances left\n\0");
    }
    if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    close(s);
    return 0;
}

