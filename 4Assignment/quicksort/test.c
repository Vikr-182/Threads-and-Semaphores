#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
typedef long long ll;

ll MAX = 100000000000;                      //  1e11
ll MIN = -100000000000;                     // -1e11
ll MOD = 1000000007;

ll longlongmax = __LONG_LONG_MAX__;

ll maxormin(ll a,ll b,ll flag){
    return flag==1?(a>b?a:b):(a<b?a:b);
}

ll overflowcheck(ll a){
    if(a==1){
        return 0;
    }
    else
    {
        ll temp = a;
        ll power = 1;
        while( longlongmax/temp>a ){
            temp *= a;
            power++;
        }
        return power+1;                     // Overflow occurs at this power.
    }
    
}

ll merge(ll A[],ll B[],ll left,ll mid,ll right){
    ll count = 0;
    ll n1 = mid-left+1; 
    ll n2 = right-mid; 

    ll LA[n1],LB[n1],RA[n1],RB[n2];

    for(ll n=0;n<n1;n++){ LA[n] = A[n+left];LB[n] = B[n+left];}
    for(ll n=0;n<n2;n++){ RA[n] = A[n+mid+1];RB[n] = B[n+mid+1];}
    
    ll i=0,j=0,k=left;

    while(i<n1&&j<n2){
        if(LA[i]<RA[j]){
            A[k] = LA[i];
            B[k] = LB[i];
            i++;
        }
        else{
            A[k] = RA[j];
            B[k] = RB[j];
            count += n1-i;
            j++;
        }
        k++;
    }
    while(i<n1){    
            A[k] = LA[i];
            B[k] = LB[i];
            i++;
            k++;
    }

    while(j<n2){
            A[k] = RA[j];
            B[k] = RB[j];
            j++;
            k++;    
    }
    return count;
}

ll mergesort(ll A[],ll B[],ll left,ll right){
    ll total = 0;
    if(left<right){
        ll mid = (right-left)/2+left;
        total += mergesort(A,B,left,mid);
        total += mergesort(A,B,mid+1,right);
        total += merge(A,B,left,mid,right);
    }
    return total;
}

ll get_random(ll limit,ll flag)
{
    ll a = rand()%limit;
    if(a < 0)
    {
        a *= -1;
    }
    a += (!flag & !a) * limit/2;
    return a;
}

int main(int argc, char const *argv[])
{
    srand(time(0));
    ll T = 1;
    for(ll t = 0; t < T; t++)
    {
        ll N = get_random(10,0);
        printf("%lld\n",N);
        ll A[N];
        for(ll n = 0; n < N; n++)
        {
            A[n] = get_random(100,1);
            printf("%lld ",A[n]);
        }
        printf("\n");
    }
        
    return 0;
}
