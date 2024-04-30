CC = gcc
CFLAGS = -std=c99 -g -ljansson

compdetect_client : client.c
	$(CC) $(CFLAGS) -o client.c compdetect_client

clean :
	rm -rf $(PROG) $(OBJS) $(RESULT)    
	rm -rf *.dSYM





#CC = gcc
#FLAGS = -std=c99 -O0 -Wall -Werror -g -pedantic

#climate: climate.c
#       $(CC) $(FLAGS) climate.c -o climate

#clean:
#        rm -f climate
#       rm -rf *.dSYM

#testOutput: climate
#        ./proj3TestOutputFormat "$$(./climate data_tn.tdv)"