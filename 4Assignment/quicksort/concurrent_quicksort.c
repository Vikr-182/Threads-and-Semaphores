#include "constraints.h"

void quicksort(ll a[],ll left,ll right)
{
    if(left>=right)
    {
        return ;
    }
    ll size = (right-left+1);
    ll i;

    
    if(size<=5)
    {
        insertion_sort(a+left,size);
        return ;
    }

    // find the median in O(n) time
    ll med = kthSmallest(a,left,right,size/2);
    
    // partition the array around the median
    ll ind = partition(a,left,right,med);
    
    pid_t child_left,child_right;
    child_left = fork();
    if(child_left<0)
    {
        perror("Error forking\n");
        exit(0);
    } 
    else if(!child_left)
    {
        quicksort(a,left,ind-1);
        exit(1);
    }
    else
    {
        child_right = fork();
        if(child_right<0)
        {
            perror("Forking errr");
            exit(0);
        }
        else if(!child_right)
        {
            quicksort(a,ind+1,right);
            exit(1);
        }
        int status;
        waitpid(child_left,&status,0);
        waitpid(child_right,&status,0);
    }
}

int main(int argc,char argv[])
{
    ll N ;
    scanf("%d",&N);
    key_t key = IPC_PRIVATE;
    ll *Array;
    ll *shm_array;
    size_t size_shm = sizeof(int)*N;
    int shmid;
    if((shmid =  shmget(key,size_shm,IPC_CREAT|0666))==-1)
    {
        perror("shm id error\n");
        exit(0);
    }
    
    if((shm_array = shmat(shmid,NULL,0)) == (int *)-1)
    {
        perror("Shm error\n");
        exit(0);
    }
    for (ll n = 0; n < N; n++)
    {
        scanf("%d",&Array[n]);
        shm_array[n] = Array[n];
    }
    clock_t start = clock();
    // printf("Starting the sorting\n");
    bar();
    quicksort(shm_array,0,N-1);
    for(int i = 0; i < N; i++)
    {
        printf("%d ",shm_array[i]);
    }
    printf("\n");
    printf("Time taken:\t %f\n",(double)(clock()-start)/(CLOCKS_PER_SEC));
    bar();
    // printf("Finished sorting\n");
    
    return 0;
}