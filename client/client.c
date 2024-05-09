//Taiyo Williamson, 20688536
//client-side of client-server application to detect network compression

//header files
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "../cJSON.h"

//global constants 
#define DEFAULT_LENGTH 22
#define ADDR_LEN 14
#define ITEMS 11
#define BUFFER_MAX 1024

//struct to hold json line items
typedef struct {
    char *key;
    char *value;
} jsonLine;

//functions made
cJSON *JSONObj(char *input[]); //creates cJSON root variable that is used to make json calls
char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post); //handles everything to establish TCP connection and send JSON file in form of char buffer
void send_UDP (jsonLine *items); //handles everything to send the two UDP payload trains, one low entropy, one high entropy

int main(int argc, char *argv[]) {
   //parse given json file into struct
    if (argc < 2) {
        printf("missing JSON file in cmd line arg!\n");
        return EXIT_FAILURE;
    }
    //root to make JSON calls
    //array of structs, representing config.json
    cJSON *json = JSONObj(argv);
  
    // access the JSON data 
    cJSON *server_ip_addr = cJSON_GetObjectItemCaseSensitive(json, "server_ip_addr"); 
    //printf("%s\n", server_ip_addr->valuestring);
    cJSON *UDP_src_port = cJSON_GetObjectItemCaseSensitive(json, "UDP_src_port"); 
    cJSON *UDP_dest_port = cJSON_GetObjectItemCaseSensitive(json, "UDP_dest_port"); 
    cJSON *TCP_dest_port_headSYN = cJSON_GetObjectItemCaseSensitive(json, "TCP_dest_port_headSYN"); 
    cJSON *TCP_dest_port_tailSYN = cJSON_GetObjectItemCaseSensitive(json, "TCP_dest_port_tailSYN"); 
    cJSON *TCP_port_preProb = cJSON_GetObjectItemCaseSensitive(json, "TCP_port_preProb"); 
    cJSON *TCP_port_postProb = cJSON_GetObjectItemCaseSensitive(json, "TCP_port_postProb"); 
    cJSON *UDP_packet_size = cJSON_GetObjectItemCaseSensitive(json, "UDP_packet_size"); 
    cJSON *inter_time = cJSON_GetObjectItemCaseSensitive(json, "inter_time"); 
    cJSON *UDP_train_size = cJSON_GetObjectItemCaseSensitive(json, "UDP_train_size"); 
    cJSON *UDP_TTL = cJSON_GetObjectItemCaseSensitive(json, "UDP_TTL"); 

    jsonLine config[ITEMS] = {
      {"server_ip_addr", server_ip_addr->valuestring},
      {"UDP_src_port", UDP_src_port->valuestring},
      {"UDP_dest_port", UDP_dest_port->valuestring},
      {"TCP_dest_port_headSYN", TCP_dest_port_headSYN->valuestring},
      {"TCP_dest_port_tailSYN", TCP_dest_port_tailSYN->valuestring},
      {"TCP_port_preProb", TCP_port_preProb->valuestring},
      {"TCP_port_postProb", TCP_port_postProb->valuestring},
      {"UDP_packet_size", UDP_packet_size->valuestring},
      {"inter_time", inter_time->valuestring},
      {"UDP_train_size", UDP_train_size->valuestring},
      {"UDP_TTL", UDP_TTL->valuestring}
    };
    //printf("%s: %s\n", config[2].key, config[2].value);

    char buffer[BUFFER_MAX] = ""; //create buffer, use strcat to fill/serialize json file
    for (int i=0; i < ITEMS; i++) {
        strcat(buffer,config[i].key);
        strcat(buffer,":");
        strcat(buffer,config[i].value);
        if (i == 10) //remove last comma 
            break;
        strcat(buffer, ":");
    }
    
    

    //Pre-Probing TCP Connection Phase

    //establish first tcp connection in function call
    int pre_post = 1; //pre-post indicates which port to use and whether to use TCP proto to SEND a message, or RECV one
    char *addr; //addr is server IP address
    strcpy(addr, config[0].value);
    int port[2] = {atoi(config[5].value), atoi(config[6].value)}; //store ports in int array
    char *placeholder = est_TCP(buffer, port, addr, pre_post); //placeholder is never used, is assigned a NULL value

    //Probing Phase

    send_UDP(config); //only one parameter, passing entire JSON struct array

    //Post-Probing TCP Connection Phase
        
        //establish same tcp connection with same function call, same parameters,
        //but this time server will return msg
        pre_post = 0;
        //establish post-Probe TCP connection, here, the client will receive a message from the server, stored in res
        char *res = est_TCP(NULL, port, addr, pre_post);
        //compression converts char to int using ASCII manipulation
        int compression = res[0] - 48; //based on msg, if (res == 0, printf("No compression detected")), if res == 1, then yes compression
        if (compression) 
            printf("FINAL: Network compression detected!");
        else    
            printf("FINAL: Network compression NOT detected!");
        free(res);

    //DONE with pt 1!
    cJSON_Delete(json);
    return 0;
}

