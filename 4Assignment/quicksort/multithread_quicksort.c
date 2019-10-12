#include "constraints.h"

void quickSort(int arr[], int l, int h) 
{ 
    if (l < h) 
    { 
        int n = h-l+1; 
        int med = kthSmallest(arr, l, h, n/2);  
        int p = partition(arr, l, h, med); 
  
        quickSort(arr, l, p - 1); 
        quickSort(arr, p + 1, h); 
    } 
} 
 
void * quicksort(void *arg)
{
    ll thread_num = *(ll *)arg;
    // sort from thread_num to thread_num+1
    if(thread_num==(3))
    {
        // sort from median to last
        quickSort(AArray,medians_array[thread_num],pura);
    }
    else
    {
        quickSort(AArray,medians_array[thread_num],medians_array[thread_num+1]);
    }
}

int main(int argc,char argv[])
{
    ll N ;
    scanf("%d",&N);
    pura = N-1;
    for (ll n = 0; n < N; n++)
    {
        scanf("%d",&AArray[n]);
    }
    ll med = kthSmallest(AArray,0,N-1,(N-1)/2);
    medians_array[1] = partition(AArray,0,N-1,med);

    ll max = kthSmallest(AArray,0,medians_array[1]-1,(medians_array[1]-1)/2);

    medians_array[0] = kthSmallest(AArray,0,medians_array[1]-1,max);

    ll max2 = kthSmallest(AArray,medians_array[1]+1,N-1,(N-1-medians_array[1]-1+1)/2);
    medians_array[2] = kthSmallest(AArray,medians_array[1]+1,N-1,max2);
    
    for (ll n = 0; n < MAX_THREADS; n++)
    {
        pthread_create(&threads[n],NULL,quicksort,(void *)&n);
    }
    
    for (ll n = 0; n < MAX_THREADS; n++)
    {
        pthread_join(threads[n],NULL);
    }
    
    return 0;
}