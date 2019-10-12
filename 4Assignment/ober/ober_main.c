#include "global.h"

int myCond(int request, int pass_num)
{
    if (request)
    {
        // of type available available_cab_ride_pool
        ll fg = 0;
        ll ind = 0;
        for (ll n = 0; !fg && n < N; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]);
            if (cabss[n]->cab_type == 1 && cabss[n]->status!=-1)
            {
                // ride pool
                fg = 1;
                ind = n;
                if (cabss[ind]->num_pass[0] == -1) 
                {
                    cabss[ind]->num_pass[0] = pass_num;
                }
                else
                {
                    cabss[ind]->num_pass[1] = pass_num;
                    cabss[n]->status = -1;
                }
                printf("Passenger %d boarded %d cab ok aah\n", pass_num, ind);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]);
        }
        for (ll n = 0; !fg && n < N; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]);
            if (cabss[n]->cab_type == 0 && cabss[n]->status!=-1)
            {
                // empty
                fg = 1;
                ind = n;
                if (cabss[ind]->num_pass[0] == -1) 
                {
                    cabss[ind]->num_pass[0] = pass_num;
                }
                else
                {
                    cabss[ind]->num_pass[1] = pass_num;
                }
                printf("Passenger %d boarded %d cab ok ooh\n", pass_num, ind);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]);
        }
        return !fg;
    }
    else
    {
        ll fg = 0;
        ll ind = 0;
        for (ll n = 0; n < N && !fg ; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]);
            if (cabss[n]->cab_type == 0 && cabss[n]->status!=-1)
            {
                // empty ride
                fg = 1;
                ind = n;
                if (cabss[ind]->num_pass[0] == -1)
                {
                    cabss[ind]->num_pass[0] = pass_num;
                    cabss[n]->status = -1;
                }
                printf("Passenger %d boarded %d cab ok iih\n", pass_num    , ind);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]);
        }
        return !fg;
    }
}

void *function_pass(void *arg)
{
    // deal with passenger numbered 'n'
    int num = *(int *)arg;
    int wait_time = passengers[num]->max_wait_time;
    int arrival_time = passengers[num]->arrival_time;
    int ride_time = passengers[num]->ride_time;
    clock_t start = clock();
    while (clock() - start < arrival_time)
        ; // wait for it's arrival_time
    printf("Passenger %d arrived on board with request type %d and wait time %d and ride time %d \n", num,passengers[num]->cab_type,wait_time,passengers[num]->ride_time);
    int request = passengers[num]->cab_type;
    if (!request)
    {
        // of type available_cab_ride_pool
         start = clock();
        int a = myCond(request,num);
        while(a && clock()-start<wait_time*1000)
        {
            a = myCond(request,num);
        }
        if (a != 0)
        {
            printf("Passenger %d leaved without service\n", num);
            pthread_exit(NULL);
        }
        start = clock();
        while(clock()-start<ride_time*1000);                     // ride the cab 
        cabss[passengers[num]->cab_num]->status = 1;        // ready for ride
        printf("Passenger %d left %d cab\n",num,passengers[num]->cab_num);    
    }
    else
    {
        // if type available_cab_ride_premium
        start = clock();
        int a = myCond(request,num);
        while(a && clock()-start<wait_time*1000)
        {
            a = myCond(request,num);
        }
        if (a != 0)
        {
            printf("Passenger %d leaved without service\n", num);
            pthread_exit(NULL);
        }
        start = clock();
        while(clock()-start<ride_time*1000)
        {
            printf(" O gaddi %d meri chali tyon tyon %d ->%ld \n",passengers[num]->cab_num,num,clock()-start);
        }
        ;                     // ride the cab 
        cabss[passengers[num]->cab_num]->status = 1;        // ready for ride
        printf("Passenger %d left %d cab\n",num,passengers[num]->cab_num);    
    }

    // if made available then use the nearest one available
    // lock server
    // C.S of choosing server
    sem_post(&ride_done);
    passengers[num]->payment_status = 1; // ready to do payment
    sem_wait(&available_servers);
    printf("Going bye %d\n", num);
    pthread_exit(NULL);
}

void *function_srvr(void *arg)
{
    // deal with srvr numbered 'n'
    int nu = *(int *)arg;
    // printf("La %d\n",nu);
    while (!Alldone)
    {
        printf("Server %d ready for payment\n", nu);
        if (Alldone)
        {
            pthread_exit(NULL);
        }
        sem_wait(&ride_done); // waiting for rider to be completed
        if (Alldone)
        {
            pthread_exit(NULL);
        }
        sem_wait(&passenger_lock); // Allow only 1 server to iterate over the array
        ll num = 0;
        ll fg = 0;
        for (num = 0; !fg && num < N; num++)
        {
            if (passengers[num]->payment_status == 1)
            {
                fg = 1;
            }
        }
        if (Alldone)
        {
            pthread_exit(NULL);
        }
        num--;
        printf("Payment of rider numbered  %d done by server numbered %d\n", num, nu);
        sleep(1);							  // Assume 1 second to do the payment
        passengers[num]->payment_status = -1; // NULL value , passenger leaves
        sem_post(&passenger_lock);			  // release lock for other servers to find riders
        sem_post(&available_servers);		  // ready to take a passenger's payment
        for (ll n = 0; n < N; n++)
        {
            printf("%d ", passengers[n]->payment_status);
        }
        printf("\n");
    }
    pthread_exit(NULL);
}

int main(void)
{
    srand(time(0));
    printf("Welcome to Ober Cab Services :- Hope you have a nice day\n");
    scanf("%d %d %d", &N, &M, &K);

    // Initialize semaphores
    sem_init(&available_cab_ride_pool, 0, N);	// Number of free cabs
    sem_init(&waiting_passenger, 0, M);			 // Number of riders
    sem_init(&available_cab_ride_premium, 0, N); // Number of free cabs for premium
    sem_init(&available_servers, 0, 0);			 // Number of free available_servers
    sem_init(&ride_done, 0, 0);
    sem_init(&passenger_lock, 0, 1); // Initialise lock to 1

    // Initialize structs
    passenger_init();
    cab_init();
    server_init();

    // if (clock_gettime(CLOCK_REALTIME, &start) == -1)
    // {
    // 	handleerror("clock gettime");
    // }

    // Create threadss
    int numbers[2000];
    for(int g=0;g<2000;g++)
    {
        numbers[g] = g;
    }
    for (ll n = 0; n < N; n++)
    {
        pthread_create(&pss_threads[n], NULL, function_pass,&numbers[n]);
    }
    for (ll n = 0; n < K; n++)
    {
        int b = n;
        pthread_create(&srvr_threads[n], NULL, function_srvr,&numbers[n]);
    }

    // Wait for threads to join one by one
    for (ll k = 0; k < N; k++)
    {
        pthread_join(pss_threads[k], NULL);
    }
    Alldone = 1;
    for (ll k = 0; k < K; k++)
    {
        sem_post(&ride_done);
        sem_post(&passenger_lock);
    }
    for (ll k = 0; k < K; k++)
    {
        pthread_join(srvr_threads[k], NULL);
    }
    printf("Thank you for using Ober Cab Services. We wish to serve you again\n");
}
