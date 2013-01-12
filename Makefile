all: warehouse store1 store2 store3 store4
warehouse: warehouse.o
	gcc -o warehouse -g warehouse.o -lsocket -lnsl -lresolv	

warehouse.o: warehouse.c warehouse.h
	gcc -g -c -Wall warehouse.c

store1: store1.o
	gcc -o store1 -g store1.o -lsocket -lnsl -lresolv
	
store1.o: store1.c store1.h
	gcc -g -c -Wall store1.c
	
store2: store2.o
	gcc -o store2 -g store2.o -lsocket -lnsl -lresolv
	
store2.o: store2.c store2.h
	gcc -g -c -Wall store2.c
	
store3: store3.o
	gcc -o store3 -g store3.o -lsocket -lnsl -lresolv
	
store3.o: store3.c store3.h
	gcc -g -c -Wall store3.c
	
store4: store4.o
	gcc -o store4 -g store4.o -lsocket -lnsl -lresolv
	
store4.o: store4.c store4.h
	gcc -g -c -Wall store4.c
	
clean:
	rm -f *.o warehouse store1 store2 store3 store4
