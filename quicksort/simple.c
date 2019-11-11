#include "constraints.h"

int main(void)
{
    int N;
    scanf("%d",&N);
    int Array[N];
    for(int n=0;n<N;n++)
    {
        scanf("%d",&Array[n]);
    }
    int med = kthSmallest(Array,0,N-1,N/2);
    for(int n=0;n<N;n++)
    {
        printf("%d ",Array[n]);
    }
    printf("\n");
}
