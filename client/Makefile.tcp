INC = -I ../public

TcpClient : TcpClient.o
	cc -o TcpClient TcpClient.o
TcpClient.o : TcpClient.c
	cc -c $(INC) TcpClient.c

clean:
	rm -rf TcpClient TcpLient.o
