main: main.cpp

test: main
	./test.sh

clean:
	rm -f main *.o *~ tmp* *.out
