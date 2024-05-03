PROG = compdetect_client
OBJS = client
#PROG = tester
#OBJS = test

%.o : %.c 
	gcc -c -g -o $@ $<
$(PROG):$(OBJS).o
	gcc -g -o $@ $^ -ljansson
clean:
	rm -rf $(OBJS).o $(PROG)

#CC = gcc
#FLAGS = -std=c99 -O0 -Wall -Werror -g -pedantic

#climate: climate.c
#       $(CC) $(FLAGS) climate.c -o climate

#clean:
#        rm -f climate
#       rm -rf *.dSYM

#testOutput: climate
#        ./proj3TestOutputFormat "$$(./climate data_tn.tdv)"