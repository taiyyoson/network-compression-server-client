//Taiyo Williamson, 20688536
//server-side of client-server application to detect network compression

//header files
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>

//global constants
#define BUFFER_MAX 1024
#define ITEMS 12
#define DEF_THRESHOLD 100


//struct to hold json line items
typedef struct 
{
    char *key;
    char *value;
} jsonLine;

//functions made, proper descriptions given at actual function bodies
int rec_UDP (int SRC_PORT, int SERVER_PORT, int INTER_TIME, int TRAIN_SIZE, int WAIT_TIME); //server_side, receives UDP packets, calculates difference in times, returns bool of whether network compression or not
char *est_TCP (int pre_post, int detect, int PRE_PORT, int POST_PORT); //handles everything to establish TCP connection and store JSON file in form of char buffer

/***
 * main is where all the code is ran, and actually calls the functions to receive TCP and UDP packets
 * argc is the number of arguments given on the cmd line
 * argv is an array of strings allocated for each argument on the cmd line
*/
int main (int argc, char *argv[]) 
{
    //same as client-side, pre_post == 1 if pre_probe, pre_post == 0 if post-probe
    int pre_post = 1;
    int temp = 0; //this is a placeholder for what is sent back to the client during post-probe phase, don't use it in pre-probe phase

    //getting port for pre-probe
    int PRE_TCP_PORT;
    if (argc < 2) 
    {
        printf("Missing PRE TCP PORT in cmd line arg!\n");
        exit(EXIT_FAILURE);
    }
    PRE_TCP_PORT = atoi(argv[1]);

    //Pre-Probing TCP Connection Phase
    char *msg = est_TCP(pre_post, temp, PRE_TCP_PORT, 1234); //1234 is a placeholder, it's not actually used in this instance call
    jsonLine items[ITEMS]; //will store config.json in here

    char *token;
    int i = 0;
    //parsing message to use server config info
    token = strtok(msg, ":");
    while (token != NULL) 
    {
        (items + i)->key = token;
        //printf("%s\n", token);
        token = strtok(NULL, ":");
        (items + i)->value = token;
        //printf("%s\n", token);
        token = strtok(NULL, ":");
        i++;
    }
    //confirmation that config.json was successfully transmitted to the server
    printf("Received config file! info parsed correctly: \n");
    for (int i=0; i < ITEMS; i++) 
    {
        printf("%s: %s\n", items[i].key, items[i].value);
    }

    //initializing values
    int dest_port = atoi(items[2].value);
    int src_port = atoi(items[1].value);
    int inter_time = atoi(items[8].value) * 1000;
    int train_size = atoi(items[9].value);
    int wait_time = atoi(items[10].value);
    int TCP_pre_port = atoi(items[5].value);
    int TCP_post_port = atoi(items[6].value);
    //Probing Phase
    int detect = rec_UDP(src_port, dest_port, inter_time, train_size, wait_time); //detect holds data that will be sent back to client



    //pre_post == 0 means program knows to send data, not receive in this TCP connection
    pre_post = 0;
    printf("Did this server-client have network compression links? %d\n", detect);
    //Post-Probing TCP Connection Phase
    char* placeholder = est_TCP(pre_post, detect, TCP_pre_port, TCP_post_port); //sends detect back to client, who decodes and returns output

    //free malloc'd memory, DONE w/ PT 1 server-side!
    free(msg);
    free(placeholder);
    return EXIT_SUCCESS;
}


