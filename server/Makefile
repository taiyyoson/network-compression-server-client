PROG = compdetect_server
OBJS = server

%.o : %.c 
	gcc -c -g -o $@ $<
$(PROG):$(OBJS).o
	gcc -g -o $@ $^
clean:
	rm -rf $(OBJS).o $(PROG)