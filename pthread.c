#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

int count = 0;			 // shared resource
int nthread = 1;		 // number of thread
int worker_loop_cnt = 1; // number of loop

// function work in threads
static void *worker(void *num);

int main(int argc, char *argv[])
{
	pthread_t *th; // thread pointer
	void *value;   // value pointer
	long i;		   // for-loop

	// check number of args
	if (argc < 3)
	{
		fprintf(stderr, "%s parameter: nthread, worker_loop_cnt\n", argv[0]);
		exit(-1);
	}

	// atoi: convert string type to int
	nthread = atoi(argv[1]);		 // thread number
	worker_loop_cnt = atoi(argv[2]); // loop number

	// set memory for threads
	th = malloc(sizeof(pthread_t) * nthread);

	printf("main: begin (count = %d)\n", count);

	// create threads
	for (i = 0; i < nthread; i++)
		// pthread_create(thread, attribute, start routine, arg)
		// thread: structure of thread
		// attribute: attribute of thread, normally NULL
		// start routine: function pointer
		// arg: the only argument for start routine
		assert(pthread_create(&th[i], NULL, worker, (void *)i) == 0);

	// wait for threads terminated
	for (i = 0; i < nthread; i++)
		assert(pthread_join(th[i], &value) == 0);

	printf("main: done (count = %d)\n", count);

	return 0;
}

static void *worker(void *num)
{
	// convert num type int manually
	int number = (int)(size_t)num;

	// handle count
	for (int i = 0; i < worker_loop_cnt; i++)
		count++;

	printf("Thread number %d: %d\n", number, count);

	return NULL;
}
