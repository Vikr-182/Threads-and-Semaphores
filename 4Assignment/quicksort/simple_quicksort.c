#include "constraints.h"

// void quicksort(ll a[],ll left,ll right)
// {
//     if(left>=right)
//     {
//         return;
//     }
//     ll size = (right-left+1);
//     ll i;
    
//     // find the median in O(n) time
//     ll med = kth_smallest(a,left,right,size/2);

//     // partition the array around the median
//     ll ind = partition(a,left,size,med);
//     printf("%d\n",ind);
//     quicksort(a,left,ind-1);
//     quicksort(a,ind+1,right);
//     return;
// }

void quickSort(int arr[], int l, int h) 
{ 
    if (l < h) 
    { 
        // Find size of current subarray 
        int n = h-l+1; 
  
        // Find median of arr[]. 
        int med = kthSmallest(arr, l, h, n/2); 
  
        // Partition the array around median 
        int p = partition(arr, l, h, med); 
  
        // Recur for left and right of partition 
        quickSort(arr, l, p - 1); 
        quickSort(arr, p + 1, h); 
    } 
} 
 

void main(int argc,char argv[])
{
    ll N;
    scanf("%d",&N);
    ll A[N];
    for (ll i = 0; i < N; i++)
    {
        scanf("%d",&A[i]);   
    }
    quickSort(A,0,N-1);
    for(int i = 0; i < N; i++)
    {
        printf("%d ",A[i]);
    }
    printf("\n");
}