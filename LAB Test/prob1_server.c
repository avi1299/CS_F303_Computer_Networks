/* Simple udp server with stop and wait functionality 
    Avinash Sontakke
    2017B5A70667P
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 8882   //The port on which to listen for incoming data
#define PDR 0.1 

typedef enum { true, false} bool;

bool toDiscard;

void discardRandom(){
    if((rand()%100) < PDR*100){
        toDiscard = true;
    }
    else{
        toDiscard = false;
    }
}

void error_exit(char *s){
    perror(s);
    exit(1);
}

char ackkk[BUFLEN]="\0";
// typedef struct packet1{
//     int sq_no;
// }ACK_PKT;

// typedef struct packet2{
//     int sq_no;
//     char data[BUFLEN];
// }DATA_PKT;

typedef struct packet{
    int sq_no;
    char data[BUFLEN];
    int length;
    bool ACK_or_DATA;
    int last_packet;
}PKT;

int main(void)
{
    struct sockaddr_in serverAddr, clientAddr;
    int sockfd, i, slen = sizeof(clientAddr) , recv_len;
    //char buf[BUFLEN];
    PKT rcv_pkt,ack_pkt;
    ack_pkt.ACK_or_DATA = true;
    ack_pkt.length=0;
    ack_pkt.last_packet=0;
    *ack_pkt.data = *ackkk;

    /* Open the file that we wish to transfer */ 
    FILE *fp = fopen("output.txt","w");
    if(fp==NULL)
    {
        printf("File open error");
        return 1;
    }
    fclose(fp) ;     


    //create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        error_exit("socket");
    }
    // zero out the structure
    memset((char *) &serverAddr, 0, sizeof(serverAddr));
     
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


    int seq =0;
     
    //bind socket to port
    if( bind(sockfd , (struct sockaddr*)&serverAddr, sizeof(serverAddr) ) == -1){
        error_exit("bind");
    }
    int state =0;
    while(1)
    {	     
        // printf("current state is %d\n", state);
        switch(state)
        {  
            case 0:
            {
                //printf("Waiting for packet %d from sender...\n",seq);
                fflush(stdout);
                discardRandom();

                //try to receive some data, this is a blocking call
                if ((recv_len = recvfrom(sockfd, &rcv_pkt, BUFLEN, 0, (struct sockaddr *) &clientAddr, &slen)) == -1){
                    error_exit("recvfrom()");
                }
                
                if (rcv_pkt.sq_no == seq && toDiscard == false){  
                    printf("RCVD DATA:Seq.No %d of size %ld Bytes\n",seq, strlen(rcv_pkt.data));
                        FILE *fp = fopen("output.txt","a");
                        if(fp==NULL)
                        {
                            printf("File open error");
                            return 1;
                        }                    
                    ack_pkt.sq_no = ++seq;
                    fputs(rcv_pkt.data,fp);
                    fclose(fp);
                    //printf("rcv_pkt.last packet=%d",rcv_pkt.last_packet);
                    ack_pkt.last_packet=rcv_pkt.last_packet;
                    
                    if (sendto(sockfd, &ack_pkt, recv_len, 0, (struct sockaddr*) &clientAddr, slen) == -1){
                        error_exit("sendto()");
                    }
                    state = 0;
                    printf("SENT ACK:for PKT with Seq.No. %d\n",seq-1);
                    //printf("Packet received with seq. no. %d and Packet content is = %s last packet=%d\n",rcv_pkt.sq_no, rcv_pkt.data,rcv_pkt.last_packet);
                    if( rcv_pkt.last_packet == 1) 
                        return 0;
                    break;
                    
                }
                else
                    printf("DROP DATA:Seq.No %d of size %ld Bytes\n",seq, strlen(rcv_pkt.data));
                // else{   // send ACK1 and remain in this state
                //     ack_pkt.sq_no = 1;
                    
                //     if (sendto(sockfd, &ack_pkt, recv_len, 0, (struct sockaddr*) &clientAddr, slen) == -1){
                //         error_exit("sendto()");
                //     }
                // }                
            }
            break;
            // case 1:
            // {   
            //     printf("Waiting for packet 1 from sender...\n");
            //     fflush(stdout);

            //     //try to receive some data, this is a blocking call
            //     if ((recv_len = recvfrom(sockfd, &rcv_pkt, BUFLEN, 0, (struct sockaddr *) &clientAddr, &slen)) == -1){
            //         error_exit("recvfrom()");
            //     }               

            //     if (rcv_pkt.sq_no == 1 && toDiscard == false){                     
            //         ack_pkt.sq_no = 1;
                    
            //         if (sendto(sockfd, &ack_pkt, recv_len, 0, (struct sockaddr*) &clientAddr, slen) == -1){
            //             error_exit("sendto()"); 
            //         }
            //         state = 0;
            //         printf("Packet received with seq. no.= %d and Packet content is= %s\n",rcv_pkt.sq_no, rcv_pkt.data);
            //         break;
            //     }
            //     else{   //send ACK0  and remain in this state
            //         ack_pkt.sq_no = 0;
                    
            //         if (sendto(sockfd, &ack_pkt, recv_len, 0, (struct sockaddr*) &clientAddr, slen) == -1){
            //             error_exit("sendto()"); 
            //         }
            //     }                
            // }   
            // break;
        }
    }
    close(sockfd);
    return 0;
}