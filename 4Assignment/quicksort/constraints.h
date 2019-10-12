#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef int ll;

void swap(ll *a,ll *b)
{
    ll temp = *a;
    *a = *b;
    *b = temp;
}

void insertion_sort(ll arr[],ll size)
{
    ll i,key,j;
    for( i = 1; i < size; i++)
    {
        key = arr[i];
        j = i-1;
        while(j>=0 && arr[j]>key)
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

ll partition(ll arr[], ll l, ll r, ll x) 
{ 
    ll i = l; 
    for (i=l; i<r; i++) 
        if (arr[i] == x) 
           break; 
    swap(&arr[i], &arr[r]); 
  
    i = l; 
    for (ll j = l; j <= r - 1; j++) 
    { 
        if (arr[j] <= x) 
        { 
            swap(&arr[i], &arr[j]); 
            i++; 
        } 
    } 
    swap(&arr[i], &arr[r]); 
    return i; 
} 

ll small_median(ll arr[],ll size)
{
    printf("called\n");
    for(int m = 0; m < size; m++)
    {
        printf("%d ",arr[m]);
    }
    printf("\n");
    insertion_sort(arr,size);
    printf("resuled\n");
    for(int m = 0; m < size; m++)
    {
        printf("%d ",arr[m]);
    }
    printf("\n");
    return arr[size/2];
}

ll kth_smallest(ll arr[],ll left,ll right,ll k)
{
    ll size = right-left+1;
    if(k <= 0 || k > size)
    {
        return -1e7;
    }
    ll median[(size)/5+!(!(size%5))];
    printf("Hi %d %d %d\n",left,right,k);
    ll ind = 0;
    for(int i = 0; i < size/5; i++)
    {
        median[ind++] = small_median(arr+left+i*5,5);
    }
    if(size%5)
    {
        median[ind++] = small_median(arr+left+size-(size%5),size%5);
    }
    ll median_of_medians ;
    if(size < 10)
    {
        median_of_medians = median[0];
    }
    else
    {
        median_of_medians = kth_smallest(median,0,ind-1,ind/2);
    }
    ll position = partition(arr,left,right,median_of_medians);

    printf("called %d %d %d\n",left,right,k);
    for(int i = 0; i < size; i++)
    {
        printf("%d ",arr[left+i]);
    }
    printf("\n");
    if(position==(size/2))
    {
        return arr[position];
    }
    else if(position-left > k-1)
    {
        return kth_smallest(arr,left,position-1,k);
    }
    else
    {
        return kth_smallest(arr,position+1,right,k-position+left-1);
    }
}
