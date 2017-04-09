#include <netinet/tcp.h>  /* for TCP_xxx defines */
#include "public.h"

static char strres[128];
union val
{
	int i_val;
	long l_val;
	struct timeval timeval_val;
	struct linger linger_val;
} val;

static char *sock_str_flag(union val *, int);
static char *sock_str_int(union val *,  int);
static char *sock_str_linger(union val *, int);
static char *sock_str_timeval(union val *, int);

char *sock_str_flag(union val* pval, int len)
{
	memset(strres, 0x0, sizeof(strres));
	if(len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) is not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "%s", pval->i_val == 0 ? "off" : "on");
	return (strres);
}

char *sock_str_int(union val* pval, int len)
{
	memset(strres, 0x0, sizeof(strres));
	if(len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) is not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "%d", pval->i_val);
	return (strres);
}

char *sock_str_linger(union val* pval, int len)
{
	memset(strres, 0x0, sizeof(strres));
	if(len != sizeof(struct linger))
		snprintf(strres, sizeof(strres), "size (%d) is not sizeof(struct linger)", len);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", pval->linger_val.l_onoff, pval->linger_val.l_linger);
	return (strres);
}

char *sock_str_timeval(union val* pval, int len)
{
	memset(strres, 0x0, sizeof(strres));
	if(len != sizeof(struct timeval))
		snprintf(strres, sizeof(strres), "size (%d) is not sizeof(struct timeval)", len);
	else
		snprintf(strres, sizeof(strres), "%d sec %d usec", (int)pval->timeval_val.tv_sec, (int)pval->timeval_val.tv_usec);
	return (strres);
}



struct sock_opts{
	const char *opt_str;
	int opt_level;
	int opt_name;
	char *(*opt_val_str)(union val*, int);
} sock_opts[] = {
			{"SO_BROADCAST(UDP�㲥��Ϣ)", SOL_SOCKET, SO_BROADCAST, sock_str_flag},
			{"SO_DEBUG(TCP������Ϣ)", SOL_SOCKET, SO_DEBUG, sock_str_flag},
			{"SO_DONTROUTE(���ƹ�·�ɱ�)", SOL_SOCKET, SO_DONTROUTE, sock_str_flag},
			{"SO_ERROR(��ȡ�׽��ִ���)", SOL_SOCKET, SO_ERROR, sock_str_int},
			{"SO_KEEPALIVE(���ִ��ѡ��)", SOL_SOCKET, SO_KEEPALIVE, sock_str_flag},
			{"SO_LINGER(close��ʽ)", SOL_SOCKET, SO_LINGER, sock_str_linger},
			{"SO_OOBINLINE(���������������)", SOL_SOCKET, SO_OOBINLINE, sock_str_flag},
			{"SO_RCVBUF(���ջ�����)", SOL_SOCKET, SO_RCVBUF, sock_str_int},
			{"SO_SNDBUF(���ͻ�����)", SOL_SOCKET, SO_SNDBUF, sock_str_int},
			{"SO_RCVLOWAT(���յ�ˮλ���)", SOL_SOCKET, SO_RCVLOWAT, sock_str_int},
			{"SO_SNDLOWAT(���͵�ˮλ���)", SOL_SOCKET, SO_SNDLOWAT, sock_str_int},
			{"SO_RCVTIMEO(���ճ�ʱֵ)", SOL_SOCKET, SO_RCVTIMEO, sock_str_timeval},
			{"SO_SNDTIMEO(���ͳ�ʱֵ)", SOL_SOCKET, SO_SNDTIMEO, sock_str_timeval},
			{"SO_REUSEADDR(�ظ��󶨵�ַ)", SOL_SOCKET, SO_REUSEADDR, sock_str_flag},
#ifdef SO_REUSEPORT
			{"SO_REUSEPORT(�ظ��󶨶˿�)", SOL_SOCKET, SO_REUSEPORT, sock_str_flag},
#else
			{"SO_REUSEPORT", 0, 0, NULL},
#endif 
			{"SO_TYPE(�׽�������)", SOL_SOCKET, SO_TYPE, sock_str_int},
#ifdef SO_USELOOPBACK
			{"SO_USELOOPBACK", SOL_SOCKET, SO_USELOOPBACK, sock_str_flag},
#else
			{"SO_USELOOPBACK", 0, 0, NULL},
#endif
			{"IP_TOS", IPPROTO_IP, IP_TOS, sock_str_int},
			{"IP_TTL", IPPROTO_IP, IP_TTL, sock_str_int},
			//{"IPV6_DONTFRAG", IPPROTO_IPV6, IPV6_DONTFRAG, sock_str_flag},
			{"IPV6_UNICAST_HOPS", IPPROTO_IPV6, IPV6_UNICAST_HOPS, sock_str_int},
			{"IPV6_V6ONLY", IPPROTO_IPV6, IPV6_V6ONLY, sock_str_flag},
			{"TCP_MAXSEG(���ֽڴ�СMSS)", IPPROTO_TCP, TCP_MAXSEG, sock_str_int},
			{"TCP_NODELAY(��ֹTCP��Nagle�㷨)", IPPROTO_TCP, TCP_NODELAY, sock_str_flag},
			//{"SCTP_AUTOCLOSE", IPPROTO_SCTP, SCTP_AUTOCLOSE, sock_str_int},
			//{"SCTP_MAXBURST", IPPROTO_SCTP, SCTP_MAXBURST, sock_str_int},
			//{"SCTP_MAXSEG", IPPROTO_SCTP, SCTP_MAXSEG, sock_str_int},
			//{"SCTP_NODELAY", IPPROTO_SCTP, SCTP_NODELAY, sock_str_flag},
			{NULL, 0, 0, NULL}
};

int main(int argc, char* argv)
{
	socklen_t len;
	struct sock_opts* psopt = sock_opts;
	int sockfd;
	//double dval = 1536000*0.06/8;
	//printf("%.2f\r\n", dval);
	printf("sizeof(int) = %d\r\n", sizeof(int));
	for(psopt; psopt->opt_str != NULL; psopt++)
	{
		printf("%s: ", psopt->opt_str);
		if(psopt->opt_val_str == NULL)
		{
			printf("undefine\r\n");
		}else
		{
			switch(psopt->opt_level){
				case SOL_SOCKET:
				case IPPROTO_IP:
				case IPPROTO_TCP:
					sockfd = socket(AF_INET, SOCK_STREAM, 0);
					break;
#ifdef IPPROTO_IPV6
				case IPPROTO_IPV6:
					sockfd = socket(AF_INET6, SOCK_STREAM, 0);
					break;
#endif
#ifdef IPPROTO_SCTP
				case IPPROTO_SCTP:
					sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
					break;
#endif
				default:
					printf("can not create fd for opt level:%d\r\n", psopt->opt_level);
					exit(1);
					break;
				}
			len = sizeof(val);
			if(getsockopt(sockfd, psopt->opt_level, psopt->opt_name, &val, &len) == -1)
			{
				printf("getsockopt error!\r\n");
				exit(1);
			}else
			{
				printf("default = %s\r\n", (*psopt->opt_val_str)(&val, len));
			}
			close(sockfd);
		}
	}
	printf("main end\r\n");
	exit(0);
}
