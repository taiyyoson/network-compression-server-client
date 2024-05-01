#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <jansson.h>
#define DEFAULT_LENGTH 22

typedef struct {
    char key[DEFAULT_LENGTH];
    char value[DEFAULT_LENGTH];
} jsonLine;

void parseconfig (jsonLine **arr, char *input[]);
void est_TCP();

int main(int argc, char *argv[]) {
    
    //parse given json file into struct
    /*if (argc < 2) {
        printf("missing JSON file in cmd line arg!");
        return EXIT_FAILURE;
    }*/
    jsonLine *config = (jsonLine*) malloc(11 * sizeof(DEFAULT_LENGTH)*2);

    parseconfig(&config, argv);
    

    //Pre-Probing TCP Connection Phase

        //establish first tcp connection in function call
            //establish_TCP();
            //sends array of json structs to server (this is the msg)
            //close connection


    //Probing Phase

        //create new socket for UDP packets, connects with server-side socket
        //after socket is created, same function call twice (for loop for easy understanding)
            //include a count, count = 0 is for low entropy, count = 1 is for high entropy
            
            //first time, set timer with inter_time
                //call function with socket
                int count = 0;
                int sockfd;
                send_UDP(sockfd, count, config);
                //while timer isn't == 0 (or packet count != 6000), run while loop
                //to make and send UDP packets with all 0s buffer 
            //second time, same thing, but:
                //use urandom file to generate and send random packets, 6000
                count++;
                send_UDP(sockfd, count, config);

        
        //close socket



    //Post-Probing TCP Connection Phase
        
        //establish same tcp connection with same function call, same parameters,
        //but this time server will return msg
        //close connection

        //based on msg, if (res == 0, printf("No compression detected")), if res == 1, then yes compression

    //DONE with pt 1!
    free(config);
    return 0;
}

void est_TCP() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    char buffer[UDP_packet_size] = "Hello World";

    if (sockfd == -1) {
	    printf("ERROR opening socket");
        return;
    }



}

void send_UDP (int sock, int count, jsonLine *config) { 
    //create buffer
    char BUFFER[1000];
    //if not count, low entropy payload
    if (!count) {
        //first time, set timer with inter_time
                //call function with socket
                //while timer isn't == 0 (or packet count != 6000), run while loop
                //to make and send UDP packets with all 0s buffer 
        //basic timer
        int sec = 0, pak_count = 0;
        clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            //send UDP packets
            
            pak_count++;
        } while (sec <= atoi(config[8]) && pak_count <= atoi(config[9])); //config[8] is variable that holds inter_time

    }
    //if count, high entropy payload
    if (count) {

    }
}

void parseconfig (jsonLine **arr, char *input[]) {
    int json_test = 1;
    //this is to test that .json parses correctly
    FILE *fp = fopen("../config.json", "r"); 
    if (!fp) {
        printf("No JSON file was given.");
        return;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek (fp, 0, SEEK_SET);

    char *json_buffer = (char*) malloc(file_size);
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    json_error_t error;
    json_t *root = json_loads(json_buffer, 0, &error);
    free(json_buffer);
    if (!root) {
        printf("Failed to parse JSON\n");
        return;
    }
    //storing values in struct array
    strcpy((**arr).key,"server_ip_addr");
    strcpy((**arr).value,json_string_value(json_object_get(root, "server_ip_addr")));
    (*arr)++;
    strcpy((**arr).key,"UDP_src_port");
    strcpy((**arr).value,json_string_value(json_object_get(root, "UDP_src_port")));
    (*arr)++;
    strcpy((**arr).key,"UDP_dest_port");
    strcpy((**arr).value,json_string_value(json_object_get(root, "UDP_dest_port")));
    (*arr)++;
    strcpy((**arr).key,"TCP_dest_port_headSYN");
    strcpy((**arr).value,json_string_value(json_object_get(root, "TCP_dest_port_headSYN")));
    (*arr)++;
    strcpy((**arr).key,"TCP_dest_port_tailSYN");
    strcpy((**arr).value,json_string_value(json_object_get(root, "TCP_dest_port_tailSYN")));
    (*arr)++;
    strcpy((**arr).key,"TCP_port_preProb");
    strcpy((**arr).value,json_string_value(json_object_get(root, "TCP_port_preProb")));
    (*arr)++;
    strcpy((**arr).key,"TCP_port_postProb");
    strcpy((**arr).value,json_string_value(json_object_get(root, "TCP_port_postProb")));
    (*arr)++;
    strcpy((**arr).key,"UDP_packet_size");
    strcpy((**arr).value,json_string_value(json_object_get(root, "UDP_packet_size")));
    (*arr)++;
    strcpy((**arr).key,"inter_time");
    strcpy((**arr).value,json_string_value(json_object_get(root, "inter_time")));
    (*arr)++;
    strcpy((**arr).key,"UDP_train_size");
    strcpy((**arr).value,json_string_value(json_object_get(root, "UDP_train_size")));
    (*arr)++;
    strcpy((**arr).key,"UDP_TTL");
    strcpy((**arr).value,json_string_value(json_object_get(root, "UDP_TTL")));
    json_decref(root);

    //purely testing purposes
    if (json_test) 
        printf("Sample JSON variable: %s", (**arr).value);
}