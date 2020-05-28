CC = g++
TARGETS = hw2test
CFLAGS = -std=c++11

all: $(TARGETS)
	mkdir -p result
	mkdir -p p2_result

hw2test : hw2test.o

.cpp.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

clean:
	@rm -f $(TARGETS).o $(TARGETS)
	@rm -fr result
	@rm -fr p2_result

check_all: p1_check p2_check

p1_check:
	diff -qr result/ result_ans/
p2_check:
	diff -qr p2_result/ p2_result_ans/