char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post) {
    //create socket, basic error handling
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    printf("ERROR opening socket\n");
        exit(0);
    }
    
    //fill struct with server info
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    //if pre_post, then pre-Probe, if !pre_post, then post-Probe phase
    if (pre_post)
        server_addr.sin_port = htons(PORT[0]);
    else
        server_addr.sin_port = htons(PORT[1]);
    server_addr.sin_addr.s_addr = inet_addr(ADDR);

    //error handling, try to connect with the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printf("ERROR connecting with the server using socket!\n");
        exit(0);
    }
    else
        printf("Connected to server!\n");

    //sending/receiving messages here
    //if pre_post is 1, then in pre_probing phase, client sends data (json), if pre_post is 0, then in post_probe, server returns data
    if (pre_post) {
        int count1 = send(sockfd, BUFFER, BUFFER_MAX, 0);
        if (count1 == -1) {
            printf("error sending message to server\n");
            exit(0);
        }
        else 
            printf("Sent message to server!\n");
    }
    else {
        char *msg = (char *) malloc(sizeof(char) * BUFFER_MAX);
        int count2 = recv(sockfd, msg, BUFFER_MAX, 0);
        if (count2 == -1) {
            printf("error receiving message from server\n");
            exit(0);
        }
        else    
            printf("Received message from server!");
        return msg;
    }

    close(sockfd);
    //base case
    return NULL;
}

void send_UDP (jsonLine *items) { 
    //create socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Error making UDP socket\n");
        exit(EXIT_FAILURE);
    }

    //set DF bit
    int dfval = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_DONTFRAG, &dfval, sizeof(dfval)) < 0) {
        printf("error with setting don't fragment bit\n");
        exit(EXIT_FAILURE);
    }

    //fill server info
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(items[2].value));
    sin.sin_addr.s_addr = inet_addr(items[0].value);

    printf("Set server info (struct sockaddr_in stuff lmao)!\n");

    //initialize necessary variables
    int packet_size = atoi(items[7].value);
    int train_size = atoi(items[9].value);
    int inter_time = atoi(items[8].value);
    //fill buffer with 1000 0s
    char low_entropy_BUFFER[packet_size];
    memset(low_entropy_BUFFER, 0, packet_size);
    //first time, set timer with inter_time
            //while timer isn't == inter_time (or packet count != 6000), run while loop
            //to make and send UDP packets with all 0s buffer 
    //basic timer
        float sec = 0;
        int pak_count = 0, true_count = 0;
        clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            //send UDP packet (6000 times haha)
            //setting packet ID
            low_entropy_BUFFER[0] = pak_count & 0xFF;
            low_entropy_BUFFER[1] = pak_count & 0xFF;
            if (sendto(sockfd, low_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send\n");
            pak_count++;
        } while ((sec <= inter_time) && (pak_count <= train_size));
        printf("Low entropy payload sent!");
    
    //second time, restart before timer and new difference timer
        //make random packet_data using random_file in ../dir
        char high_entropy_BUFFER[packet_size];
        FILE *fp;
        if ((fp = fopen("../random_file", "rb")) == NULL) {
            printf("error opening file");
        }
        fread(high_entropy_BUFFER, sizeof(char), packet_size, fp);
        fclose(fp);
        

        sec = 0, pak_count = 0, true_count = 0;
        before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            //setting packet ID
            high_entropy_BUFFER[0] = pak_count & 0xFF;
            high_entropy_BUFFER[1] = pak_count & 0xFF;
            //send UDP packet (6000 times again)
            if (sendto(sockfd, high_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send\n");
            pak_count++;
        } while ((sec <= inter_time) && (pak_count <= train_size));
        printf("High entropy payload sent!");
    close(sockfd);
}

cJSON *JSONObj(char *input[]) {
    // open the file 
    FILE *fp = fopen(input[1], "r"); 
    if (fp == NULL) { 
        printf("Error: Unable to open the file.\n"); 
        exit(EXIT_FAILURE); 
    } 
  
    // read the file contents into a string 
    char buffer[1024]; 
    int len = fread(buffer, 1, sizeof(buffer), fp); 
    fclose(fp);

    // parse the JSON data 
    cJSON *json = cJSON_Parse(buffer); 
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        exit(EXIT_FAILURE);
    } 

    return json;
}