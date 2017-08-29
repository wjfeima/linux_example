#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define	BUFFER_SIZE	100

/*消息结构体*/
struct message
{
	long msg_type;
	char msg_text[BUFFER_SIZE];
};

int qid_snd_msg, qid_rcv_msg;

/*发送线程*/
void pthread_snd_msg(void)
{
	struct message sndbuf, rcvbuf;
	long *psnd_msg = NULL;
	long *prcv_msg = NULL;
	int ret;
	
	/*初始化发送/接收缓冲区*/
	memset(&sndbuf, 0, sizeof(sndbuf));
	memset(&rcvbuf, 0, sizeof(rcvbuf));
	psnd_msg = (long *)(sndbuf.msg_text);
	prcv_msg = (long *)(rcvbuf.msg_text);

	/*获取进程ID号*/
	sndbuf.msg_type = getpid();
	
	while(1)
	{
		/*发送消息*/
		if (msgsnd(qid_snd_msg, &sndbuf, sizeof(sndbuf.msg_text), IPC_NOWAIT) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));

			if ((errno == EACCES) || (errno == EFAULT) 
				|| (errno == EIDRM) || (errno == EINVAL) || (errno == ENOMEM))
			{
				goto ErrLable;
			}
		}	

		printf("send %ld from pid %ld\n", *psnd_msg, sndbuf.msg_type);

		/*接收消息，阻塞等待*/
		memset(&rcvbuf, 0, sizeof(rcvbuf));
		if ((ret = msgrcv(qid_rcv_msg, &rcvbuf, sizeof(rcvbuf.msg_text), 0, MSG_NOERROR)) == -1)
		{	
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			
			if ((errno == EACCES) || (errno == EFAULT) 
				|| (errno == EIDRM) || (errno == EINVAL))
			{
				goto ErrLable;
			}	
		}

		if (ret > 0)
		{
			printf("recv %ld from pid %ld\n", *prcv_msg, rcvbuf.msg_type);
		}
		
		/*将接收数据加1后继续进行发送*/
		*psnd_msg = *prcv_msg + 1;

		/*延时1S*/
		sleep(1);

		printf("\n");
	}

	/*线程退出*/
ErrLable:	
	pthread_exit(NULL);	
}

/*信号处理函数*/
void signal_handler(int sig)
{
	puts("receive ctrl+c......");
	exit(1);	
}

/*main进程退出时执行资源回收函数*/
void cleanup(void)
{
	/* 删除消息队列*/
	if ((msgctl(qid_snd_msg, IPC_RMID, NULL )) == -1)
	{ 
		perror("msgctl");
	}

	if ((msgctl(qid_rcv_msg, IPC_RMID, NULL )) == -1)
	{ 
		perror("msgctl");
	}
	
	puts("release resourse......\n");
}

/*主进程*/
int main(int argc, char *argv[])
{
	key_t key_snd_msg, key_rcv_msg;
	pthread_t send_pthread_id;

	/*注册信号处理函数ctrl+c*/
	signal(SIGINT, signal_handler);

	/*注册终止函数exit*/
	atexit(cleanup);

	/*根据不同的路径与关键字产生标准的key*/
	if ((key_snd_msg = ftok(".", '1')) == -1)
	{
		perror("ftok");
		return -1;
	}

	if ((key_rcv_msg = ftok(".", '2')) == -1)
	{
		perror("ftok");
		return -1;
	}

	printf("key_snd_msg = 0x%08x, key_rcv_msg = 0x%08x\n", key_snd_msg, key_rcv_msg);
	
	/*创建消息队列*/
	if ((qid_snd_msg = msgget(key_snd_msg, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		perror("msgget");
		return -1;
	}	

	if ((qid_rcv_msg = msgget(key_rcv_msg, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		perror("msgget");
		return -1;
	}	

	printf("qid_snd_msg = %d, qid_rcv_msg = %d\n", qid_snd_msg, qid_rcv_msg);
	
	/*创建发送消息线程*/
	if(pthread_create(&send_pthread_id, NULL, (void *)pthread_snd_msg, NULL) != 0)
	{
		perror("create msgsnd thread fail");
	}
	
	/*等待线程结束，回收线程资源*/
	pthread_join(send_pthread_id, NULL);

	return 0;
}


