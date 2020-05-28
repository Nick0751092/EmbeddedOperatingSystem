CC = g++
TARGETS = hw3test hw3checker
CFLAGS = -std=c++11

all: $(TARGETS)
	mkdir -p result
	mkdir -p p2_result

hw3test : hw3test.o

hw3checker : hw3checker.o

.cpp.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

clean:
	rm -f *.o $(TARGETS)
	@rm -fr result
	@rm -fr p2_result

check_all: p1_check p2_check

p1_check:
	diff -qr result/ result_ans/
p2_check:
	./hw3checker
