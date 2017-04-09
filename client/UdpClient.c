#include "public.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: udpcli <IPaddress>\r\n");
		exit(1);
	}
	char sndbuf[MAX_LINE], rcvbuf[MAX_LINE+1];
	memset(sndbuf, 0x0, sizeof(sndbuf));
	memset(rcvbuf, 0x0, sizeof(rcvbuf));
	int sockfd;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{	
		printf("socket error!\r\n");
		exit(1);
	}
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(SEVER_PORT);
	while(fgets(sndbuf, MAX_LINE, stdin) != NULL)
	{
		int ret = sendto(sockfd, sndbuf, strlen(sndbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		memset(rcvbuf, 0x0, sizeof(rcvbuf));
		ret = recvfrom(sockfd, rcvbuf, MAX_LINE, 0, NULL, NULL);
		fputs(rcvbuf, stdout);
		memset(sndbuf, 0x0, sizeof(sndbuf));
	}
}
