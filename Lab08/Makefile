CC:=g++
OBJS = sockop
TARGETS=process client client_no
all: $(TARGETS)	

$(OBJS).o: $(OBJS).c
	$(CC) -c -o $@ $<

process: $(OBJS).o echo-process.c
	$(CC) -o process $^

client: echoc.c $(OBJS).o
	$(CC) -o client $^

client_no: echoc_no.c $(OBJS).o
	$(CC) -o client_no $^
	
clean:
	rm *.o
	rm $(TARGETS)
