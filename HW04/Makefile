CC = g++
TARGETS = finaltest finalchecker server
CFLAGS = -std=c++11

all: $(TARGETS)
	mkdir -p result
	mkdir -p p2_result

finaltest : finaltest.o

finalchecker : finalchecker.o

.cpp.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

clean:
	rm -f *.o $(TARGETS)
	@rm -fr result
	@rm -fr p2_result

check_all: final_check
	@for number in 1 2 3 4 5 6 7 8; do \
		wc -l p2_result/$$number.txt ;\
	done
final_check:
	./finalchecker

server:
	g++ server.c -o server

