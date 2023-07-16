#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

int turn = 0;
int nthread = 1;
int max_work_cnt = 0;
int work_cnt = 1;
int *work_cnt_per_thread;

pthread_mutex_t lock;

static void *worker(void *num);

int main(int argc, char *argv[])
{
	pthread_t *th;
	void *value;
	long i;

	if (argc < 3)
	{
		fprintf(stderr, "%s parameter : nthread, max_work_cnt\n", argv[0]);
		exit(-1);
	}

	nthread = atoi(argv[1]);
	max_work_cnt = atoi(argv[2]);

	th = malloc(sizeof(pthread_t) * nthread);
	work_cnt_per_thread = malloc(sizeof(int) * nthread);

	pthread_mutex_init(&lock, NULL);

	for (i = 0; i < nthread; i++)
		assert(pthread_create(&th[i], NULL, worker, (void *)i) == 0);

	for (i = 0; i < nthread; i++)
		assert(pthread_join(th[i], &value) == 0);

	for (i = 0; i < nthread; i++)
		printf("Thread %ld work_cnt : %d\n", i, work_cnt_per_thread[i]);

	free(th);
	free(work_cnt_per_thread);

	printf("Complete\n");

	return 0;
}

static void *worker(void *num)
{
	int number = (int)(size_t)num;

	while (work_cnt < max_work_cnt)
	{
		while (turn != number)
		{
		}

		pthread_mutex_lock(&lock);

		work_cnt++;
		work_cnt_per_thread[number]++;
		turn = (turn + 1) % nthread;

		pthread_mutex_unlock(&lock);
	}

	return NULL;
}
