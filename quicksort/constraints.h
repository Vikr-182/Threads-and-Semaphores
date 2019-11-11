#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <limits.h>
#include <sys/shm.h>

#define MAX_THREADS 4
pthread_t threads[MAX_THREADS];

typedef int ll;

ll medians_array[3];
ll AArray[2000000];
ll pura;
  

void bar()
{
    for (ll i = 0; i < 20; i++)
    {
        printf("-");
    }
    printf("\n");
}

void display(ll arr[],ll left,ll r)
{
    for (ll l = left; l <= r; l++)
    {
        printf("%d ",arr[l]);
    }
    printf("\n");
}

void swap(ll *a,ll *b)
{
    ll temp = *a;
    *a = *b;
    *b = temp;
}

void insertion_sort(ll Array[],ll size)
{
    ll i,key,j;
    for( i = 1; i < size; i++)
    {
        key = Array[i];
        j = i-1;
        while(j>=0 && Array[j]>key)
        {
            Array[j+1] = Array[j];
            j--;
        }
        Array[j+1] = key;
    }
}

ll partition(ll Array[], ll l, ll r, ll x) 
{ 
    ll i = l; 
    for (i=l; i<r; i++) 
        if (Array[i] == x) 
           break; 
    swap(&Array[i], &Array[r]); 
  
    i = l; 
    for (ll j = l; j <= r - 1; j++) 
    { 
        if (Array[j] <= x) 
        { 
            swap(&Array[i], &Array[j]); 
            i++; 
        } 
    } 
    swap(&Array[i], &Array[r]); 
    return i; 
} 

ll small_median(ll Array[],ll size)
{
    insertion_sort(Array,size);
    return Array[size/2];
}

ll find_index_at_k(ll Array[],ll left,ll right,ll k)
{
    ll size = right-left+1;
    if(k <= 0 || k > size)
    {
        return -1e7;
    }
    ll median[(size)/5+!(!(size%5))];
    ll ind = 0;
    for(int i = 0; i < size/5; i++)
    {
        median[ind++] = small_median(Array+left+i*5,5);
    }
    if(size%5)
    {
        median[ind++] = small_median(Array+left+size-(size%5),size%5);
    }
    ll median_of_medians ;
    if(size < 10)
    {
        median_of_medians = median[0];
    }
    else
    {
        median_of_medians = find_index_at_k(median,0,ind-1,ind/2);
    }
    ll position = partition(Array,left,right,median_of_medians);

    if((position-left)==(k-1))
    {
        return Array[position];
    }
    else if(position-left > k-1)
    {
        return find_index_at_k(Array,left,position-1,k);
    }
    else
    {
        return find_index_at_k(Array,position+1,right,k-position+left-1);
    }
}

