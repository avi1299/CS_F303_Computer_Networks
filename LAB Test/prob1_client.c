/*
    Simple udp client with stop and wait functionality
    Avinash Sontakke
    2017B5A70667P
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8882   //The port on which to send data
#define TIMEOUT 2

typedef enum { true, false} bool;

//bool toDiscard;

// void discardRandom(){
//     if(rand() > RAND_MAX/2){
//         toDiscard = true;
//     }
//     else{
//         toDiscard = false;
//     }
// }

// typedef struct packet1{
//     int sq_no;
// }ACK_PKT;

typedef struct packet{
    int sq_no;
    char data[BUFLEN];
    int length;
    bool ACK_or_DATA;
    int last_packet;
}PKT;

void error_exit(char *s){
    perror(s);
    exit(1);
}
 
int main(void){
    struct sockaddr_in serverAddr;
    int sockfd, i, slen=sizeof(serverAddr);

    char buf[BUFLEN];
    char message[BUFLEN];

    PKT send_pkt,rcv_ack;

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        error_exit("socket");
    }
 
    memset((char *) &serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int seq=0;
    int ack=0;
    int state = 0;
    int siz=0;

    int doclose=0;
    /* Open the file that we wish to transfer */
    FILE *fp = fopen("input.txt","r");
    if(fp==NULL)
    {
        printf("File open error");
        return 1;
    }

    while(1)
    {
        // printf("current state is %d\n", state);
        switch(state)
        {
            case 0: 
            {
                fgets(send_pkt.data, BUFLEN, fp);
                send_pkt.length=strlen(send_pkt.data);
                send_pkt.sq_no = seq;
                send_pkt.ACK_or_DATA = false;
                send_pkt.last_packet=doclose;
                printf("SENT DATA:Seq.No %d of size %ld Bytes\n",seq, strlen(send_pkt.data));//wait for sending packet with seq. no. 0
                //printf("Sent last packt = %d: ",send_pkt.last_packet);
                
                if (feof(fp))
                    {
                        fclose(fp); 
                        doclose=1;
                        send_pkt.last_packet=1 ;
                    }
                // if(feof(fp))
                //     send_pkt.last_packet = 1;
                // else
                //     send_pkt.last_packet = 0;

                if (sendto(sockfd , &send_pkt, sizeof(send_pkt), 0 , (struct sockaddr *) &serverAddr, slen) == -1){
                    error_exit("sendto()");
                }
                state = 1;
            }
            break; 

            case 1:
            {//waiting for ACK 0

                //discardRandom();
                fd_set rcvSet;
                int n;
                
                struct timeval tv;

                FD_ZERO(&rcvSet);
                FD_SET(sockfd, &rcvSet);

                tv.tv_sec = TIMEOUT;
                tv.tv_usec = 0;

                if((n = select(sockfd + 1, &rcvSet, NULL, NULL, &tv) ) < 0){
                    error_exit("error on select");
                }
                
                if(n == 0) {     // timeout expired, send packet again and remain in this state
                    printf("RESENT DATA: Seq.No %d of size %ld Bytes\n",seq, strlen(send_pkt.data));
                    if (sendto(sockfd , &send_pkt, sizeof(send_pkt), 0 , (struct sockaddr *) &serverAddr, slen) == -1){
                        error_exit("sendto()");
                    }
                    break;
                }
                
                // socket is readable => ack arrived
                if (recvfrom(sockfd , &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *) &serverAddr, &slen) == -1){
                    error_exit("recvfrom()");
                }               

                if(rcv_ack.sq_no == (seq+1)){  // if ACK0 arrived, change the state   
                    printf("RCVD ACK:for PKT with Seq.No. %d\n",seq);
                    seq++;                     
                    state = 0; 
                    if (doclose == 1)
                    {
                        //fclose(fp);
                        //printf("Received ack seq. no. %d\n\n",rcv_ack.sq_no);
                        return 0;    
                    }           
                }
                else{
                    break;
                }

                //printf("Received ack seq. no. %d  received last packet %d\n\n",rcv_ack.sq_no,rcv_ack.last_packet);
            }
            break;

            // case 2:
            // {
            //     printf("Enter message 1: ");  
            //     //wait for sending packet with seq. no. 1
            //     fgets(send_pkt.data, BUFLEN, stdin);

            //     send_pkt.sq_no = 1;		
            //     if (sendto(sockfd, &send_pkt, sizeof(send_pkt) , 0 , (struct sockaddr *) &serverAddr, slen)==-1){
            //         error_exit("sendto()");
            //     }
            //     state = 3; 
            // }
            // break;                    

            // case 3:	//waiting for ACK 1
            // {
            //     discardRandom();

            //     fd_set rcvSet;
            //     int n;
                
            //     struct timeval tv;

            //     FD_ZERO(&rcvSet);
            //     FD_SET(sockfd, &rcvSet);

            //     tv.tv_sec = TIMEOUT;
            //     tv.tv_usec = 0;

            //     if((n = select(sockfd + 1, &rcvSet, NULL, NULL, &tv) ) < 0){
            //         error_exit("error on select");
            //     }
                
            //     if(n == 0) {     // timeout expired, send packet again and remain in this state
            //         printf("Timeout occured\n");
            //         if (sendto(sockfd , &send_pkt, sizeof(send_pkt), 0 , (struct sockaddr *) &serverAddr, slen) == -1){
            //             error_exit("sendto()");
            //         }
            //         break;
            //     }
                
            //     // socket is readable => ack arrived
            //     if (recvfrom(sockfd , &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *) &serverAddr, &slen) == -1){
            //         error_exit("recvfrom()");
            //     }               

            //     if (rcv_ack.sq_no == 1 && toDiscard == false){  // ACK1 has arrived, change the state                        
            //         state = 0;                     
            //     }
            //     else{
            //         break;
            //     }

            //     printf("Received ack seq. no. %d\n\n",rcv_ack.sq_no);                     
            // }
            // break;
        }
    }
    if( close(sockfd) < 0){
        error_exit("close");
    }
    return 0;
}