#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define SEM_COUNT 3

int count[SEM_COUNT];
int working[SEM_COUNT];
int nthread = 1;
int worker_loop_cnt = 1;

pthread_mutex_t lock;

#ifndef __APPLE__
sem_t semaphore;
#else
sem_t *semaphore;
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

	// init lock
	pthread_mutex_init(&lock, NULL);

	// init semaphore
#ifndef __APPLE__
	sem_init(&semaphore, 0, SEM_COUNT);
#else
	semaphore = sem_open("/semaphore", O_CREAT, 0644, SEM_COUNT);
	sem_unlink("/semaphore");
#endif

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

	free(th);

	printf("Count array: ");
	for (i = 0; i < SEM_COUNT; i++)
		printf("%d ", count[i]);

	printf("\nComplete\n");

	return 0;
}

static void *worker(void *num)
{
	int number = (int)(size_t)num;
	int count_index = -1;
	int i = 0;

	// increase semaphore value
#ifndef __APPLE__
	sem_wait(&semaphore);
#else
	sem_wait(semaphore);
#endif

	// find empty working space : should be locked
	pthread_mutex_lock(&lock);
	for (i = 0; i < SEM_COUNT; i++)
	{
		if (working[i] == 0)
		{
			working[i] = 1;
			count_index = i;
			break;
		}
	}
	pthread_mutex_unlock(&lock);

	if (count_index == -1)
	{
		fprintf(stderr, "Thread number %d: count_index < 0", number);
		exit(-1);
	}

	// handle count
	for (i = 0; i < worker_loop_cnt; i++)
		count[count_index]++;

	// empty working space : should be locked
	pthread_mutex_lock(&lock);
	working[count_index] = 0;
	pthread_mutex_unlock(&lock);

	// decrease semaphore value
#ifndef __APPLE__
	sem_post(&semaphore);
#else
	sem_post(semaphore);
#endif

	return NULL;
}
