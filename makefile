main: main.o scanner.o parser.o testtree.o semantics.o
	g++ main.o scanner.o parser.o testtree.o semantics.o -o comp

main.o: main.c token.h node.h
	g++ -c main.c

scanner.o: scanner.c scanner.h token.h
	g++ -c scanner.c

parser.o: parser.c parser.h token.h node.h
	g++ -c parser.c

testtree.o: testtree.c testtree.h token.h node.h
	g++ -c testtree.c

semantics.o: semantics.c semantics.h token.h node.h
	g++ -c semantics.c

clean:
	rm *.o *.asm comp
