#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef long long ll;

ll get_random(ll limit,ll flag)
{
    int a = rand()%limit;
    if(a<0) a+= limit/2;
    a += (!flag & !a)*(limit/2);
    return a;  
}


int main(void)
{
    srand(time(0));
    ll N = get_random(2000,0);
    printf("%lld\n",N);
    for(ll n=0;n<N;n++)
    {
        ll a = get_random(2000,1);
        printf("%lld ",a);
    }
    printf("\n");
}
