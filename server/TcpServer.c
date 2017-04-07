#include "public.h"
void sigchld(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\r\n", pid);
	return;
}
void str_eho(int connfd)
{
	ssize_t n = 0;
	char buf[MAX_LINE];
	memset(buf, 0x0, sizeof(buf));
again:
	while((n = read(connfd, buf, MAX_LINE)) > 0)
		write(connfd, buf, n);
	if(n<0 && errno == EINTR)
		goto again;
	else if(n < 0)
		printf("str_eho read error!\r\n");
}
int main(int argv, char* argc[])
{	
	int listenfd, connfd;
	char buf[MAX_LINE];
//	int client[FD_SETSIZE];
	int maxfd, nready;
	int i = 0;
	int max = 0;
	struct pollfd client[MaxOpen];
	fd_set allset, reset;
	struct sockaddr_in servaddr, cliaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("socket error!\r\n");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SEVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("bind error!\r\n");
		exit(1);
	}
	if(listen(listenfd,5) != 0)
	{
		printf("listen error!\r\n");
		exit(1);
	}
	/* epoll */
	int epollfd = epoll_create(0);
	printf("epoll fd = %d\r\n", epollfd);
	/*struct epoll_event{
	__unit32_t events; // 要注册的事件,与poll基本相同,增加E.epoll新增两个事件EPOLLET和EPOLLONESHOT
	epoll_data_t data;
	};*/
	struct epoll_event lsevent;
	lsevent.events = EPOLLIN;
	lsevent.data.fd = listenfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &lsevent);
	struct epoll_event events[MaxEpoll];
	int max = 1;
	for(;;)
	{
		int ret = epoll_wait(epollfd, &events, max+1, -1);
		if(ret < 0)
		{
			printf("epoll_wait error!\r\n");
			exit(1);
		}
		printf("%d 个描述就绪\r\n", ret);
		for(i = 0; i<ret; i++)
		{
			int sockfd = events[i].data.fd;
			if(sockfd == listenfd)
			{
				int len = sizeof(cliaddr);
				char cliip[64];
				memset(cliip, 0x0, sizeof(cliip));
				int sockcli = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
				if(sockcli < 0)
				{
					printf("accept error!\r\n");
					exit(1);
				}
				inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
				int nPort = ntohs(cliaddr.sin_port);
				printf("accept from IP:%s, Port:%d", cliip, nPort);
				struct epoll_event clievent;
				clievent.events = EPOLLIN;
				clievent.data.fd = sockcli;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, sockcli, &clievent);
				max += 1;
			}else
			{
				if(events[i].events & EPOLLIN)
				{
					printf("EPOLLIN...\r\n");
					memset(buf, 0x0, sizeof(buf));
					int nret = read(sockfd, buf, sizeof(buf));
					if(nret <= 0)
					{
						close(sockfd);
						continue;
					}
					write(sockfd, buf, nret);
				}
			}
		}
		
	}
	/* epoll */
	/* poll */
	/*client[0].fd = listenfd;
	client[0].events = POLLIN;
	for(i = 1; i<MaxOpen; i++)
	{
		client[i].fd = -1;
	}
	max = 0;
	for(;;)
	{
		nready = poll(client, max+1, -1);
		if(nready<0 && errno == EINTR)
			continue;
		if(nready < 0)
		{
			printf("poll error!\r\n");
			exit(1);
		}
		if(client[0].revents & POLLIN)
		{
			int len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
			if(connfd < 0)
				printf("accept error!\r\n");
			for(i = 0; i<MaxOpen; i++)
			{
				if(client[i].fd < 0)
				{
					char cliip[64];
					memset(cliip, 0x0, sizeof(cliip));
					inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
					int nPort = ntohs(cliaddr.sin_port);
					printf("accept a client from IP:%s, Port:%d\r\n", cliip, nPort);
					client[i].fd = connfd;
					client[i].events = POLLIN;
					break;
				}
			}
			if(i > max)
				max = i;
			if(--nready <= 0)
				continue;
		}
		for(i = 1; i<=max; i++)
		{
			if(client[i].fd < 0)
				continue;
			if(client[i].revents & (POLLIN|POLLERR))
			{
				int n = 0;
				if((n = read(client[i].fd, buf, MAX_LINE)) < 0)
				{
					if(errno == ECONNRESET)
					{
						close(client[i].fd);
						client[i].fd = -1;
					}else
					{
						printf("read error!\r\n");
					}
				}else if(n == 0)
				{
					close(client[i].fd);
					client[i].fd = -1;
				}else
				{
					write(client[i].fd, buf, n);
				}
				if(--nready <= 0)
					break;
			}
		}
	}*/
	/* select */
/*	for(i; i<FD_SETSIZE; i++)
	{
		client[i] = -1;
	}
	maxfd = listenfd;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	for(;;)
	{
		reset = allset;
		nready = select(maxfd+1, &reset, NULL, NULL, NULL);
		if(nready < 0 && errno != EINTR)
		{
			printf("select error!\r\n");
			exit(1);
		}
		if(errno == EINTR)
			continue;
		if(FD_ISSET(listenfd, &reset))
		{
			int len = sizeof(cliaddr);
			bzero(&cliaddr, sizeof(cliaddr));
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
			if(connfd < 0 && errno == EINTR)
			{
				printf("accept EINTR!\r\n");
				continue;
			}
			if(connfd < 0)
			{
				printf("accept error!\r\n");
				continue;
			}
			for(i = 0; i<FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;         // add a new descriptor 
					char cliip[64];
					memset(cliip, 0x0, sizeof(cliip));
					inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
					int nPort = ntohs(cliaddr.sin_port);
					printf("a client from IP:%s Port:%d\r\n", cliip, nPort);
					break;
				}
			}					
			if(i == FD_SETSIZE)
			{
				printf("too many client!\r\n");
				exit(1);
			}
			if(max<i)
			{
				max = i;
			}
			if(connfd > maxfd)
			{
				maxfd = connfd;
			}
			FD_SET(connfd, &allset);
			if(--nready == 0)     // only listen can read 
			{
				continue;
			}
		}
		for(i = 0; i<=max; i++)
		{
			if(client[i] < 0)
			{
				continue;
			}
			if(FD_ISSET(client[i], &reset))
			{
				memset(buf, 0x0, sizeof(buf));
				int nread = read(client[i], buf, MAX_LINE);
				if(nread == 0)
				{
					FD_CLR(client[i], &allset);
					close(client[i]);
					client[i] = -1;
					printf("a client close!\r\n");
				}else
				{
					write(client[i], buf, MAX_LINE);
				}
				if(--nready == 0)
				{
					break;
				}
			}
		}
	}
	*/	
/*	signal(SIGCHLD, sigchld);
	int nCount = 0;
	printf("listen...\r\n");
	pid_t clipid;
	for(;;)
	{
		int len = sizeof(cliaddr);
		bzero(&cliaddr, sizeof(cliaddr));
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
		if(connfd < 0)
		{	
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				printf("accept error!\r\n");
				exit(1);
			}
			// continue;
		}
		char cliip[64];
		memset(cliip, 0x0, sizeof(cliip));
		inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
		int nPort = ntohs(cliaddr.sin_port);
		printf("accept %d connect request! from  IP:%s Port:%d\r\n",++nCount, inet_ntop(AF_INET, &cliaddr.sin_addr
,cliip,sizeof(cliip)), ntohs(cliaddr.sin_port));
		if((clipid = fork()) == 0)
		{
			close(listenfd);
			str_eho(connfd);
			printf("a client close!\r\n");
			nCount--;
			exit(0);	
		}
		close(connfd);
	}*/

}

