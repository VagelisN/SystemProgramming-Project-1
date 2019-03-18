OBJECTS = werhauz.o heap.o hash.o cdr.o cdr_list.o dest_list.o

werhauz: $(OBJECTS)
	gcc -g3 $(OBJECTS) -o werhauz

werhauz.o: werhauz.c
	gcc -c werhauz.c

heap.o: heap.c heap.h
	gcc -c heap.c

hash.o: hash.c hash.h
	gcc -c hash.c

cdr.o: cdr.c cdr.h
	gcc -c cdr.c

dest_list.o: dest_list.c dest_list.h
	gcc -c dest_list.c

cdr_list.o: cdr_list.c cdr_list.h
	gcc -c cdr_list.c

clean:
	rm *.o werhauz

