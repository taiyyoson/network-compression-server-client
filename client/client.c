//Taiyo Williamson, 20688536
//client-side of client-server application to detect network compression

//header files
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>

#include "../cJSON.h"

//global constants 
#define DEFAULT_LENGTH 22
#define ADDR_LEN 14
#define ITEMS 12
#define BUFFER_MAX 1024


//struct to hold json line items
typedef struct 
{
    char *key;
    char *value;
} jsonLine;

//functions made, proper descriptions given at actual function bodies
cJSON *JSONObj(char *input[]); //creates cJSON root variable that is used to make json calls
char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post, int server_wait_time); //handles everything to establish TCP connection and send JSON file in form of char buffer
void send_UDP (jsonLine *items); //handles everything to send the two UDP payload trains, one low entropy, one high entropy

/***
 * main is where all the code is ran, and actually calls the functions to send TCP and UDP packets
 * argc is the number of arguments given on the cmd line
 * argv is an array of strings allocated for each argument on the cmd line
*/
int main(int argc, char *argv[]) {
   //first parse given json file into struct
    if (argc < 2) { //if there's no config.json file given
        printf("missing JSON file in cmd line arg!\n");
        return EXIT_FAILURE;
    }
    //root to make JSON calls
    cJSON *json = JSONObj(argv);
  
    // access the JSON data 
    cJSON *server_ip_addr = cJSON_GetObjectItemCaseSensitive(json, "server_ip_addr"); 
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
    cJSON *server_wait_time = cJSON_GetObjectItemCaseSensitive(json, "server_wait_time");

    //array of structs, representing config.json
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
      {"UDP_TTL", UDP_TTL->valuestring},
      {"server_wait_time", server_wait_time->valuestring}
    };
    //simple testing printf statement
    //printf("%s: %s\n", config[2].key, config[2].value);

    //buffer is used when establishing pre-probe TCP connection
    char buffer[BUFFER_MAX] = ""; //create buffer, use strcat to fill/serialize json file
    for (int i=0; i < ITEMS; i++) 
    {
        strcat(buffer,config[i].key); 
        strcat(buffer,":"); //server-side will use strtok to parse this string
        strcat(buffer,config[i].value);
        if (i == 11) //remove last comma 
            break;
        strcat(buffer, ":");
    }
    
    

    int pre_post = 1; //pre-post indicates which port to use and whether to use TCP proto to SEND a message, or RECV one
    char *addr = config[0].value; //addr points to IP address
    int port[2] = {atoi(config[5].value), atoi(config[6].value)}; //store ports in int array


    //Pre-Probing TCP Connection Phase
    char *placeholder = est_TCP(buffer, port, addr, pre_post, 0); //placeholder is never used, is assigned a NULL value, 0 is placeholder




    //Probing Phase
    send_UDP(config); //only one parameter, passing entire JSON struct array



    pre_post = 0;
    //Post-Probing TCP Connection Phase (this time client receives message, receives only necessary input to detect network compression)
    char *res = est_TCP(NULL, port, addr, pre_post, atoi(config[11].value)); //atoi value is server_wait_time
       
        //compression converts char to int using ASCII manipulation
        int compression = res[0] - 48; //based on msg, if (res == 0, printf("No compression detected")), if res == 1, then yes compression
        if (compression) 
            printf("FINAL: Network compression detected!\n");
        else    
            printf("FINAL: Network compression NOT detected!\n");
    

    //DONE with pt 1!
    free(res); //freeing, dereferencing memory locations
    cJSON_Delete(json);
    return 0;
}

/***
 * est_TCP makes a socket, fills the necessary info to send a message to a server, and either sends or receives a buffer through TCP 
 *  connection
 * BUFFER is the buffer to be sent to the server.if it's in post-probe phase, then BUFFER is NULL
 * PORT is the dest UDP port, located in the config file json struct array
 * ADDR is the server IP address
 * pre_post indicates whether we're in pre-probe or post-probe. if pre_post == 0, then pre-probe, if pre_post == 1, then post-probe
 * server_wait_time is my inter_time replacement, in a sense. server.c's recvfrom() has a timeout of only 0.5 seconds, so the client 
 *  is free to send its payload trains relatively fast, regardless of whether packets get lost or not
 * returns a char array that is the message from the server. if pre_post == 0, then returns a NULL char array, not used
*/
char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post, int server_wait_time) 
{
    //initial sleep, to basically let server and client match up, all sending and receiving will be done in similar time intervals
    if (!pre_post) 
        sleep(server_wait_time);
    //create socket, basic error handling
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
	    printf("ERROR opening socket\n");
        exit(EXIT_FAILURE);
    }
    
    //fill struct with server info
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    //if pre_post, then pre-Probe, if !pre_post, then post-Probe phase (depending on the ports that are used, 7777 vs 6666)
    if (pre_post)
        server_addr.sin_port = htons(PORT[0]);
    else
        server_addr.sin_port = htons(PORT[1]);
    if (!(inet_pton(AF_INET, ADDR, &(server_addr.sin_addr)) > 0)) 
    {
        printf("client.c 166: ERROR assigning address to socket\n");
        exit(EXIT_FAILURE);
    }

    //error handling, try to connect with the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) 
    {
        printf("client.c 172: ERROR connecting with the server using socket!\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Connected to server!\n");

    //sending/receiving messages here
    //if pre_post is 1, then in pre_probing phase, client sends data (json), if pre_post is 0, then in post_probe, server returns data
    if (pre_post) 
    {
        int count1 = send(sockfd, BUFFER, BUFFER_MAX, 0); // sending JSON data
        if (count1 == -1)
        {
            printf("error sending message to server\n");
            exit(EXIT_FAILURE);
        }
        else 
            printf("Sent message to server!\n");
    }
    else 
    {
        char *msg = (char *) malloc(sizeof(char) * BUFFER_MAX);
        int count2 = recv(sockfd, msg, BUFFER_MAX, 0);
        if (count2 == -1) 
        {
            printf("error receiving message from server\n");
            exit(0);
        }
        else    
            printf("Received message from server!\n");
        return msg;
    }

    //closing socket
    close(sockfd);
    //base case
    return NULL;
}



