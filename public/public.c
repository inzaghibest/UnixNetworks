#include "public.h"

/* read n bytes from descriptor */
ssize_t readn(int fd, void *buff, size_t n)
{
	size_t nread = 0;
	size_t nLeft = n;
	char* ptr = buff;
	
	while(nLeft>0)
	{
		if((nread = read(fd, ptr, nLeft)) < 0)
		{
			if(errno == EINTR)
			{
				nread = 0;
				continue;
			}else
			{
				return -1;
			}
		}else if(nread == 0)  /* EOF */
		{
			break;
		}
		nLeft -= nread;
		ptr += nread;
	}
	return n-nLeft;  /* return already read bytes >=0 */
}
			
