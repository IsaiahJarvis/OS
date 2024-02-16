#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define INF 999

// global variables
sem_t writeLock;
pthread_mutex_t readLock;
int** graph;
int** dist;
int reader;

/*
 * makeDist
 *
 * loads values from graph into dist matrix
 * dist[ui][vi] = wi
 * dist[vi][ui] = wi
 *
 * int n: number of nodes in graph being input (size of matrix)
 *
 * int m: number of link entries
 */
void makeDist(int n, int m);

/*
 * makeGraph
 *
 * allocates space for the global input graph and loads values from user into graph
 *
 * int m: number of link entries
 */
void makeGraph(int m);

/*
 * populateMatrix
 *
 * randomly generates an undirectional (n * n) matrix
 *
 * int n: number of nodes (dimensions of matrix)
 *
 */
void populateMatrix(int n);

/*
 * doFW
 *
 * FW worker function
 * runs for the j loop of the FW algorithm
 * implements semaphores and mutex to control access
 *
 * void *arg_s: struct containing n, i and k
 * void *arg_s->int n: matrix dimensions
 * void *arg_s->int i: matrix row being compared to k by i'th thread
 * void *arg_s->int k: the intermediate vector used to compared distances
 */
void* doFW(void* arg_s);

// struct for storing values passed to thread (see doFW definition for variable description)
struct arg_s {
	int n;
	int i;
	int k;
};

int main() {
	// variables
	int n, m;
	// get file name
	printf("Enter number of nodes: ");
	scanf("%d", &n);
	printf("\n");

	if (n <= 10) {
		printf("Enter number of link entries: ");
		scanf("%d", &m);
		printf("\n");
	}

	// allocate dist matri
	dist = malloc(n * sizeof(int*));
	for (int i = 0; i < n; i++) {
		dist[i] = malloc(n * sizeof(int));
	}

	// make matrices
	if (n <= 10) {
		makeGraph(m);
		makeDist(n, m);
	}
	else {
		populateMatrix(n);
	}

	// initialize threads, mutex and semaphore
	pthread_t* threads = (pthread_t*)malloc(n * sizeof(pthread_t));
	pthread_mutex_init(&readLock, NULL);
	sem_init(&writeLock, 0, 1);

	// start clock
	clock_t beginning = clock();

	// loop for creating threads
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			// save arguments to be sent to threads in argument struct
			struct arg_s* arguments = malloc(sizeof(struct arg_s));
			arguments->n = n;
			arguments->i = i;
			arguments->k = k;
			// create threads running doFW up to n
			pthread_create((threads + i), NULL, doFW, (void*)(arguments));
			//free(a);
		}
		for (int i = 0; i < n; i++) {
			// join threads
			pthread_join(*(threads + i), NULL);
		}
	}

	// stop clock and print execution time
	clock_t end = clock();

	// if matrix is larger than 50 dont print
	if (n <= 50) {
		// print dist n * n matrix
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++) {
				// if dist == 999 print inf
				if (dist[i][j] == INF) {
					printf(" inf");
				}
				else {
					printf("%4d", dist[i][j]);
				}
			}
			printf("\n");
		}
	}
	else {
		printf("Graph to large to print\n");
	}

	printf("Time spent executing %f seconds\n", (double)(end - beginning) / CLOCKS_PER_SEC);

	// free allocated data
	for (int i = 0; i < n; i++) {
		free(dist[i]);
	}
	// only needed to be freed if taking user input
	if (n < 10) {
		for (int i = 0; i < m; i++) {
			free(graph[i]);
		}
	}
	free(dist);
	free(graph);
	free(threads);
	pthread_mutex_destroy(&readLock);
	sem_destroy(&writeLock);
}

void makeGraph(int m) {
	// allocate space for the input graph
	graph = malloc(m * sizeof(int*));
	for (int i = 0; i < m; i++) {
		graph[i] = malloc(3 * sizeof(int));
	}

	// read in final graph
	for (int i = 0; i < m; i++)
	{
		// take in user input for each link entry
		printf("link %d\n", i + 1);
		printf("Enter ui: ");
		scanf("%d", &graph[i][0]);
		printf("\n");

		printf("Enter vi: ");
		scanf("%d", &graph[i][1]);
		printf("\n");

		printf("Enter wi: ");
		scanf("%d", &graph[i][2]);
		printf("\n");

		// if negative weight exit with warning
		if (graph[i][0] < 0 || graph[i][1] < 0 || graph[i][2] < 0) {
			printf("Entered a negative weight. Please try again\n");
			printf("Exiting...");
			getchar();
			exit(0);
		}
	}
}

void makeDist(int n, int m) {
	// loop to fill n * n dist matrix
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++) {
			// set all values to inf unless i == j, which is set to 0
			dist[i][j] = INF;
			if (j == i) {
				dist[i][j] = 0;
			}
		}
	}
	// for each link entry store in graph[i][0] (ui) graph[i][1] (vi) the weight of that link wi
	for (int i = 0; i < m; i++) {
		dist[graph[i][0] - 1][graph[i][1] - 1] = graph[i][2];
		dist[graph[i][1] - 1][graph[i][0] - 1] = graph[i][2];
	}
}

void populateMatrix(int n) {
	// seed rand()
	srand(time(0));

	// for n * n matrix populate dist[i][j] and dist[j][i] with random nums
	for (int i = 0; i < n; i++) {
		for (int j = i; j < n; j++) {
			// if i == j dist is 0
			if (i == j) {
				dist[i][j] = 0;
			}
			else {
				// numbers are between 1 to 10
				int z = rand() % 10 + 1;
				dist[i][j] = z;
				dist[j][i] = z;
			}
		}
	}
}

void* doFW(void* args) {
	// cast arg_s to struct arg_s
	struct arg_s* arguments = (struct arg_s*)args;

	// cast k, i, n to ints
	int k = (int)arguments->k;
	int i = (int)arguments->i;
	int n = (int)arguments->n;

	for (int j = 0; j < n; j++)
	{
		// enter reader section and lock
		pthread_mutex_lock(&readLock);

		// increment global variable reader
		reader++;

		// if there are any readers lock writer secton
		if (reader == 1) {
			sem_wait(&writeLock);
		}

		// unlock read section
		pthread_mutex_unlock(&readLock);

		// read
		if ((dist[i][k] + dist[k][j]) < dist[i][j])
		{
			// wait for readLock
			pthread_mutex_lock(&readLock);

			// decrement amount of readers
			reader--;

			// if no readers left unlock writers
			if (reader == 0) {
				sem_post(&writeLock);
			}
			pthread_mutex_unlock(&readLock);
			// read end

			// lock writer critical section and write
			sem_wait(&writeLock);
			dist[i][j] = dist[i][k] + dist[k][j];
			sem_post(&writeLock);
		}
		else {
			// if (dist[i][k] + dist[k][j] is not < dist[i][j]) the
			//  reader still needs to be decremented
			pthread_mutex_lock(&readLock);

			// decrement amount of readers
			reader--;

			// if no readers left unlock writers
			if (reader == 0) {
				sem_post(&writeLock);
			}
			pthread_mutex_unlock(&readLock);
		}
	}
	free(arguments);
}