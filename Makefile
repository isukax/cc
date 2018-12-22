main:
	g++ -Wall -g -O0 main.cpp -o main

test: main
	./test.sh

clean:
	rm -f main *.o *~ tmp* *.out
