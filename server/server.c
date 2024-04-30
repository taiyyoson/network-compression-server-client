#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main (void) {

    //Pre-Probing TCP Connection Phase

    //server listens for incoming connection from client
    //receive json file, store in variable




    //Probing Phase

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

    return EXIT_SUCCESS;
}