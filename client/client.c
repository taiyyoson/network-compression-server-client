#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <jansson.h>

char *parseconfig[] (int count, char *input[]);

int main(int argc, char *argv[]) {
    
    char *config[] = parseconfig(argc, argv);

    //just use mass pseudocode to organiuze your thoughts bro, organize it in the same way vahab organized the 3 phases




    return 0;
}

char *parseconfig[] (int count, char *input[]) {
    int json_test = 1;
    //this is to test that .json parses correctly
    FILE *fp = fopen(input[1], "r"); 
    if (!fp) {
        printf("No JSON file was given.");
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }
    const char *server_ip_addr = json_string_value(json_object_get(root, "server_ip_addr"));
    const char *UDP_src_port = json_string_value(json_object_get(root, "UDP_src_port"));
    const char *UDP_dest_port = json_string_value(json_object_get(root, "UDP_dest_port"));
    const char *TCP_dest_port_headSYN = json_string_value(json_object_get(root, "TCP_dest_port_headSYN"));
    const char *TCP_dest_port_tailSYN = json_string_value(json_object_get(root, "TCP_dest_port_tailSYN"));
    const char *TCP_port_preProb = json_string_value(json_object_get(root, "TCP_port_preProb"));
    const char *TCP_port_postProb = json_string_value(json_object_get(root, "TCP_port_postProb"));
    const char *UDP_packet_size = json_string_value(json_object_get(root, "UDP_packet_size"));
    const char *inter_time = json_string_value(json_object_get(root, "inter_time"));
    const char *UDP_train_size = json_string_value(json_object_get(root, "UDP_train_size"));
    const char *UDP_TTL = json_string_value(json_object_get(root, "UDP_TTL"));
    json_decref(root);

    //purely testing purposes
    if (json_test) 
        printf("Sample JSON variable: %s", UDP_packet_size);
}



