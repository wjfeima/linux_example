#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

pthread_mutex_t pro_mutex;
pthread_cond_t	pro_syn;

/*条件判断变量*/
int syn_num = 0;

void frame_snd(void)
{
	unsigned char cnt = 0;
	
	while(1)
	{
		if (cnt++ >= 2)
		{
			cnt = 0;
			printf("signaling\n");
			
			pthread_mutex_lock(&pro_mutex);
			syn_num = 1;
			/*唤醒被阻塞的线程*/
			pthread_cond_signal(&pro_syn);
			pthread_mutex_unlock(&pro_mutex);
		}

		sleep(1);
	}
	
	pthread_exit(NULL);
}

void frame_rcv(void)
{
	while(1)
	{
		pthread_mutex_lock(&pro_mutex);

		while (syn_num == 0)
		{
			/*阻塞等待唤醒*/
			pthread_cond_wait(&pro_syn, &pro_mutex);
		}

		syn_num = 0;
		pthread_mutex_unlock(&pro_mutex);

		printf("wakeup\n");
	}	

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t frame_snd_pthread_id;
	pthread_t frame_rcv_pthread_id;

	/*互斥锁/条件变量初始化*/
	pthread_mutex_init(&pro_mutex, NULL);
	pthread_cond_init(&pro_syn, NULL);

	/*创建线程*/
	if (pthread_create(&frame_snd_pthread_id, NULL, (void *)frame_snd, NULL) != 0)
	{
		perror("pthread_create_snd");
	}

	if (pthread_create(&frame_rcv_pthread_id, NULL, (void *)frame_rcv, NULL) != 0)
	{
		perror("pthread_create_rcv");
	}

	/*回收线程资源*/
	pthread_join(frame_snd_pthread_id, NULL);
	pthread_join(frame_rcv_pthread_id, NULL);
	
	return 0;
}











