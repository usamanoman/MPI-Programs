#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <string.h>

/* function to swap array elements */

void swap (int v[], int i, int j) {
	int t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}

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

/* recursive function to generate permutations */
void perm (int v[], int n, int i,int scores[],int** graph,int myid,char * result[],int values) {
	static int count=0;
	int j;
	char buff[3];
	if (i == n) {
		char *res;
		res = (char *)malloc(sizeof(buff)+(n+1)+(n));
		strcpy(res,"");
		for (j=0; j<n; j++){
			snprintf(buff,sizeof(buff),"%d-",v[j]);
			strcat(res,buff);
			if(j==n-1)
			scores[count+(myid*values)]=scores[count+(myid*values)]+graph[v[j]][0];
			else
			scores[count+(myid *values)]=scores[count+(myid *values)]+graph[v[j]][v[j+1]];
		}
		res[strlen(res)]=res[0];
		result[count]=res;
		//printf("%s -- (%d)\n",result[count],scores[count+(myid *values)]);
		count++;
	} else
		for (j=i; j<n; j++) {
			swap (v, i, j);
			perm (v, n, i+1,scores,graph,myid,result,values);
			swap (v, i, j);
		}
}

int main(argc,argv)
int argc;
char *argv[];
{
    double eTime,sTime,pTime;
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

	double time_spent;
	int myid, numprocs;
	int tag,source,destination,count;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	tag=1234;
	source=0;
	destination=1;
	count=1;
	int *v;
    sTime=MPI_Wtime();

	v=malloc(numprocs * sizeof(int*));
	int location=0;
	int mini_with_rank[2]={100,100};
	int allmini_with_rank[2]={100,100};
	int mini_with_loc[2]={100,100};
	int allmini_with_loc[2]={100,100};

	
	for (i=0; i<numprocs; i++) v[i] = i;
	int *scores;
	scores = malloc(values* sizeof(int*));

	char **result;
	result=malloc((values/(numprocs-1))* sizeof(char*));

	if(myid == source){
		printf("The graph is:\n");
		for(i=0;i<size;i++){
	        	for(j=0;j<size;j++)
	            		printf("%d ",matrics[i][j]);
	        	printf("\n");
	    	}
		for(i = 1 ; i < numprocs ; i++){
			MPI_Recv(&scores[(i-1)*(values/(numprocs-1))],(values/(numprocs-1)),MPI_INT,i,tag+2,MPI_COMM_WORLD,&status);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	if(myid != 0){
		for(i=0;i<(values/(numprocs-1));i++){
				scores[(myid-1)*(values/(numprocs-1))+i]=0;	
		}
		swap(v,1,myid);
		perm (v, size, 2,scores,matrics,myid-1,result,(values/(numprocs-1)));
		location=find_min(&scores[(myid-1)*(values/(numprocs-1))],(values/(numprocs-1)));
		mini_with_loc[0] = scores[((myid - 1 ) * (values/(numprocs-1)) ) + location];
		mini_with_loc[1] =location;
		mini_with_rank[0] = scores[((myid - 1 ) * (values/(numprocs-1)) ) + location];
		mini_with_rank[1] =myid;
		MPI_Send(&scores[(myid-1)*(values/(numprocs-1))],(values/(numprocs-1)),MPI_INT,0,tag+2,MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);		
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Allreduce(mini_with_rank,allmini_with_rank,1 , MPI_2INT,MPI_MINLOC,MPI_COMM_WORLD);
	MPI_Allreduce(mini_with_loc,allmini_with_loc,1 , MPI_2INT,MPI_MINLOC,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);	
	if(myid == allmini_with_rank[1]){
		//MPI_Send(result[allmini_with_loc[1]],strlen(result[allmini_with_loc[1]])+1,MPI_CHAR,0,tag+7,MPI_COMM_WORLD);	
		eTime=MPI_Wtime();
		pTime=fabs(eTime-sTime);
		printf("My rank is : %d, I have found a minimum path:%s with value:%d\n\nEstimated time take in processing was : %5.6e\n",allmini_with_rank[1],result[allmini_with_loc[1]],allmini_with_loc[0],pTime);
	}
	MPI_Barrier(MPI_COMM_WORLD);	

	MPI_Finalize();
	return 0;
}