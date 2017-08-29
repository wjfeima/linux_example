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

	/*�����߳����ȼ�ʱ��ȷ����root�û�*/
	if ((uid = getuid()) == 0)
	{
		printf("The current user is root\n");
	}
	else
	{
		printf("The current user is not root\n");
		//return -1;
	}
	
	/*��ʼ���߳�����*/
	pthread_attr_init(&pattr);

	//===========================================
	/*��ȡϵͳĬ�ϵ��Ȳ���*/
	pthread_attr_getschedpolicy(&pattr, &policy);
	if (policy == SCHED_OTHER)
	{
		/*��ʱ����*/
		strcpy(tempbuf, "SCHED_OTHER");	
	}
	else if (policy == SCHED_FIFO)
	{
		/*ʵʱ���Ȳ��ԣ��ȵ��ȷ���*/
		strcpy(tempbuf, "SCHED_FIFO");	
	}
	else if (policy == SCHED_RR)
	{
		/*ʵʱ���Ȳ��ԣ�ʱ��Ƭ��ת*/
		strcpy(tempbuf, "SCHED_RR"); 
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current policy is %s\n", tempbuf);

	//===================================================
	/*��ȡSCHED_OTHER ���ȼ���Χ*/
	priority = sched_get_priority_min(policy);
	printf("SCHED_OTHER priority_min is %d\n", priority);
	priority = sched_get_priority_max(policy);
	printf("SCHED_OTHER priority_max is %d\n", priority);
	
	//=================================================
	/*��ȡϵͳĬ�ϵļ̳�����*/
	pthread_attr_getinheritsched(&pattr, &inherit); 
	if (inherit == PTHREAD_INHERIT_SCHED)
	{
		/*�̳�*/
		strcpy(tempbuf, "PTHREAD_INHERIT_SCHED");	
	}
	else if (inherit == PTHREAD_EXPLICIT_SCHED)
	{
		/*�Ǽ̳�*/
		strcpy(tempbuf, "PTHREAD_EXPLICIT_SCHED");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current inherit is %s\n", tempbuf);

	//===================================================
	/*��ȡϵͳĬ�ϰ�����*/
	pthread_attr_getscope(&pattr, &scope); 
	if (scope == PTHREAD_SCOPE_SYSTEM)
	{
		/*��*/
		strcpy(tempbuf, "PTHREAD_SCOPE_SYSTEM");	
	}
	else if (scope == PTHREAD_SCOPE_PROCESS)
	{
		/*�ǰ�*/
		strcpy(tempbuf, "PTHREAD_SCOPE_PROCESS");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current scope is %s\n", tempbuf);

	//======================================================
	/*��ȡϵͳĬ�Ϸ������*/
	pthread_attr_getdetachstate(&pattr, &detachstate); 
	if (detachstate == PTHREAD_CREATE_DETACHED)
	{
		/*����*/
		strcpy(tempbuf, "PTHREAD_CREATE_DETACHED");	
	}
	else if (detachstate == PTHREAD_CREATE_JOINABLE)
	{
		/*�Ƿ���*/
		strcpy(tempbuf, "PTHREAD_CREATE_JOINABLE");	
	}
	else
	{
		strcpy(tempbuf, "Error Occure"); 
	}
	printf("current detachstate is %s\n", tempbuf);
	
	//==========================================================
	/*�����̰߳�����*/
	pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
	
	/*�����̵߳��Ȳ���ΪSCHED_FIFO*/
	pthread_attr_setschedpolicy(&pattr, SCHED_FIFO);

	/*��ȡSCHED_FIFO ���ȼ���Χ*/
	priority = sched_get_priority_min(SCHED_FIFO);
	printf("SCHED_FIFO priority_min is %d\n", priority);
	priority = sched_get_priority_max(SCHED_FIFO);
	printf("SCHED_FIFO priority_max is %d\n", priority);

	/*���ò��̳н��̵����ȼ�*/
	pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED); 
	
	/*�����߳�1���ȼ�Ϊ10*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 10;
	pthread_attr_setschedparam(&pattr, &param);

	/*�����߳�1*/
	if (pthread_create(&pthread_id_1, &pattr, (void *)fun_thread_1, "linux") != 0)
	{
		printf("create fun_thread_1 thread fail\n");
		
	}

	/*�����߳�2���ȼ�Ϊ20*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 20;
	pthread_attr_setschedparam(&pattr, &param);
	
	/*�����߳�2*/
	if (pthread_create(&pthread_id_2, &pattr, (void *)fun_thread_2, "vxworks") != 0)
	{
		printf("create fun_thread_2 thread fail\n");
	}

	/*�����߳�3���ȼ�Ϊ30*/
	pthread_attr_getschedparam(&pattr, &param);
	param.sched_priority = 30;
	pthread_attr_setschedparam(&pattr, &param);
	
	/*�����߳�3*/
	if (pthread_create(&pthread_id_3, &pattr, (void *)fun_thread_3, "windows") != 0)
	{
		printf("create fun_thread_3 thread fail\n");
	}

	/*����߳�����*/
	pthread_attr_destroy(&pattr);

	/*�ȴ��߳̽����������߳���Դ*/
	pthread_join(pthread_id_1, NULL);
	pthread_join(pthread_id_2, NULL);
	pthread_join(pthread_id_3, NULL);
	
	return 0;
}










