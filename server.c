#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 					2000
#define BUFFER_SIZE				1024

int main(int argc, char *argv[])
{
	struct sockaddr_in server_sockaddr, client_sockaddr;
	fd_set inset, tmp_inset;
	int sock_fd, sin_size;
	char rxbuf[BUFFER_SIZE], txbuf[BUFFER_SIZE];
	long *ptx_msg = NULL;
	long *prx_msg = NULL;
	int opval = 1;
	struct timeval tv;
	int ret, recvlen, sndlen;

	/*初始化发送/接收缓冲区*/
	memset(&txbuf, 0, sizeof(txbuf));
	memset(&rxbuf, 0, sizeof(rxbuf));
	ptx_msg = (long *)txbuf;
	prx_msg = (long *)rxbuf;
	
	/*服务器创建socket*/
	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		return -1;
	}

	/* 填充服务器socket结构*/
	bzero(&server_sockaddr, sizeof(struct sockaddr_in));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;

	/* 绑定服务器的socket文件描述符到端口信息*/
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opval, sizeof(opval));
	if(bind(sock_fd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		return -1;
	}

	/*初始化读文件描述符集*/
	FD_ZERO(&inset);
	FD_SET(sock_fd, &inset);

	/*设置超时时间*/
	tv.tv_sec = 1;
	tv.tv_usec = 0;	
	while(1)
	{
		/*非阻塞方式，循环测试读文件标识符是否准备就绪*/
		tmp_inset = inset;
		ret = select(sock_fd + 1, &tmp_inset, NULL, NULL, &tv);
		/*出错*/
		if (ret == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			goto ErrLable;
		}
		/*超时*/
		else if (ret == 0)
		{
			continue;
		}
		
		/*接收到数据*/
		if(FD_ISSET(sock_fd, &tmp_inset) > 0)
		{
			/*接收数据*/
			sin_size = sizeof(struct sockaddr_in);
			if ((recvlen = recvfrom(sock_fd, rxbuf, sizeof(rxbuf), 0, 
							(struct sockaddr *)&client_sockaddr, &sin_size)) == -1)
			{
				fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
				goto ErrLable;
			}

			if (recvlen > 0)
			{
				printf("Recv %ld from CliIP %s, CliPORT %d\n", *prx_msg, 
						inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port));
			}
		}
		
		/*延时1S*/
		sleep(1);

		/*将接收数据加1后继续进行发送*/
		*ptx_msg = *prx_msg + 1;

		/*发送数据*/
		if ((sndlen = sendto(sock_fd, txbuf, sizeof(txbuf), 0, 
				(struct sockaddr *)&client_sockaddr, sizeof(struct sockaddr))) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			goto ErrLable;
		}

		if (sndlen > 0)
		{
			printf("Send %ld from SerIP\n", *ptx_msg);
		}
		printf("\n");
	}

ErrLable:	
	close(sock_fd);

	return 0;
}