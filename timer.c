#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

/*SIGALRM�źŴ�����*/
void timer_handler(int sig)
{
	static char time_1s = 0;
	
	printf("1s coming\n");
	if (time_1s++ >= 1)
	{
		time_1s = 0;
		printf("2s coming\n");	
	}
}

int main(int argc, char *argv[])
{
	struct itimerval value; 

	/*���1S����SIGALRM�ź�*/
	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = 0;
	value.it_interval = value.it_value;

	setitimer(ITIMER_REAL, &value, NULL);

	/*ָ���źŴ�����*/
	signal(SIGALRM, timer_handler);
	
	while(1);
	
	return 0;
}


