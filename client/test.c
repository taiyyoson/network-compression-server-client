#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#define SIZE 4

void running(char *input[]);
int main(int argc, char *argv[]) {

    running(argv);

    return 0;
}
void running (char *input[]) {
    // Open the JSON file
    FILE *fp = fopen(input[1], "r");
    if (!fp) {
        fprintf(stderr, "Failed to open file\n");
        return;
    }

    // Read the contents of the JSON file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json_buffer = malloc(file_size);
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    // Parse the JSON data
    json_error_t error;
    json_t *root = json_loads(json_buffer, 0, &error);
    free(json_buffer);
    if (!root) {
        fprintf(stderr, "Failed to parse JSON: %s\n", error.text);
        return;
    }

    // Extract data from JSON
    const char *name = json_string_value(json_object_get(root, "server_ip_addr"));
    const char *age = json_string_value(json_object_get(root, "UDP_src_port"));
    const char *address = json_string_value(json_object_get(root, "UDP_TTL"));
    
    // Print the extracted data
    printf("Name: %s\n", name);
    printf("Age: %s\n", age);
    printf("Address: %s\n", address);

    // Cleanup
    json_decref(root);
}
