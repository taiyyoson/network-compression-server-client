PROG = compdetect_client
OBJS = client.o ../cJSON.o

%.o : %.c 
	gcc -c -g -o $@ $<
$(PROG):$(OBJS)
	gcc -g -o $@ $^ 
clean:
	rm -rf $(OBJS) $(PROG)
