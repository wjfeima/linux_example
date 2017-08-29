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

/*��Ϣ�ṹ��*/
struct message
{
	long msg_type;
	char msg_text[BUFFER_SIZE];
};

int qid_snd_msg, qid_rcv_msg;

/*�����߳�*/
void pthread_snd_msg(void)
{
	struct message sndbuf, rcvbuf;
	long *psnd_msg = NULL;
	long *prcv_msg = NULL;
	int ret;
	
	/*��ʼ������/���ջ�����*/
	memset(&sndbuf, 0, sizeof(sndbuf));
	memset(&rcvbuf, 0, sizeof(rcvbuf));
	psnd_msg = (long *)(sndbuf.msg_text);
	prcv_msg = (long *)(rcvbuf.msg_text);

	/*��ȡ����ID��*/
	sndbuf.msg_type = getpid();
	
	while(1)
	{
		/*������Ϣ*/
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

		/*������Ϣ�������ȴ�*/
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
		
		/*���������ݼ�1��������з���*/
		*psnd_msg = *prcv_msg + 1;

		/*��ʱ1S*/
		sleep(1);

		printf("\n");
	}

	/*�߳��˳�*/
ErrLable:	
	pthread_exit(NULL);	
}

/*�źŴ�����*/
void signal_handler(int sig)
{
	puts("receive ctrl+c......");
	exit(1);	
}

/*main�����˳�ʱִ����Դ���պ���*/
void cleanup(void)
{
	/* ɾ����Ϣ����*/
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

/*������*/
int main(int argc, char *argv[])
{
	key_t key_snd_msg, key_rcv_msg;
	pthread_t send_pthread_id;

	/*ע���źŴ�����ctrl+c*/
	signal(SIGINT, signal_handler);

	/*ע����ֹ����exit*/
	atexit(cleanup);

	/*���ݲ�ͬ��·����ؼ��ֲ�����׼��key*/
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
	
	/*������Ϣ����*/
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
	
	/*����������Ϣ�߳�*/
	if(pthread_create(&send_pthread_id, NULL, (void *)pthread_snd_msg, NULL) != 0)
	{
		perror("create msgsnd thread fail");
	}
	
	/*�ȴ��߳̽����������߳���Դ*/
	pthread_join(send_pthread_id, NULL);

	return 0;
}