/***
 * send_UDP handles the whole probing phase. It creates a socket and immediately sends the low entropy payload,
 *  then sends the high entropy payload after giving the server time to catch up
 * items is a jsonLine array consisting of all infro from config.json
*/
void send_UDP (jsonLine *items) 
{ 
    //create socket, basic error handling
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        printf("Error making UDP socket\n");
        exit(EXIT_FAILURE);
    }

    //set DF bit, basic error handling
    int dfval = IP_PMTUDISC_DO; //linux df bit, on Mac OSes, DF bit is IP_DONTFRAG, but we are using Ubuntu (Linux)
    if (setsockopt(sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &dfval, sizeof(dfval)) < 0) 
    { //if linux, use IP_MTU_DISCOVER & IP_PMTUDISC_DO
        printf("error with setting don't fragment bit\n");
        exit(EXIT_FAILURE);
    }

    //fill server info
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(items[2].value)); //dst port is always the same here
    if (!(inet_pton(AF_INET, items[0].value, &(sin.sin_addr)) > 0)) 
    {
        printf("client.c 243: ERROR assigning address to socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Set server info, preparing to send payloads!\n");


    


    //initialize necessary variables
    int packet_size = atoi(items[7].value);
    int train_size = atoi(items[9].value);
    int inter_time = atoi(items[8].value) * 1000;
    int server_wait_time = atoi(items[11].value);
    //fill buffer with 1000 0s
    char low_entropy_BUFFER[packet_size];
    memset(low_entropy_BUFFER, 0, packet_size);

    //LOW ENTROPY PAYLOAD
        printf("Sending low entropy payload!\n");
        int pak_count = 0;
        do 
        {
            //send UDP packet (6000 times haha)
            //setting packet ID
            low_entropy_BUFFER[0] = pak_count & 0xFF;
            low_entropy_BUFFER[1] = pak_count & 0xFF;
            if (sendto(sockfd, low_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send\n");
            pak_count++;
        } while (pak_count <= train_size); //sending 6000 of the same packet, only difference is incrementing packet id
        printf("Low entropy payload sent!\n");


    sleep(server_wait_time);
    //HIGH ENTROPY PAYLOAD
        //make random packet_data using random_file in ../dir
        char high_entropy_BUFFER[packet_size];
        FILE *fp;
        if ((fp = fopen("../random_file", "rb")) == NULL) 
        {
            printf("error opening file\n");
        }
        fread(high_entropy_BUFFER, sizeof(char), packet_size, fp);
        fclose(fp);
        
        printf("Sending high entropy payload\n"); 
        pak_count = 0; //resetting the packet id
        do 
        {
            //setting packet ID
            high_entropy_BUFFER[0] = pak_count & 0xFF;
            high_entropy_BUFFER[1] = pak_count & 0xFF;
            //send UDP packet (6000 times again)
            if (sendto(sockfd, high_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send\n");
            pak_count++;
        } while (pak_count <= train_size);
        printf("High entropy payload sent!\n");

    //close the socket
    close(sockfd);
}


/***
 * JSONObj simply creates the JSON object needed to make calls to my config.json file. 
 * input is the command line argument argv, used to identify filename config.json
 * returns a root of type cJSON, which can be used to access config.json
*/
cJSON *JSONObj(char *input[]) 
{
    // open the file, basic error handling
    FILE *fp = fopen(input[1], "r"); 
    if (fp == NULL) 
    { 
        printf("Error: Unable to open the file.\n"); 
        exit(EXIT_FAILURE); 
    } 
  
    // read the file contents into a string 
    char buffer[1024]; 
    int len = fread(buffer, 1, sizeof(buffer), fp); 
    fclose(fp);

    // parse the JSON data 
    cJSON *json = cJSON_Parse(buffer); 
    if (json == NULL) 
    { //basic error handling
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        exit(EXIT_FAILURE);
    } 
    //return the json object with the parsed JSON data
    return json;
}