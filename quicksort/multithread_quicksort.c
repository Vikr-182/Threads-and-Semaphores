#include "constraints.h"

void sort_quickly(int Array[], int l, int h) 
{ 
    if (l < h) 
    { 
        int n = h-l+1; 
        int med = find_index_at_k(Array, l, h, n/2);  
        int p = partition(Array, l, h, med); 
  
        sort_quickly(Array, l, p - 1); 
        sort_quickly(Array, p + 1, h); 
    } 
} 
 
void * quicksort(void *arg)
{
    ll thread_num = *(ll *)arg;
    // sort from thread_num to thread_num+1
    if(thread_num==(3))
    {
        // sort from median to last
        sort_quickly(AArray,medians_array[thread_num],pura);
    }
    else
    {
        sort_quickly(AArray,medians_array[thread_num],medians_array[thread_num+1]);
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
    clock_t start = clock();
    ll med = find_index_at_k(AArray,0,N-1,(N)/2);
    medians_array[1] = partition(AArray,0,N-1,med);
    ll max = find_index_at_k(AArray,0,medians_array[1]-1,(medians_array[1])/2);
    medians_array[0] = partition(AArray,0,medians_array[1]-1,max);
    ll max2 = find_index_at_k(AArray,medians_array[1]+1,N-1,(N-1-medians_array[1]-1+1)/2);
    medians_array[2] = partition(AArray,medians_array[1]+1,N-1,max2);
    for (ll n = 0; n < MAX_THREADS; n++)
    {
        pthread_create(&threads[n],NULL,quicksort,(void *)&n);
    }
    
    for (ll n = 0; n < MAX_THREADS; n++)
    {
        pthread_join(threads[n],NULL);
    }
    clock_t end = clock()-start;
    display(AArray,0,N-1);
    bar();
    printf("Time taken mult:\t %f\n",((double)(end))/CLOCKS_PER_SEC);
    bar();
    return 0;
}
