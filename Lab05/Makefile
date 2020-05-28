CC:=g++
OBJS = sockop
TARGETS=process thread client

all: $(TARGETS)	

$(OBJS).o: $(OBJS).c
	$(CC) -c -o $@ $<

process: $(OBJS).o echo-process.c
	$(CC) -o process $^

thread: echo-thread.c $(OBJS).o
	$(CC) -pthread -o thread $^

client: echoc.c $(OBJS).o
	$(CC) -o client $^

clean:
	rm *.o
	rm $(TARGETS)
