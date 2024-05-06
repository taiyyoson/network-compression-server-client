#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PRE_PORT  7777
#define POST_PORT 6666
#define BUFFER_MAX 1024
#define ITEMS 11

typedef struct {
    char *key;
    const char *value;
} jsonLine;

int rec_UDP (int SRC_PORT, int SERVER_PORT, int INTER_TIME);
char *est_TCP (int pre_post, int detect);
int main (int argc, char *argv[]) {

    //Pre-Probing TCP Connection Phase

    //server listens for incoming connection from client
    //receive json file, store in variable
    int pre_post = 1;
    int temp = 0;
    char *msg = est_TCP(pre_post, temp);

    jsonLine items[ITEMS];

    char delim[2] = ",";
    char indelim[2] = ":";
    char* token;
    char* token2;


    token = strtok(msg, delim);

    int i=0;
    while((token != NULL) && (i < ITEMS)) {
        token2 = strtok(token, indelim);
        while (token2 != NULL) {
            strcpy(items[i].key, token2);
            token2 = strtok(NULL, indelim);
            strcpy(items[i].value, token2);
        }
        token = strtok(NULL, delim);
        i++;
    }
   



    //Probing Phase
    int dest_port = atoi(items[2].value);
    int src_port = atoi(items[1].value);
    int inter_time = atoi(items[8].value);
    int detect = rec_UDP(src_port, dest_port, inter_time);



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
    pre_post = 0;
    char* placeholder = est_TCP(pre_post, detect);

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

int rec_UDP(int SRC_PORT, int SERVER_PORT, int INTER_TIME) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    client_addr.sin_port = htons(SRC_PORT);
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_MAX];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0) < 0)) {
        printf("error with creating socket");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr) < 0)) {
        printf("bind failed");
        exit(0);
    }

    printf("Server is starting listen() for UDP packets");
    //LOW ENTROPY PAYLOAD
    //first received UDP packet

    int rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len);
    float sec = 0;
    int rec_last;
    clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
        } while(((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) > 0) && sec <= INTER_TIME);

    clock_t after = clock() - before;
    float low_entropy = after;

    //HIGH ENTROPY PAYLOAD
    sec = 0;
    rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len);
    before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
        } while(((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) > 0) && sec <= INTER_TIME);

    clock_t after2 = clock() - before;
    float high_entropy = after2;

    if ((high_entropy - low_entropy) >= 0.1) {
        return 1;
    }
    else {
        return 0;
    }

}
