#include <stdlib.h>     
#include <stdio.h> 
#include <string.h> 
#include <errno.h>     
#include <pthread.h>
#include <semaphore.h>

void fun_thread_1(char *pargs)
{
	unsigned long pthid = pthread_self();
	
	while(1)
	{
		printf("%s: %s from %lu\n", __FUNCTION__, pargs, pthid);
		sleep(1);	
	}

	pthread_exit(NULL);	
}

void fun_thread_2(char *pargs)
{
	unsigned long pthid = pthread_self();
	
	while(1)
	{
		printf("%s: %s from %lu\n", __FUNCTION__, pargs, pthid);
		sleep(1);	
	}

	pthread_exit(NULL);	
}

void fun_thread_3(char *pargs)
{
	unsigned long pthid = pthread_self();
	
	while(1)
	{
		printf("%s: %s from %lu\n", __FUNCTION__, pargs, pthid);
		sleep(1);	
	
	}

	pthread_exit(NULL);	
}


int main(int argc, char *argv[])
{
	pthread_attr_t pattr;
	struct sched_param param;
	pthread_t pthread_id_1, pthread_id_2, pthread_id_3;
	int policy, priority, inherit, scope, detachstate;
	char tempbuf[50];
	int uid;

	/*设置线程优先级时需确保是root用户*/
	if ((uid = getuid()) == 0)
	{
		printf("The current user is root\n");
	}
	else
	{
		printf("The current user is not root\n");
		//return -1;
	}
	
	/*初始化线程属性*/
	pthread_attr_init(&pattr);

	//===========================================
	/*获取系统默认调度策略*/
	pthread_attr_getschedpolicy(&pattr, &policy);
	if (policy == SCHED_OTHER)
	{
		/*分时调度*/
		strcpy(tempbuf, "SCHED_OTHER");	
	}
	else if (policy == SCHED_FIFO)
	{
		/*实时调度策略，先到先服务*/
		strcpy(tempbuf, "SCHED_FIFO");	
	}
	else if (policy == SCHED_RR)
	{
		/*实时调度策略，时间片轮转*/
		strcpy(tempbuf, "SCHED_RR"); 
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current policy is %s\n", tempbuf);

	//===================================================
	/*获取SCHED_OTHER 优先级范围*/
	priority = sched_get_priority_min(policy);
	printf("SCHED_OTHER priority_min is %d\n", priority);
	priority = sched_get_priority_max(policy);
	printf("SCHED_OTHER priority_max is %d\n", priority);
	
	//=================================================
	/*获取系统默认的继承属性*/
	pthread_attr_getinheritsched(&pattr, &inherit); 
	if (inherit == PTHREAD_INHERIT_SCHED)
	{
		/*继承*/
		strcpy(tempbuf, "PTHREAD_INHERIT_SCHED");	
	}
	else if (inherit == PTHREAD_EXPLICIT_SCHED)
	{
		/*非继承*/
		strcpy(tempbuf, "PTHREAD_EXPLICIT_SCHED");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current inherit is %s\n", tempbuf);

	//===================================================
	/*获取系统默认绑定属性*/
	pthread_attr_getscope(&pattr, &scope); 
	if (scope == PTHREAD_SCOPE_SYSTEM)
	{
		/*绑定*/
		strcpy(tempbuf, "PTHREAD_SCOPE_SYSTEM");	
	}
	else if (scope == PTHREAD_SCOPE_PROCESS)
	{
		/*非绑定*/
		strcpy(tempbuf, "PTHREAD_SCOPE_PROCESS");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current scope is %s\n", tempbuf);

	//======================================================
	/*获取系统默认分离策略*/
	pthread_attr_getdetachstate(&pattr, &detachstate); 
	if (detachstate == PTHREAD_CREATE_DETACHED)
	{
		/*分离*/
		strcpy(tempbuf, "PTHREAD_CREATE_DETACHED");	
	}
	else if (detachstate == PTHREAD_CREATE_JOINABLE)
	{
		/*非分离*/
		strcpy(tempbuf, "PTHREAD_CREATE_JOINABLE");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current detachstate is %s\n", tempbuf);
	
	//==========================================================
	/*设置线程绑定属性*/
	pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
	
	/*设置线程调度策略为SCHED_FIFO*/
	pthread_attr_setschedpolicy(&pattr, SCHED_FIFO);

	/*获取SCHED_FIFO 优先级范围*/
	priority = sched_get_priority_min(SCHED_FIFO);
	printf("SCHED_FIFO priority_min is %d\n", priority);
	priority = sched_get_priority_max(SCHED_FIFO);
	printf("SCHED_FIFO priority_max is %d\n", priority);

	/*设置不继承进程的优先级*/
	pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED); 
	
	/*设置线程1优先级为10*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 10;
	pthread_attr_setschedparam(&pattr, &param);

	/*创建线程1*/
	if (pthread_create(&pthread_id_1, &pattr, (void *)fun_thread_1, "linux") != 0)
	{
		printf("create fun_thread_1 thread fail\n");
		
	}

	/*设置线程2优先级为20*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 20;
	pthread_attr_setschedparam(&pattr, &param);
	
	/*创建线程2*/
	if (pthread_create(&pthread_id_2, &pattr, (void *)fun_thread_2, "vxworks") != 0)
	{
		printf("create fun_thread_2 thread fail\n");
	}

	/*设置线程3优先级为30*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 30;
	pthread_attr_setschedparam(&pattr, &param);
	
	/*创建线程3*/
	if (pthread_create(&pthread_id_3, &pattr, (void *)fun_thread_3, "windows") != 0)
	{
		printf("create fun_thread_3 thread fail\n");
	}

	/*清除线程属性*/
	pthread_attr_destroy(&pattr);

	/*等待线程结束，回收线程资源*/
	pthread_join(pthread_id_1, NULL);
	pthread_join(pthread_id_2, NULL);
	pthread_join(pthread_id_3, NULL);
	
	return 0;
}










