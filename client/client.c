#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <jansson.h>
#define DEFAULT_LENGTH 22
#define ADDR_LEN 14
#define ITEMS 11
#define BUFFER_MAX 1024

typedef struct {
    char *key;
    const char *value;
} jsonLine;

json_t *json_init(char *input[]);
char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post);
void send_UDP (jsonLine *items);

int main(int argc, char *argv[]) {
   //parse given json file into struct
    if (argc < 2) {
        printf("missing JSON file in cmd line arg!");
        return EXIT_FAILURE;
    }
    json_t *root = json_init(argv);
    jsonLine config[ITEMS] = {
      {"server_ip_addr", json_string_value(json_object_get(root, "server_ip_addr"))},
      {"UDP_src_port", json_string_value(json_object_get(root, "UDP_src_port"))},
      {"UDP_dest_port", json_string_value(json_object_get(root, "UDP_dest_port"))},
      {"TCP_dest_port_headSYN", json_string_value(json_object_get(root, "TCP_dest_port_headSYN"))},
      {"TCP_dest_port_tailSYN", json_string_value(json_object_get(root, "TCP_dest_port_tailSYN"))},
      {"TCP_port_preProb", json_string_value(json_object_get(root, "TCP_port_preProb"))},
      {"TCP_port_postProb", json_string_value(json_object_get(root, "TCP_port_postProb"))},
      {"UDP_packet_size", json_string_value(json_object_get(root, "UDP_packet_size"))},
      {"inter_time", json_string_value(json_object_get(root, "inter_time"))},
      {"UDP_train_size", json_string_value(json_object_get(root, "UDP_train_size"))},
      {"UDP_TTL", json_string_value(json_object_get(root, "UDP_TTL"))}
    };
    printf("%s: %s\n", config[0].value, config[0].key);
    
    

    //Pre-Probing TCP Connection Phase

    //establish first tcp connection in function call
        //establish_TCP();
            //sends array of json structs to server (this is the msg)
            //close connection
    int pre_post = 1;
    char buffer[BUFFER_MAX];
    for (int i=0; i < ITEMS; i++) {
        strcat(buffer,config[i].key);
        strcat(buffer,":");
        strcat(buffer,config[i].value);
        strcat(buffer, ",");
    }
    //printf("%s",buffer);
    char *addr;
    strcpy(addr, config[0].value);
    //printf("%s", addr);
    int port[2] = {atoi(config[5].value), atoi(config[6].value)};
    char *placeholder = est_TCP(buffer, port, addr, pre_post);

    //Probing Phase

        //create new socket for UDP packets, connects with server-side socket
            //first time, set timer with inter_time
                //call function with socket
    send_UDP(config);
                //while timer isn't == 0 (or packet count != 6000), run while loop
                //to make and send UDP packets with all 0s buffer 
            //second time, same thing, but:
                //use urandom file to generate and send random packets, 6000


        
        //close socket



    //Post-Probing TCP Connection Phase
        
        //establish same tcp connection with same function call, same parameters,
        //but this time server will return msg
        //based on msg, if (res == 0, printf("No compression detected")), if res == 1, then yes compression
        pre_post = 0;
        char *res = est_TCP(NULL, port, addr, pre_post);
        int compression = res[0] - 48;
        if (compression) 
            printf("FINAL: Network compression detected!");
        else    
            printf("FINAL: Network compression NOT detected!");

    //DONE with pt 1!
    json_decref(root);
    return 0;
}

char *est_TCP(const char *BUFFER, int *PORT, char *ADDR, int pre_post) {
    int sockfd, connfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    printf("ERROR opening socket");
        exit(0);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (pre_post)
        server_addr.sin_port = htons(PORT[0]);
    else
        server_addr.sin_port = htons(PORT[1]);
    server_addr.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printf("ERROR connecting with the server using socket!");
        exit(0);
    }

    //sending/receiving messages here
    //if pre_post is 1, then in pre_probing phase, client sends data (json), if pre_post is 0, then in post_probe, server returns data
    if (pre_post) {
        int count1 = send(sockfd, BUFFER, BUFFER_MAX, 0);
        if (count1 == -1) {
            printf("error sending message to server");
            exit(0);
        }
    }
    else {
        char msg[BUFFER_MAX];
        int count2 = recv(sockfd, msg, BUFFER_MAX, 0);
        if (count2 == -1) {
            printf("error receiving message from server");
            exit(0);
        }
        return msg;
    }

    close(sockfd);
    return NULL;
}

void send_UDP (jsonLine *items) { 
    //create socket
    int sockfd;
    if (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == -1) {
        printf("Error making UDP socket");
        return;
    }

    //set DF bit
    int dfval = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_DONTFRAG, &dfval, sizeof(dfval)) < 0) {
        printf("error with setting don't fragment bit");
    }
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(items[2].value));
    sin.sin_addr.s_addr = inet_addr(items[0].value);


    //create buffer
    int packet_size = atoi(items[7].value);
    int train_size = atoi(items[9].value);
    int inter_time = atoi(items[8].value);
    char low_entropy_BUFFER[packet_size];
    memset(low_entropy_BUFFER, 0, packet_size);
    //first time, set timer with inter_time
            //while timer isn't == 0 (or packet count != 6000), run while loop
            //to make and send UDP packets with all 0s buffer 
    //basic timer
        float sec = 0;
        int pak_count = 0, true_count = 0;
        clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            //send UDP packet (6000 times haha)
            low_entropy_BUFFER[0] = pak_count & 0xFF;
            low_entropy_BUFFER[1] = pak_count & 0xFF;
            if (sendto(sockfd, low_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send");
            else 
                true_count++;
            pak_count++;
        } while ((sec <= inter_time) && (pak_count <= train_size)); //items[8] is variable that holds inter_time
        printf("true_count: %d and pak_count: %d. Client/server lost %d packets from the low entropy payload", true_count, pak_count, pak_count - true_count);
    
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
        clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            high_entropy_BUFFER[0] = pak_count & 0xFF;
            high_entropy_BUFFER[1] = pak_count & 0xFF;
            //send UDP packet (6000 times again)
            if (sendto(sockfd, high_entropy_BUFFER, packet_size, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
                printf("packet failed to send");
            else 
                true_count++;
            pak_count++;
        } while ((sec <= inter_time) && (pak_count <= train_size));
        printf("true_count: %d and pak_count: %d. Client/server lost %d packets from the high entropy payload", true_count, pak_count, pak_count - true_count);
    
    close(sockfd);
}

json_t *json_init (char *input[]) {
    int json_test = 1;
    //this is to test that .json parses correctly
    FILE *fp = fopen(input[1], "r"); 
    if (!fp) {
        printf("No JSON file was given.");
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *json_buffer = malloc(file_size);
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    json_error_t error;
    json_t *root = json_loads(json_buffer, 0, &error);
    free(json_buffer);
    if (!root) {
        fprintf(stderr, "Failed to parse JSON: %s\n", error.text);
        return NULL;
    }
    
    return root;
}