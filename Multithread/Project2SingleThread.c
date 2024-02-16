#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#define INF 999;
int **graph;
int **dist;

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
 */
void populateMatrix(int n);

/*
 * doFW
 *
 * Floyd-Warshall algorithm
 *
 * int n: size of matrix
 */
void doFW(int n);

int main() {
	// variables
	int n, m;

	// get number of nodes
	printf("Enter number of nodes: ");
	scanf("%d", &n);
	printf("\n");

	// if n is 10 or less take second user input for m
	if (n <= 10) {
		printf("Enter number of link entries: ");
        	scanf("%d", &m);
        	printf("\n");
	}

	// allocate dist matrix
        dist = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) {
                dist[i] = malloc(n * sizeof(int));
        }	

	// make matrices
	if (n <= 10) {
		makeGraph(m);
		makeDist(n, m);
	} else {
		populateMatrix(n);
	}

	// start clock
	clock_t beginning = clock();

	// call FW algorithm worker
	doFW(n);
	// stop clock
	clock_t end = clock();

	// print final dist matrix if less than 50 by 50
	if (n <= 50) {
	for (int i = 0; i < n; i++)
        {
                for(int j = 0; j < n; j++) {
			// if dist == 999 print inf
                        if (dist[i][j] == 999) {
				printf(" inf");
			} else {
				printf("%4d", dist[i][j]);
			}
                }
		printf("\n");
        }
	} else {
		printf("Graph to large to print\n");
	}

	// print time spent
	printf("Time spent executing %f seconds\n", (double)(end - beginning) / CLOCKS_PER_SEC);

	// free previously allocated memory
	for (int i=0; i < n; i++) {
		free(dist[i]);
	}
	if (n < 10) {
		for (int i=0; i < m; i++) {
			free(graph[i]);
		}
	}
	free(dist);
	free(graph);
}


void makeGraph(int m)  {
	// allocate space for the input graph
        graph = malloc(m * sizeof(int *));
        for (int i = 0; i < m; i++) {
                graph[i] = malloc(3 * sizeof(int));
        }

	// read in final graph
        for (int i = 0; i < m; i++)
        {
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
                if (graph[i][0] < 0, graph[i][1] < 0, graph[i][2] < 0){
			printf("Entered a negative weight. Please try again\n");
			printf("Exiting...");
			exit(0);
		}
	}
}

void makeDist(int n, int m) {
	// loop for n by n matrix 
	for (int i = 0; i < n; i++)
        {
                for(int j = 0; j < n; j++) {
			// initilize all dist to inf
                        dist[i][j] = INF;
			// if j==i dist must be 0
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
		for(int j = i; j < n; j++) {
			// if i == j dist is 0
			if (i == j) {
				dist[i][j] = 0;
			} else {
				// numbers are between 1 to 10
				int z = rand() % 10 + 1;
				dist[i][j] = z;
				dist[j][i] = z;
			}
		}
	}
}


void doFW(int n) {
	// Floyd-Warshall algorithm supplied in project description
        for(int k = 0; k < n; k++){
                for(int i = 0; i < n; i++) {
                        for(int j = 0; j < n; j++) {
                                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                                        dist[i][j] = dist[i][k] + dist[k][j];
                                }
                        }
                }
        }
}

