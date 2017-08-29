#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem;

void trigger(void)
{
	int i, value;

	for (i = 0; i < 5; i++)
	{
		/*发送信号量*/
		sem_post(&sem);
		sem_getvalue(&sem, &value);
		printf("sem value: %d, trigging %d\n", value, i + 1);
	}

	pthread_exit(NULL);
}

void action(void)
{
	int i = 0, value;

	/*延时2S*/
	sleep(2);

	while(1)
	{
		/*获取信号量*/
		sem_wait(&sem);
		sem_getvalue(&sem, &value);
		printf("sem value: %d, acting %d\n", value, ++i);
	}

	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
	pthread_t trigger_pthread_id;
	pthread_t action_pthread_id;
	int value;

	/*初始化线程信号量为0*/
	sem_init(&sem, 0, 0);

	/*创建线程*/
	if (pthread_create(&trigger_pthread_id, NULL, (void *)trigger, NULL) != 0)
	{
		printf("create trigger thread fail\n");
	}

	if (pthread_create(&action_pthread_id, NULL, (void *)action, NULL) != 0)
	{
		printf("create action thread fail\n");
	}

	/*获取信号量值*/
	sem_getvalue(&sem, &value);
	printf("sem value: %d\n", value);

	/*回收资源*/
	pthread_join(trigger_pthread_id, NULL);
	pthread_join(action_pthread_id, NULL);
	
	return 0;
}
