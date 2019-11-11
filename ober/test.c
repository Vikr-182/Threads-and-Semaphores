#include "global.h"

int main(void)
{
	int wa ;
	scanf("%d",&wa);
	struct timespec ts;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int timeInMs = 1000 * wa;
	ts.tv_sec = time(NULL) + timeInMs / 1000;
	ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
	ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
	ts.tv_nsec %= (1000 * 1000 * 1000);

	//clock_gettime(CLOCK_REALTIME, &ts);
	//ts.tv_sec += wa;
	int b = pthread_cond_timedwait(&cond_empty, &mutex_empty, &ts);
	while ( b == 0)
	{
		b = pthread_cond_timedwait(&cond_empty, &mutex_empty, &ts);
	}
	printf("%d\n",b);
}
