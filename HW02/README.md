make all:

	to compile all you need

make check_all:

	to see result is right or wrong

make p1_check:

	only check p1

make p2_check:

	only check p2

make clean:

	rm hw2test.o hw2test result(dir) p2_result(dir)

** please use console.txt and concert.txt to setup your server **

** and make sure you restart(reset the remaining seat) your server when you do check **

run : ./{hw2test} {server_ip} {server_port}
