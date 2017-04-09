#include "public.h"

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in sockaddr;
	struct sockaddr_in sockcli;
	int len = sizeof(sockcli);
	char rcvbuf[MAX_LINE];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		printf("socket error!\r\n");
		exit(1);
	}
	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(SEVER_PORT);
	int ret = bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if(ret < 0)
	{
		printf("bind error!\r\n");
		exit(1);
	}
	for(;;)
	{
		memset(rcvbuf, 0x0, sizeof(rcvbuf));
		int nread = recvfrom(sockfd, rcvbuf, sizeof(rcvbuf), 0, (struct sockaddr*)&sockcli, &len);
		char cllip[64];
		memset(cllip, 0x0, sizeof(cllip));
		inet_ntop(AF_INET, &sockcli.sin_addr, cllip, sizeof(cllip));
		int port = ntohs(sockcli.sin_port);
		printf("recvfrom IP:%s Port:%d message:%s", cllip, port, rcvbuf);
		sendto(sockfd, rcvbuf, nread, 0, (struct sockaddr*)&sockcli, len);
	}
}
