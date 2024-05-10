//Taiyo Williamson, 20688536
//server-side of client-server application to detect network compression

//header files
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>

//global constants, hard-coding ports rather than command-line arguments
#define PRE_TCP_PORT 7777
#define BUFFER_MAX 1024
#define ITEMS 12
#define DEF_THRESHOLD 0.1


//struct to hold json line items
typedef struct {
    char *key;
    char *value;
} jsonLine;

int rec_UDP (int SRC_PORT, int SERVER_PORT, int INTER_TIME, int TRAIN_SIZE, int WAIT_TIME); //server_side, receives UDP packets, calculates difference in times, returns bool of whether network compression or not
char *est_TCP (int pre_post, int detect, int PRE_PORT, int POST_PORT); //handles everything to establish TCP connection and store JSON file in form of char buffer

int main (int argc, char *argv[]) {

    //Pre-Probing TCP Connection Phase
    int pre_post = 1;
    int temp = 0;
    //serialized buffer containing config.json
    char *msg = est_TCP(pre_post, temp, PRE_TCP_PORT, 1234); //1234 is a placeholder, it's not actually used in this instance call
    jsonLine items[ITEMS]; //will store config.json in here

    char *token;
    int i = 0;
    //parsing message to use server config info
    token = strtok(msg, ":");
    while (token != NULL) {
        (items + i)->key = token;
        printf("%s\n", token);
        token = strtok(NULL, ":");
        (items + i)->value = token;
        printf("%s\n", token);
        token = strtok(NULL, ":");
        i++;
    }

    //printf("%s\n", items[0].value);

    //Probing Phase
    //initializing values
    int dest_port = atoi(items[2].value);
    int src_port = atoi(items[1].value);
    int inter_time = atoi(items[8].value);
    int train_size = atoi(items[9].value);
    int wait_time = atoi(items[10].value);
    int detect = rec_UDP(src_port, dest_port, inter_time, train_size, wait_time); //detect holds data that will be sent back to client
    int TCP_pre_port = atoi(items[5].value);
    int TCP_post_port = atoi(items[6].value);


    //Post-Probing TCP Connection Phase
    //pre_post == 0 means program knows to send data, not receive in this TCP connection
    pre_post = 0;
    char* placeholder = est_TCP(pre_post, detect, TCP_pre_port, TCP_post_port); //sends detect back to client, who decodes and returns output

    //free malloc'd memory
    free(msg);
    free(placeholder);
    return EXIT_SUCCESS;
}

char *est_TCP(int pre_post, int detect, int PRE_PORT, int POST_PORT) {
    //similar logic to in client.c

    //creating socket
    struct sockaddr_in addr;
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket fail\n");
        exit(EXIT_FAILURE);
    }
    //filling server info, will use to bind socket
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    //pre_post determines TCP_port PREPROB or POSTPROB
    if (pre_post)
        addr.sin_port = htons(PRE_PORT);
    else    
        addr.sin_port = htons(POST_PORT);

    //bind socket, error handling
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("cannot bind socket to address\n");
        exit(0);
    }

    //listen for TCP connect requests
    if (listen(sockfd, 10) < 0) {
        printf("error listening\n");
        exit(0);
    }
    printf("server is listening\n");

    //fill in server info, accept request
    struct sockaddr_in server_addr;
    int client_sock;
    socklen_t addr_len = sizeof(server_addr);
    client_sock = accept(sockfd, (struct sockaddr *)&server_addr, &addr_len);
    if (client_sock < 0) {
        printf("error accepting connection\n");
        exit(0);
    }
    else 
        printf("Accepted connection\n");

    //first TCP, receiving JSON file
    if (pre_post) {
        int received;
        char *BUFFER = (char *) malloc(sizeof(char) * BUFFER_MAX);
        if ((received = recv(client_sock, BUFFER, BUFFER_MAX,0) < 0)) { //error handling
            printf("recv() failed\n");
        }
        else if (received == 0) {
            printf("sender has closed connection\n");
        }
        else {
            printf("received successfully!\n");
        }
        return BUFFER;
    }
    //second TCP connection, sending data back
    else {
        char *BUFFER2 = (char *) malloc(sizeof(char) * BUFFER_MAX);
        //copying server's findings to buffer, sending buffer back to client (will only be either 1 or 0)
        sprintf(BUFFER2, "%d", detect);
        int count1 = send(client_sock, BUFFER2, strlen(BUFFER2), 0);
        if (count1 == -1) { //error handling
            printf("error sending message to client\n");
            exit(0);
        }
        else 
            printf("Sent result to client!\n");
        return BUFFER2;
    }
    //edge case
    return NULL;
}

int rec_UDP(int SRC_PORT, int SERVER_PORT, int INTER_TIME, int TRAIN_SIZE, int WAIT_TIME) {

    //basic idea:
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


    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(struct sockaddr_in);

    //creating socket
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("error with creating socket\n");
        exit(0);
    }

    //fill server info
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { //binding the socket
        printf("server.c 193: bind failed\n");
        exit(0);
    }

    //update statement
    printf("Server is starting listen() for UDP packets\n");
    //LOW ENTROPY PAYLOAD
    //first received UDP packet
    int rec_first;
    char buffer[BUFFER_MAX]; //initializing buffer to receive payloads, but no handling the packets, placeholder
    while ((rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        continue;
    }
    printf("Received first packet! Starting low entropy timer\n");
    long double msec = 0;
    int rec_last = 0;
    int pak = 0;
    //starting the timer while still receiving packets
    clock_t before = clock();

        do {
            clock_t difference = clock() - before;
            msec = difference / CLOCKS_PER_SEC;
            if (((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0))
                printf("FAILED to receive packet\n");
            pak++;
        } while(msec <= INTER_TIME && pak <= TRAIN_SIZE);
    //stop timer
    clock_t after = clock() - before;
    long double low_entropy = after / CLOCKS_PER_SEC;
    printf("Received low entropy payload! Time is: %Lf\n", low_entropy);


    sleep(WAIT_TIME - 2);
    //HIGH ENTROPY PAYLOAD (same as low entropy payload)
    msec = 0, pak = 0;
    while ((rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        continue;
    }
    printf("Received first packet! Starting high entropy timer\n");
    before = clock();
        do {
            clock_t difference = clock() - before;
            msec = difference / CLOCKS_PER_SEC;
            if (((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0))
                printf("FAILED to receive packet\n");
            pak++;
        } while(pak <= TRAIN_SIZE && msec <= INTER_TIME);

    clock_t after2 = clock() - before;
    long double high_entropy = after2 / CLOCKS_PER_SEC;
    printf("Received high entropy payload! Time is: %Lf\n", high_entropy);
    
    //#define 100 ms as the threshold
    if ((high_entropy - low_entropy) >= DEF_THRESHOLD) {
        return 1;
    }
    else {
        return 0;
    }

}
