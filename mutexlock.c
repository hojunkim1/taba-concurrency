#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

int count = 0;
int nthread = 1;
int worker_loop_cnt = 1;
pthread_mutex_t lock; // mutex variable

static void *worker(void *num);

int main(int argc, char *argv[])
{
	// local
	pthread_t *th;
	void *value;
	long i;

	// check number of args
	if (argc < 3)
	{
		fprintf(stderr, "%s parameter: nthread, worker_loop_cnt\n", argv[0]);
		exit(-1);
	}

	// init args
	nthread = atoi(argv[1]);
	worker_loop_cnt = atoi(argv[2]);

	// set memory for threads
	th = malloc(sizeof(pthread_t) * nthread);

	// init lock
	pthread_mutex_init(&lock, NULL);

	printf("main: begin (count = %d)\n", count);

	// create threads
	for (i = 0; i < nthread; i++)
		assert(pthread_create(&th[i], NULL, worker, (void *)i) == 0);

	// wait for threads terminated
	for (i = 0; i < nthread; i++)
		assert(pthread_join(th[i], &value) == 0);

	printf("main: done (count = %d)\n", count);

	return 0;
}

static void *worker(void *num)
{
	int number = (int)(size_t)num;

	// handle count: locked work
	pthread_mutex_lock(&lock);
	for (int i = 0; i < worker_loop_cnt; i++)
		count++;
	pthread_mutex_unlock(&lock);

	printf("Thread number %d: %d\n", number, count);

	return NULL;
}
