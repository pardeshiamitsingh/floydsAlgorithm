
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


void floyd(int p, int n, int local_mat[], int my_rank);
void printMatrix(int mat[], int n);

int main(int argc, char* argv[])
{
	int no_of_process, rank, no_of_vertices;
	double start, end;
	//no of verticies in directed graph
	no_of_vertices = 6;
	int* matrix = NULL;
	int* local_matrix;
	int* temp_matrix;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &no_of_process);

	printf("process no :%d\n", rank);
	if (rank == 0)

	if (rank == 0)
	{
		matrix = malloc(no_of_vertices * no_of_vertices * sizeof(int)); //the adjacency matrix of digraph


		int matrix_def[6][6] = {
			{ 0,	2,	5,	10000,	10000,	10000 } ,   /*  initializers for row indexed by 0 */
			{ 10000,	0,	7,	1,		10000,	8 } ,   /*  initializers for row indexed by 1 */
			{ 10000,	10000,	0,	4,		10000,	10000 } ,   /*  initializers for row indexed by 2 */
			{ 10000,	10000,	10000,	0,		3,	10000 } ,   /*  initializers for row indexed by 3 */
			{ 10000,	10000,	2,	10000,		0,	3 } ,   /*  initializers for row indexed by 4 */
			{ 10000,	5,	10000,	2,		4,	0 } ,   /*  initializers for row indexed by 5 */
		};

		matrix = &matrix_def;

	}

	// Buffer allocations 
	local_matrix = malloc(no_of_vertices * (no_of_vertices / no_of_process) * sizeof(int));
	temp_matrix = malloc(no_of_vertices * no_of_vertices * sizeof(int));

	// Distributes matrix among the processors 
	MPI_Scatter(matrix, no_of_vertices * (no_of_vertices / no_of_process), MPI_INT, local_matrix, no_of_vertices * (no_of_vertices / no_of_process), MPI_INT, 0, MPI_COMM_WORLD);
	start = MPI_Wtime();// log the start time
						//Call Floyd's algorithm find the cost

						//Call floyds
	floyd(no_of_process, no_of_vertices, local_matrix, rank);
	end = MPI_Wtime();
	//Gathers data 
	MPI_Gather(local_matrix, no_of_vertices * (no_of_vertices / no_of_process), MPI_INT, temp_matrix, no_of_vertices * (no_of_vertices / no_of_process), MPI_INT, 0, MPI_COMM_WORLD);

	//To Print the cost
	if (rank == 0)
	{
		printf("\nShortest-Paths matrix for the graph\n", end - start);
		printf("\n");
		printMatrix(temp_matrix, no_of_vertices);
		printf("\n");
	}
	MPI_Finalize();
	return(0);
}




// Floyd's Algorithm
void floyd(int no_of_process, int no_of_vertices, int local_matrix[], int rank)
{
	int* tmp; // to hold the row to braodcast
	int local_index; //index local of the broadcast row
	int root; 

	tmp = malloc(no_of_vertices * sizeof(int));

	int k;
	for (k = 0; k < no_of_vertices; k++) {
		//determine the owner of kth row
		root = k / (no_of_vertices / no_of_process);
		//send it to all 
		if (rank == root) {
			//it is k - first row number, which is floor (n/p)
			local_index = k % (no_of_vertices / no_of_process);
			int j;
			//copy it to temp array to broadcast
			//actual row will be updated
			for (j = 0; j < no_of_vertices; j++)
				tmp[j] = local_matrix[local_index * no_of_vertices + j];
		}
		//Broadcasts tmp from root to all other processes
		MPI_Bcast(tmp, no_of_vertices, MPI_INT, root, MPI_COMM_WORLD);
		int local;
		for (local = 0; local < no_of_vertices / no_of_process; local++)
		{
			int i;
			for (i = 0; i < no_of_vertices; i++)
			{
				local_matrix[local * no_of_vertices + i] =
					min(local_matrix[local * no_of_vertices + i], local_matrix[local*no_of_vertices + k] + tmp[i]);
			}
		}
	}
	free(tmp);
}


void printMatrix(int mat[], int n)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (mat[i * n + j] > 998)
				printf("∞ ");
			else
				printf("%d ", mat[i * n + j]);
		}
		printf("\n");
	}
}

