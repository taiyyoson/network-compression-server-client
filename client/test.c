#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../cJSON.h"
typedef struct {
    char *key;
    char *value;
} jsonLine;

cJSON *JSONObj(char *input[]);
int main(int argc, char *argv[]) { 
    cJSON *json = JSONObj(argv);
  
    // access the JSON data 
    cJSON *server_ip_addr = cJSON_GetObjectItemCaseSensitive(json, "server_ip_addr"); 
    printf("%s\n", server_ip_addr->valuestring);
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

    jsonLine config[11] = {
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
    printf("%s: %s\n", config[2].key, config[2].value);

    char buffer[1000] = ""; //create buffer, use strcat to fill/serialize json file
    for (int i=0; i < 11; i++) {
        strcat(buffer,config[i].key);
        strcat(buffer,":");
        strcat(buffer,config[i].value);
        if (i == 10)
            break;
        strcat(buffer, ",");
    }
    
    // delete the JSON object 
    cJSON_Delete(json); 
    return 0; 
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