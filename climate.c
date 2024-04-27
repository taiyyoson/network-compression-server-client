/* climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F 
 * Max Temperatuer on: Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F
 * Min Temperature on: Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F
 * Max Temperature on: Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F 
 * Min Temperature on: Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 *
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»   0.0»100.0»  0.0»95644.0»277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»0.0»100.0»  0.0»99226.0»282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»   0.0»0.0»0.0»102112.0»   285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»   0.0»100.0»  0.0»101765.0» 285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»   0.0»22.0»   0.0»102074.0» 285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»   0.0»0.0»0.0»101679.0»   283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»   0.0»100.0»  0.0»102343.0» 285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»   0.0»100.0»  0.0»100645.0» 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info {
    char code[3];
    unsigned long num_records;
    long double sum_temp;

    long double sum_humidity;
    long double sum_cloud;
    float max_temp;
    float min_temp;
    time_t min_temp_time;
    time_t max_temp_time;
    unsigned long lightning_count;
    unsigned long snow_count;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[]) {

    /* TODO: fix this conditional. You should be able to read multiple files. */
    if (argc < 2) {
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = { NULL };

    int i;
    for (i = 1; i < argc; ++i) {
        /* TODO: Open the file for reading */
        FILE *fp = fopen(argv[i],"r");
        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */
        if (fp == NULL) {
                printf("File doesn't exist, ERROR\n");
                continue;
        }
        printf("Opening file: %s\n", argv[i]);
        /* TODO: Analyze the file */
        /* analyze_file(file, states, NUM_STATES); */
        analyze_file (fp, states, NUM_STATES);
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}

void analyze_file(FILE *file, struct climate_info **states, int num_states) {
    const int line_sz = 100;
    char line[line_sz];
    char delim[] = "\t";
    while (fgets(line, line_sz, file) != NULL) {

        /* TODO: We need to do a few things here:
         *
         *       * Tokenize the line.
         *       * Determine what state the line is for. This will be the state
         *         code, stored as our first token.
         *       * If our states array doesn't have a climate_info entry for
         *         this state, then we need to allocate memory for it and put it
         *         in the next open place in the array. Otherwise, we reuse the
         *         existing entry.
         *       * Update the climate_info structure as necessary.
         */

        //initialize first token
        char *token = strtok(line, delim);
        //loop through all states
        for (int i=0; i < num_states;i++) {
            //if couldn't find matching state code in states[i]->code, dynamically allocate memory/create new space for that state
            if (*(states + i) == NULL) {
                *(states + i) = (struct climate_info*) malloc(sizeof(struct climate_info));
                //initialize all members
                strcpy((*(states+i))->code, token);

                (*(states+i))->num_records = 0;
                (*(states+i))->sum_temp = 0;
                (*(states+i))->sum_humidity = 0;
                (*(states+i))->sum_cloud = 0;
                (*(states+i))->max_temp = 0;
                (*(states+i))->min_temp = 0;
                (*(states+i))->lightning_count = 0;
                (*(states+i))->snow_count = 0;
                (*(states+i))->min_temp_time = 0;
                (*(states+i))->max_temp_time = 0;
                //subtract so that this newly created state can go through else if statement below
                i--;
            }
            else if (strcmp(token, (*(states + i))->code) == 0) {
                //increment record count
                (*(states+i))->num_records += 1;

                int line_count = 0;
                //used to store time in case Temperature is max temperature
                time_t tempTime;
                //used as placeholders to store values;
                double holder = 0;
                int holder2 = 0; 
                float deg_f;
                //loop through tokens in line 
                while (token != NULL) {
                    //line_count == 0 is state code

                    //UNIX timestamp
                    if (line_count == 1) {
                        //temporarily store time (in case temperature associated with tempTime is a maximum or minimum temp)
                        sscanf(token, "%llu", &tempTime);
                        tempTime /= 1000;
                    }
                    //line_count == 2 is geolocation hash

                    //humidity 
                    if (line_count == 3) {
                        sscanf(token, "%lf", &holder);
                        //increment sums to later divide to find average
                        (*(states+i))->sum_humidity += holder;
                    }
                    //snow present
                    if (line_count == 4) {
                        sscanf(token, "%d", &holder2);
                        if (holder2 == 1) {
                            //increment if snow is there
                            (*(states+i))->snow_count += 1;
                        }
                    }
                    //cloud cover
                    if (line_count == 5) {
                        sscanf(token, "%lf", &holder);
                        //increment sum
                        (*(states+i))->sum_cloud += holder;
                    }
                    //lightning
                    if (line_count == 6) {
                        sscanf(token, "%d", &holder2);
                        if (holder2 == 1) {
                            //increment count
                            (*(states+i))->lightning_count += 1;
                        }
                    }
                    //line_count == 7 is pressure(Pa)

                    //surface temperature
                    if (line_count == 8) {
                        sscanf(token, "%lf", &holder);
                        //convert from kelvin to farenheit
                        deg_f = holder * 1.8 - 459.67;
                        //increment sum
                        (*(states+i))->sum_temp += deg_f;
                        //if current temp is > max_temp, change to new and set max_temp_time
                        if (deg_f > ((*(states+i))->max_temp)) {
                            (*(states+i))->max_temp = deg_f;
                            (*(states+i))->max_temp_time = tempTime;
                        }
                        //min_temp
                        if (deg_f < ((*(states+i))->min_temp)) {
                            (*(states+i))->min_temp = deg_f;
                            (*(states+i))->min_temp_time = tempTime;
                        }
                    } 


                    //increment, move to next token
                    line_count++;
                    token = strtok(NULL, delim);
                }
                //break for loop for efficiency after filling this state
                break;
            }
            
            
        }
        
    }
}

void print_report(struct climate_info *states[], int num_states) {
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i) {
        if (states[i] != NULL) {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");

    /* TODO: Print out the summary for each state. See format above. */
    //for averages
    float avg_humidity = 0;
    float avg_temp = 0; 
    float avg_cloud = 0;
    time_t maxtimestamp;
    time_t mintimestamp;
    //loop through all states, print
    for (i = 0; i < num_states; i++) {
        //for null states
        if (states[i] == NULL) {
            continue;
        }
        //finding averages
        avg_humidity = (states[i]->sum_humidity) / (states[i]->num_records);
        avg_temp = (states[i]->sum_temp) / (states[i]->num_records);
        avg_cloud = (states[i]->sum_cloud) / (states[i]->num_records);
        maxtimestamp = states[i]->max_temp_time;
        mintimestamp = states[i]->min_temp_time;

        //printf
        printf("-- State: %s --\n", states[i]->code);
        printf("Number of Records: %lu\n", states[i]->num_records);
        printf("Average Humidity: %.1f%\n", avg_humidity);
        printf("Average Temperature: %.1fF\n", avg_temp);
        printf("Max Temperature: %.1fF\n", states[i]->max_temp);
        printf("Max Temperature on: %s", ctime(&maxtimestamp));
        printf("Min Temperature: %.1fF\n", states[i]->min_temp);
        printf("Min Temperature on: %s", ctime(&mintimestamp));
        printf("Lightning Strikes: %lu\n", states[i]->lightning_count);
        printf("Records with Snow Cover: %lu\n", states[i]->snow_count);
        printf("Average Cloud Cover: %.1f%\n", avg_cloud);
    }
}











