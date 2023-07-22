#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

int count = 0;
int nthread = 1;
int worker_loop_cnt = 1;

#ifndef __APPLE__
sem_t semaphore; // semaphore
#else
sem_t *semaphore; // semaphore
#endif

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

	// init semaphore
#ifndef __APPLE__
	sem_init(&semaphore, 0, 1);
#else
	semaphore = sem_open("/semaphore", O_CREAT, 0644, 1);
	sem_unlink("/semaphore");
#endif

	printf("main: begin (count = %d)\n", count);

	// create threads
	for (i = 0; i < nthread; i++)
		assert(pthread_create(&th[i], NULL, worker, (void *)i) == 0);

	// wait for threads terminated
	for (i = 0; i < nthread; i++)
		assert(pthread_join(th[i], &value) == 0);

	// end semaphore
#ifndef __APPLE__
	sem_destroy(&semaphore);
#else
	sem_close(semaphore);
#endif

	printf("main: done (count = %d)\n", count);

	return 0;
}

static void *worker(void *num)
{
	int number = (int)(size_t)num;

	// increase semaphore value
#ifndef __APPLE__
	sem_wait(&semaphore);
#else
	sem_wait(semaphore);
#endif

	// handle count: locked work
	for (int i = 0; i < worker_loop_cnt; i++)
		count++;

	printf("Thread number %d: %d\n", number, count);

	// decrease semaphore value
#ifndef __APPLE__
	sem_post(&semaphore);
#else
	sem_post(semaphore);
#endif

	return NULL;
}
