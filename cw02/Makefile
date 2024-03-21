BIN = *.o *.a *.so client_*

all:
	make client_static
	make client_shared
	make client_dynamic


#Clients
client_static: client.c collatz_static
	gcc -Wall client.c libcollatz.a -o client_static

client_shared: client.c collatz_shared
	gcc -Wall client.c -lcollatz -L ./ -Wl,-rpath,. -o client_shared

client_dynamic: client.c collatz_shared
	gcc -Wall client.c -ldl -o client_dynamic -D DYNAMIC


#Libraries
collatz_static: collatz.c
	gcc -c collatz.c
	ar rcs libcollatz.a collatz.o

collatz_shared: collatz.c
	gcc -Wall -fPIC -c collatz.c
	gcc -shared collatz.o -o libcollatz.so


#Tests
test_static:
	./client_static

test_shared:
	./client_shared

test_dynamic:
	./client_dynamic



clean:
	rm $(BIN)