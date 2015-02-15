#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>
int find_min(int * array,int size)
{
	int c=0;
	int minimum=array[c];
	int location=0;
	for ( c = 1 ; c < size ; c++ )
	{
		if ( array[c] < minimum )
		{
			minimum = array[c];
			location = c;
		}
	}
	return location;
}
void swap (int v[], int i, int j) {
	int	t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}

void perm (int v[], int n, int i,int * scores,int **graph) {
	 static int count=0;
	int	j;
	if (i == n) {
		for (j=0; j<n; j++){
            //printf ("%d-", v[j]);
            if(j==n-1)
            scores[count]=scores[count]+graph[v[j]][0];
            else
            scores[count]=scores[count]+graph[v[j]][v[j+1]];
        }
		//printf ("%d\n",scores[count]);
		count++;
	} else
		for (j=i; j<n; j++) {
			swap (v, i, j);
			perm (v, n, i+1,scores,graph);
			swap (v, i, j);
		}
}

int main(argc,argv)
int argc;
char *argv[];
{
	int numprocs,myid;
	double sTime,eTime,pTime;
	FILE *ifp;
    ifp = fopen("matrics.txt", "r");
    if (ifp == NULL) {
      fprintf(stderr, "Can't open input file in.list!\n");
      exit(1);
    }
    int size;
    int i=0;
    int j=0;
    fscanf(ifp, "%d", &size);

    int** matrics;

    matrics = malloc(size* sizeof(int*));
    for (i = 0; i < size; i++) {
        matrics[i] = malloc(size* sizeof(int));
    }
    for(i=0;i<size;i++)
        for(j=0;j<size;j++)
            fscanf(ifp,"%d",&matrics[i][j]);

    int values=1;
    for(i=size-1;i>0;i--)
        values *= i;
        
        MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    sTime=MPI_Wtime();

    int *v;
    v=malloc(size * sizeof(int*));
    for (i=0; i<size; i++) v[i] = i;
    for (i=0; i<4; i++) v[i] = i;
    int * scores;
    scores=malloc(values* sizeof(int*));
    for(i=0;i<values;i++){
        scores[i]=0;
    }
    printf("%d\n",values);
    for(i=0;i<size-1;i++){
        swap(v,1,i+1);
        perm (v, size, 2,scores,matrics);
    }
    int location=0;
    location=find_min(scores,values);
    printf("Min value : %d\n",scores[location]);
    
    eTime=MPI_Wtime();
    pTime= fabs(eTime-sTime);
    printf("Time Spend %5.6e\n",pTime);
    MPI_Finalize();
    return 0;
}
