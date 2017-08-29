#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 				2000
#define BUFFER_SIZE			1024

int main(int argc, char *argv[])
{
	struct sockaddr_in server_sockaddr, from_sockaddr;
	char rxbuf[BUFFER_SIZE], txbuf[BUFFER_SIZE];
	int sock_fd, sendbytes;
	int ret, recvlen, sndlen;
	int sin_size;
	long *ptx_msg = NULL;
	long *prx_msg = NULL;

	/*��ʼ������/���ջ�����*/
	memset(&txbuf, 0, sizeof(txbuf));
	memset(&rxbuf, 0, sizeof(rxbuf));
	ptx_msg = (long *)txbuf;
	prx_msg = (long *)rxbuf;

	/*����������socket*/
	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		return -1;
	}

	/* ��������socket�ṹ*/
	bzero(&server_sockaddr, sizeof(struct sockaddr_in));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	inet_aton("127.0.0.1", &server_sockaddr.sin_addr);
	
	while(1)
	{
		/*��������*/
		if ((sndlen = sendto(sock_fd, txbuf, sizeof(txbuf), 0, 
				(struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr))) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			goto ErrLable;
		}

		if (sndlen > 0)
		{
			printf("Send %ld from CliIP\n", *ptx_msg);
		}

		/*������ʽ����������*/
		sin_size = sizeof(struct sockaddr_in);
		if ((recvlen = recvfrom(sock_fd, rxbuf, sizeof(rxbuf), 0, 
						(struct sockaddr *)&from_sockaddr, &sin_size)) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			goto ErrLable;
		}
		
		if (recvlen > 0)
		{
			printf("Recv %ld from SerIP %s, SerPORT %d\n", *prx_msg, 
						inet_ntoa(from_sockaddr.sin_addr), ntohs(from_sockaddr.sin_port));	
		}

		/*���������ݼ�1��������з���*/
		*ptx_msg = *prx_msg + 1;

		/*��ʱ1S*/
		sleep(1);
		printf("\n");

	}	
	
ErrLable:	
	close(sock_fd);

	return 0;
}