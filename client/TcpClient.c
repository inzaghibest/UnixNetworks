#include "public.h"
//#define MAX_LINE 256
void str_cli(FILE* pFile, int sockfd)
{
	char sndbuf[MAX_LINE];
	char rcvbuf[MAX_LINE];
	fd_set rset;
	int maxfd;
	int stdeof = 0;
	FD_ZERO(&rset);
	int n = 0;
	for(;;)
	{
		if(stdeof == 0)
		{
			FD_SET(fileno(pFile), &rset);
		}
		FD_SET(sockfd, &rset);
		maxfd = max(fileno(pFile), sockfd) + 1;
		int ret = select(maxfd, &rset, NULL, NULL, NULL);
		if(ret == EINTR)
		{
			continue;
		}
		if(ret < 0)
		{
			printf("select error!\r\n");
			return;	
		}
		if(FD_ISSET(sockfd, &rset))
		{
			memset(rcvbuf, 0x0, MAX_LINE);
			if((n = read(sockfd, rcvbuf, MAX_LINE)) == 0)
			{
				if(stdeof == 1)
				{
					return;
				}else
				{
					printf("server terminated!\r\n");
					return;
				}
			}
			write(fileno(stdout), rcvbuf, n);
		}
		if(FD_ISSET(fileno(pFile), &rset))
		{
			memset(sndbuf, 0x0, MAX_LINE);
			if( (n = read(fileno(pFile), sndbuf, MAX_LINE)) == 0)
			{
				stdeof = 1;
				shutdown(sockfd, SHUT_WR); /* send FIN */
				FD_CLR(fileno(pFile), &rset);
				continue;
			}
			write(sockfd, sndbuf, n);
		}
		
	}
/*	while(fgets(buf, MAX_LINE, pFIle) != NULL)
	{
		write(sockfd, buf, 1);
		// µtwo write to sigpipe
		write(sockfd, buf+1, strlen(buf) - 1);
		memset(recvbuf, 0x0, sizeof(recvbuf));
		if(read(sockfd, recvbuf, MAX_LINE) == 0)
		{
			printf("server terminated\r\n");
		}
		fputs(recvbuf, stdout);
	}*/
}
int main(int argc, char* argv[])
{
	int sockconn[1];
	int i = 0;
	struct sockaddr_in sockserv;
	if(argc < 2)
	{
		printf("usage:[1] ipaddress\r\n");
		exit(0);
	}
	for(i; i<1; i++)
	{
		sockconn[i] = socket(AF_INET, SOCK_STREAM, 0);
		if(sockconn < 0)
		{
			printf("socket error!\r\n");
			exit(1);
		}
		bzero(&sockserv, sizeof(sockserv));
		sockserv.sin_family = AF_INET;
		sockserv.sin_port = htons(SEVER_PORT);
		inet_pton(AF_INET, argv[1], &sockserv.sin_addr);
		if(connect(sockconn[i], (struct sockaddr*)&sockserv, sizeof(sockserv)) != 0)
		{
			printf("connect error!\r\n");
			exit(1);
		}
	}
	str_cli(stdin, sockconn[0]);
	exit(0);
}

