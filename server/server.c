#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define PRE_PORT  7777
#define POST_PORT 6666
#define BUFFER_MAX 1024

void rec_UDP ();
char *est_TCP (int pre_post, int detect);
int main (int argc, char *argv[]) {

    //Pre-Probing TCP Connection Phase

    //server listens for incoming connection from client
    //receive json file, store in variable
    int pre_post = 1;
    int temp = 0;
    char *msg = est_TCP(pre_post, temp);



    //Probing Phase

    //connections now been established
    //start inter_time, same time as client
        //while inter_time != 0, listen for low-entropy UDP packets
    //receive first UDP packet, record timestamp for first arrival
    //receive last packet, how will we know? either 6000th packet? or record timestamp of every packet until inter_time == 0?
        //record timestamp for last arrival
        //calculate time of low_entropy_payload with last - first timestamp

    
    //restart inter_time, same time as client (for high entropy payload)
        //while inter_time != 0, receive high-entropy UDP packets
    
    //receive first UDP packet, record timestamp for first arrival
    //receive last packet, same logic
        //when coding this, ask: is this compartmentalizable? turn into function, with double call: one for low entropy, one for high entropy
        //calculate time of high_entropy_payload


    //NOW: before listening for second TCP connection: calculations
    //IF (high_entropy_payload - low_entropy_payload) >= 100 ms,
        //store int res = 1 and send that msg back to client
        //ELSE: store res = 0 and send that msg back to client


    //Post-Probing TCP Connection Phase

    return EXIT_SUCCESS;
}

char *est_TCP(int pre_post, int detect) {
    struct sockaddr_in addr;
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    if (pre_post)
        addr.sin_port = htons(PRE_PORT);
    else    
        addr.sin_port = htons(POST_PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("cannot bind socket to address");
        exit(0);
    }

    if (listen(sockfd, 1) < 0) {
        printf("error listening");
        exit(0);
    }
    struct sockaddr_in server_addr;
    int client_sock, addr_len = sizeof(server_addr);
    client_sock = accept(sockfd, (struct sockaddr *)&server_addr, &addr_len);
    if (client_sock < 0) {
        printf("error accepting connection");
        exit(0);
    }

    if (pre_post) {
        int received;
        char *BUFFER[BUFFER_MAX];
        if ((received = recv(sockfd, BUFFER, BUFFER_MAX,0) < 0)) {
            printf("recv() failed");
        }
        else if (received == 0) {
            printf("sender has closed connection");
        }
        else {
            printf("received successfully!");
            return BUFFER;
        }
    }
    else {
        char *BUFFER2[2];
        //copying server's findings to buffer, sending buffer back to client (will only be either 1 or 0)
        sprintf(BUFFER2, "%d", detect);
        int count1 = send(sockfd, BUFFER2, BUFFER_MAX, 0);
        if (count1 == -1) {
            printf("error sending message to client");
            exit(0);
        }
    }
    return NULL;
}

void rec_UDP() {
    
}