INC=-I ../public/

TcpServer:TcpServer.o
	cc -g -o  TcpServer TcpServer.o
TcpServer.o:TcpServer.c
	cc -g -c $(INC) TcpServer.c
clean:
	rm -rf TcpServer TcpServer.o
