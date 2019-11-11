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

void sort_quickly(int A[], int left, int right) 
{ 
   if(left >= right){ return ;}
    if (left < right) 
    { 
        int n = right-left+1; 
        int med = find_index_at_k(A, left, right, n/2); 
        int pos = partition(A, left, right, med); 
        sort_quickly(A, left, pos - 1); 
        sort_quickly(A, pos + 1, right); 
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
    clock_t start = clock();
    sort_quickly(A,0,N-1);
    clock_t end = clock()-start; 
    for(int i = 0; i < N; i++)
    {
        printf("%d ",A[i]);
    }
    printf("\n");
    bar();
    printf("Took this time %f\n",((double)end-start)/CLOCKS_PER_SEC);
    bar();
    printf("\n");
}
