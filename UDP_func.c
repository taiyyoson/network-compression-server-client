//for my server
int rec_UDP(int SERVER_PORT, int INTER_TIME) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
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
    int sec = 0;
    int rec_last;
    clock_t before = clock();
    while (sec <= INTER_TIME) {
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
        } while((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) > 0);
    }
    clock_t after = clock() - before;
    float low_entropy = after;

    //HIGH ENTROPY PAYLOAD
    sec = 0;
    rec_first = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len);
    before = clock();
    while (sec <= INTER_TIME) {
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
        } while((rec_last = recvfrom(sockfd, buffer, BUFFER_MAX, 0, (struct sockaddr *)&client_addr, &client_len)) > 0);
    }
    clock_t after2 = clock() - before;
    float high_entropy = after2;

    if ((high_entropy - low_entropy) >= 0.1) {
        return 1;
    }
    else {
        return 0;
    }

}


//for my client
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
        int sec = 0, pak_count = 0, true_count = 0;
        clock_t before = clock();
        do {
            clock_t difference = clock() - before;
            sec = difference / CLOCKS_PER_SEC;
            //send UDP packet (6000 times haha)
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