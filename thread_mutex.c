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

int qid;
pthread_mutex_t write_mutex;
char dispbuf[50];

void fun_snd_1(void)
{
	struct message sndbuf;

	memset(&sndbuf, 0, sizeof(sndbuf));
	sndbuf.msg_type = 1;
	
	while(1)
	{
		/*��ȫ�ֱ���dispbuf���л������*/
		pthread_mutex_lock(&write_mutex);
		strcpy(dispbuf, "welcome to linux world!");
		strcpy(sndbuf.msg_text, dispbuf);
		pthread_mutex_unlock(&write_mutex);

		/*������Ϣ*/
		if (msgsnd(qid, &sndbuf, sizeof(sndbuf.msg_text), IPC_NOWAIT) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));

			if ((errno == EACCES) || (errno == EFAULT) 
				|| (errno == EIDRM) || (errno == EINVAL) || (errno == ENOMEM))
			{
				goto ErrLable;
			}
		}	
		sleep(2);	
	}
	
ErrLable:	
	pthread_exit(NULL);
}

void fun_snd_2(void)
{
	struct message sndbuf;
	
	memset(&sndbuf, 0, sizeof(sndbuf));
	sndbuf.msg_type = 2;
	
	while(1)
	{
		/*��ȫ�ֱ���dispbuf���л������*/
		pthread_mutex_lock(&write_mutex);
		strcpy(dispbuf, "study linux is fun!");
		strcpy(sndbuf.msg_text, dispbuf);
		pthread_mutex_unlock(&write_mutex);

		/*������Ϣ*/
		if (msgsnd(qid, &sndbuf, sizeof(sndbuf.msg_text), IPC_NOWAIT) == -1)
		{
			fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));

			if ((errno == EACCES) || (errno == EFAULT) 
				|| (errno == EIDRM) || (errno == EINVAL) || (errno == ENOMEM))
			{
				goto ErrLable;
			}
		}	
		
		sleep(2);	
	}
	
ErrLable:	
	pthread_exit(NULL);
}

void fun_rcv(void)
{
	struct message rcvbuf;
	int res;
	
	while(1)
	{
		/*��������������Ϣ*/
		memset(&rcvbuf, 0, sizeof(rcvbuf));
		if((res = msgrcv(qid, &rcvbuf, sizeof(rcvbuf.msg_text), 0, IPC_NOWAIT | MSG_NOERROR)) == -1)
		{
			//fprintf(stderr, "%s:%d  %s\n", __FUNCTION__, __LINE__, strerror(errno));
			if ((errno == EACCES) || (errno == EFAULT) 
				|| (errno == EIDRM) || (errno == EINVAL))
			{
				goto ErrLable;
			}	
		}	
		
		if(res > 0)
		{
			if(rcvbuf.msg_type == 1)
			{
				printf("Rcv \"%s\" form %ld\n", rcvbuf.msg_text, rcvbuf.msg_type);
			}
			else if(rcvbuf.msg_type == 2)
			{
				printf("Rcv \"%s\" form %ld\n", rcvbuf.msg_text, rcvbuf.msg_type);
			}
		}
	}
	
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
	if ((msgctl(qid, IPC_RMID, NULL )) == -1)
	{ 
		perror("msgctl");
	}
	
	puts("release resourse......\n");
}

/*������*/
int main(int argc, char *argv[])
{
	key_t key;
	pthread_t snd_pthread_id_1;
	pthread_t snd_pthread_id_2;
	pthread_t rcv_pthread_id;

	/*��ʼ��������*/
	pthread_mutex_init(&write_mutex, NULL);

	/*ע���źŴ�����ctrl+c*/
	signal(SIGINT, signal_handler);

	/*ע����ֹ����exit*/
	atexit(cleanup);

	/*���ɼ�ֵ*/
	if((key = ftok(".", '1')) == -1)
	{
		perror("ftok");
		return -1;
	}

	/*������Ϣ����*/
	if((qid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		perror("msgget");
		return -1;
	}

	/*���������߳�1*/
	if(pthread_create(&snd_pthread_id_1, NULL, (void *)fun_snd_1, NULL) != 0)
	{
		perror("create fun_snd_1 fail");
		return -1;
	}	

	/*���������߳�2*/
	if(pthread_create(&snd_pthread_id_2, NULL, (void *)fun_snd_2, NULL) != 0)
	{
		perror("create fun_snd_2 fail");
		return -1;
	}

	/*���������߳�*/
	if(pthread_create(&rcv_pthread_id, NULL, (void *)fun_rcv, NULL) != 0)
	{
		perror("create fun_rcv fail");
		return -1;
	}

	/*�����߳���Դ*/
	pthread_join(snd_pthread_id_1, NULL);
	pthread_join(snd_pthread_id_2, NULL);
	pthread_join(rcv_pthread_id, NULL);

	
	return 0;
}