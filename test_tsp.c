#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>


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
void perm (int v[], int n, int i,int scores[],int graph[][4],int myid) {

  /* this function generates the permutations of the array
   * from element i to element n-1
   */
   static int count=0;
  int j;

  /* if we are at the end of the array, we have one permutation
   * we can use (here we print it; you could as easily hand the
   * array off to some other function that uses it for something
   */
  if (i == n) {
    for (j=0; j<n-1; j++){
                printf ("%d,%d |", v[j],v[j+1]);
                scores[count+(myid *6)]=scores[count+(myid *6)]+graph[v[j]][v[j+1]];
          }


    printf ("(%d, %d)\n",scores[count+(myid *6)],myid);
    count++;
  } else
    /* recursively explore the permutations starting
     * at index i going through index n-1
     */
    for (j=i; j<n; j++) {

      /* try the array with i and j switched */

      swap (v, i, j);
      //printf("I was here");
      perm (v, n, i+1,scores,graph,myid);

      /* swap them back the way they were */

      swap (v, i, j);
    }
}

/* little driver function to print perms of first 5 integers */
 
/************************************************************
This is a simple send/receive program in MPI
************************************************************/
struct astruct{ 
    int value; 
    int   index; 
} ; 




int main(argc,argv)
int argc;
char *argv[];
{
    clock_t begin, end;
    double time_spent;
    struct astruct in[1];
    struct astruct out[1];
    int myid, numprocs;
    int tag,source,destination,count;
    int buffer=0;
    MPI_Status status;
 
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    tag=1234;
    source=0;
    destination=1;
    count=1;
    int v[4], i,j;
    int minimum;
    int allmin;
    int location=0;
    int mini[2]={65,65};
    int allmini[2]={65,65};
    int allLocation[4];
    int graph[4][4]={
        {0,2,9,2},
        {3,0,5,3},
        {2,4,0,2},
        {5,6,5,0}
    };
    for (i=0; i<4; i++) v[i] = i;
    int scores[24];
    for(i=0;i<24;i++){
        scores[i]=0;
  
    }


    if(myid == source){
      
      mini[0]=15;
      mini[1]=myid;
      begin = clock();
      scores[1]++;
      printf("Enter a number:");
      scanf("%d",&buffer);
      for(i = 1 ; i < numprocs ; i++){
          MPI_Send(&scores[(i-1)*6],6,MPI_INT,i,tag+1,MPI_COMM_WORLD);
          MPI_Send(&buffer,count,MPI_INT,i,tag,MPI_COMM_WORLD);
      }
      printf("processor %d  sent %d\n",myid,buffer);
      for(i = 1 ; i < numprocs ; i++){
  MPI_Recv(&scores[(i-1)*6],6,MPI_INT,i,tag+2,MPI_COMM_WORLD,&status);
      //printf("Value: %d and location %d\n",allmin[0],allmin[1]);
      //MPI_Barrier(MPI_COMM_WORLD);
  }
      MPI_Barrier(MPI_COMM_WORLD);
      MPI_Reduce( &minimum, &allmin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD );
      printf("Value : %d, and location %d\n",allmin, myid);
      //for(i=1;i<numprocs;i++)
  //MPI_Recv(&location,1,MPI_INT, 4,tag+3,MPI_COMM_WORLD, &status);
        
    }
    if(myid != 0){
        MPI_Recv(&scores[(myid-1) * 6],6,MPI_INT,source,tag+1,MPI_COMM_WORLD,&status);
        MPI_Recv(&buffer,count,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
        swap(v,0,myid-1);
        perm (v, buffer, 1,scores,graph,myid-1);
        location=find_min(&scores[(myid-1)*6],6);
  in[0].value = scores[((myid - 1 )*6) + location  ];
  in[0].index = location ;
  minimum = scores[((myid - 1 )*6) + location  ];
  mini[0] = scores[((myid - 1 ) * 6 ) + location];
        mini[1] =( (myid - 1 ) * 6) +  location;
  printf("Value : %d and my rank is %d\n",minimum,myid);
  
  MPI_Send(&scores[(myid-1)*6],6,MPI_INT,0,tag+2,MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Reduce( &minimum, &allmin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD );
  
  //MPI_Allreduce(minimum, allmin, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
//  if(myid ==1)
//  printf("WOrld");
//  MPI_Send(&location, 1 , MPI_INT, 0 , tag +3 , MPI_COMM_WORLD);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);      
  //  if(myid == 1)
    MPI_Allreduce(mini,allmini,1 , MPI_2INT,MPI_MINLOC,MPI_COMM_WORLD);
    if(myid != source)
      printf("Value : %d , the process: %d\n" , allmini[0],allmini[1]);
        


    if(myid == source)
    {
        for(i=0;i<24;i++){
            printf("%d,",scores[i]);
        }
  printf("\n");
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("%f\n",time_spent);
    }
    MPI_Finalize();
    return 0;
}