/***
 * est_TCP, same as client.c, est_TCP makes a socket, fills the necessary info to send a message to a server, and 
 *  either sends or receives a buffer through TCP connection
*/
char *est_TCP(int pre_post, int detect, int PRE_PORT, int POST_PORT) 
{
    //similar logic to in client.c

    //creating socket and server info
    struct sockaddr_in addr;
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) //basic error handling
    {
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


    //using setsockopt to reuse address and port in case prior crashes happen, since socket is bound for a certain time after crashing
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0) 
    {
        printf("couldn't setsockopt to reuse ADDR\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)   
    {
        printf("couldn't setsockopt to reuse PORT\n");
        exit(EXIT_FAILURE);
    }

    //bind socket, basic error handling
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
    {
        printf("cannot bind socket to address\n");
        exit(EXIT_FAILURE);
    }

    //listen for TCP connect requests
    if (listen(sockfd, 10) < 0) //backlog = 10, can queue up to 10 connections, in case of arbitrary random connections 
    {
        printf("error listening\n");
        exit(EXIT_FAILURE);
    }
    //update statement, prints to console to keep user updated
    printf("server is listening\n");

    //accept connect request, the data from the client is stored in server_addr
    struct sockaddr_in server_addr;
    int client_sock; //also connection is stored in new socket
    socklen_t addr_len = sizeof(server_addr);
    client_sock = accept(sockfd, (struct sockaddr *)&server_addr, &addr_len);
    if (client_sock < 0) //basic error handling
    {
        printf("error accepting connection\n");
        exit(EXIT_FAILURE);
    }
    else 
        printf("Accepted connection\n");



    //first, aka pre-probing phase TCP, receiving JSON file
    if (pre_post) {
        int received;
        char *BUFFER = (char *) malloc(sizeof(char) * BUFFER_MAX); //initalize buffer at least large enough to store evrything
        //basic error handling, receiving TCP connection
        if ((received = recv(client_sock, BUFFER, BUFFER_MAX,0) < 0)) 
        {
            printf("recv() failed\n");
            exit(EXIT_FAILURE);
        }
        else if (received == 0) 
        {
            printf("sender has closed connection\n");
        }
        else 
        {
            printf("received successfully!\n");
        }
        return BUFFER;
    }
    //second TCP connection, aka post-probing phase, sending data back
    else {
        //iniatilize buffer of BUFFER_MAX size, but only BUFFER2[0] is being used
        char *BUFFER2 = (char *) malloc(sizeof(char) * BUFFER_MAX);
        sprintf(BUFFER2, "%d", detect); //copying server's findings to buffer, sending buffer back to client (will only be either 1 or 0)
        //send back to client, basic error handling
        int count1 = send(client_sock, BUFFER2, strlen(BUFFER2), 0);
        if (count1 == -1) 
        {
            printf("error sending message to client\n");
            exit(EXIT_FAILURE);
        }
        else 
            printf("Sent result to client!\n");
        return BUFFER2;
    }
    //edge case
    return NULL;
}



/***
 * rec_UDP handles whole probing phase on server side. creates socket to receive low and high entropy payloads
 * SRC_PORT is the UDP src_port, tho this isn't used
 * SERVER_PORT is the UDP dest port, which server uses to bind socket
 * INTER_TIME is ultimately not used, since remember client.c works about the same time as server.c
 *  but in case recvfrom() breaks down, INTER_TIME acts as a manual timeout so that it doesn't receive packets indefinitely
 * TRAIN_SIZE is the number of packets being sent (6000)
 * WAIT_TIME isn't used
*/
int rec_UDP(int SRC_PORT, int SERVER_PORT, int INTER_TIME, int TRAIN_SIZE, int WAIT_TIME) 
{

    //creating socket and struct
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(struct sockaddr_in);

    //creating socket, basic error handling
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        printf("error with creating socket\n");
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    //creates timeout for recvfrom. if recvfrom waits longer than 1/2 a sec, returns -1 error
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) { //basically an inactivity timer
        printf("error with setting timeout\n");
        exit(EXIT_FAILURE);
    }


    //fill server info
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    //binding socket so the server knows where to look for these UDP packets
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    { 
        printf("server.c 250: bind failed\n");
        exit(EXIT_FAILURE);
    }

    //update statement
    printf("Server is starting listen() for UDP packets\n");
    

    //LOW ENTROPY PAYLOAD
    //first received UDP packet
    int rec_first;
    char buffer[BUFFER_MAX]; //initializing buffer to receive payloads, but no handling the packets, placeholder

    //since there's a timeout, until the client sends its first UDP packet, server can wait/listen
    while ((rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0) 
    {
        continue;
    }
    printf("Received first packet! Starting low entropy timer\n"); //broke out of while loop
    //starting the timer while still receiving packets
    long double msec = 0;
    int rec_last = 0;
    clock_t before = clock();
        //receiving low entropy payload, updating time taken from first UDP packet, basically working timer
        do 
        {
            clock_t difference = clock() - before;
            msec = difference * 1000 / CLOCKS_PER_SEC;
            rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len);
            if (rec_last <= 0)
                break;
        } while(msec <= INTER_TIME); //manual timeout, in case something goes wrong
    //stop timer
    clock_t after = clock() - before;
    long double low_entropy = (after * 1000 / CLOCKS_PER_SEC);
    printf("Received low entropy payload! Time is: %Lf\n", low_entropy);



    //HIGH ENTROPY PAYLOAD (same as low entropy payload)
    msec = 0;
    while ((rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) < 0) 
    {
        continue;
    }
    printf("Received first packet! Starting high entropy timer\n");
    before = clock();
        do 
        {
            clock_t difference = clock() - before;
            msec = difference * 1000 / CLOCKS_PER_SEC;
            rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len);
            if (rec_last <= 0)
                break;
        } while(msec <= INTER_TIME);

    clock_t after2 = clock() - before;
    long double high_entropy = (after2 * 1000 / CLOCKS_PER_SEC);
    printf("Received high entropy payload! Time is: %Lf\n", high_entropy);
    
    //#define 100 ms as the threshold
    if ((high_entropy - low_entropy) >= DEF_THRESHOLD) 
        return 1; //1 means there is network compression 1 == TRUE
    else 
        return 0; //0 means there is not network compression 0 == FALSE

}
