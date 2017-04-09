INC=-I ../public/
Object = sockopt
sockopt:sockopt.o
	cc -g -o  sockopt sockopt.o
sockopt.o:sockopt.c
	cc -g -c $(INC) sockopt.c
clean:
	rm -rf sockopt sockopt.o

