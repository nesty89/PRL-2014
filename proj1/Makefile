NAME=es
FILES=es.cpp

CC=mpic++ --prefix /usr/local/share/OpenMPI

ALL:
	$(CC) -o $(NAME) $(FILES) 

clean:
	rm -f *.o $(NAME) *~ *.exe

test:
	make clean
	make
	./test.sh