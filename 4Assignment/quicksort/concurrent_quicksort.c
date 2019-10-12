#include "constraints.h"

void quicksort(ll a[],ll left,ll right)
{
    ll size = (right-left+1);
    ll i;
    if(size<=5)
    {
        insertion_sort(a+left,size);
        return ;
    }

    // find the median in O(n) time
    ll med = kth_smallest(a,left,right,size/2);

    // partition the array around the median
    ll ind = partition(a,left,size,med);

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
            quicksort(a,left+size/2,right);
            exit(1);
        }
    }
}

int main(int argc,char argv[])
{
    ll N ;
    scanf("%d",&N);
    key_t key = IPC_PRIVATE;
    ll *Array;
    size_t size_shm = sizeof(int)*N;
    int shmid =  shmget(key,size_shm,IPC_CREAT|0666);
    if(shmid<0)
    {
        perror("shm id error\n");
        exit(0);
    }
    
    if((Array = shmat(shmid,NULL,0)) == NULL)
    {
        perror("Shm error\n");
        exit(0);
    }
    for (ll n = 0; n < N; n++)
    {
        scanf("%d",&Array[n]);
    }
    quicksort(Array,0,N-1);
    for(int i = 0; i < N; i++)
    {
        printf("%d ",Array[i]);
    }
    printf("\n");

    return 0;
}