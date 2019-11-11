#include "global.h"

int myCond(int request, int pass_num)
{
    if (request) //  ? OF CAB TYPE POOL
    {
        ll fg = 0;
        ll ind = 0;
        for (ll n = 0; !fg && n < M; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]); // ! LOCK THE CAB MUTEX
            if (cabss[n]->cab_type == 1 && cabss[n]->status != -1)
            {
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
                printf(ANSI_GREEN "Passenger %d boarded %d cab ok iih\n" ANSI_RESET, pass_num, ind);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]); // ! UNLOCK THE CAB MUTEX
        }
        for (ll n = 0; !fg && n < M; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]); // ! LOCK THE CAB MUTEX
            if (cabss[n]->cab_type == 0 && cabss[n]->status != -1)
            {
                // empty
                fg = 1;
                ind = n;
                if (cabss[ind]->num_pass[0] == -1)
                {
                    cabss[ind]->num_pass[0] = pass_num;
                    cabss[ind]->cab_type = 1;
                }
                else
                {
                    cabss[ind]->num_pass[1] = pass_num;
                }
                pthread_mutex_lock(&print_mutex);
                printf(ANSI_GREEN);
                printf("Passenger %d boarded %d cab ok ooh\n", pass_num, ind);
                printf(ANSI_RESET);
                pthread_mutex_unlock(&print_mutex);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]); // ! UNLOCK THE CAB MUTEX
        }
        return !fg;
    }
    else // ? OF CAB TYPE PREMIUM
    {
        ll fg = 0;
        ll ind = 0;
        for (ll n = 0; n < M && !fg; n++)
        {
            pthread_mutex_lock(&mutex_cabs[n]); // ! LOCK THE CAB MUTEX
            if (cabss[n]->cab_type == 0 && cabss[n]->status != -1)
            {
                // empty ride
                fg = 1;
                ind = n;
                if (cabss[ind]->num_pass[0] == -1)
                {
                    cabss[ind]->num_pass[0] = pass_num;
                    cabss[n]->status = -1;
                }
                pthread_mutex_lock(&print_mutex);
                printf(ANSI_GREEN "Passenger %d boarded %d cab ok aah %d na\n" ANSI_RESET, pass_num, ind, cabss[n]->cab_type);
                cabss[n]->status = -1;
                pthread_mutex_unlock(&print_mutex);
                passengers[pass_num]->cab_num = ind;
            }
            pthread_mutex_unlock(&mutex_cabs[n]); // ! UNLOCK THE CAB MUTEX
        }
        return !fg;
    }
}

void *function_pass(void *arg)
{
    // * PASSENGER NUMBERED num
    int num = *(int *)arg;
    int wait_time = passengers[num]->max_wait_time;
    int arrival_time = passengers[num]->arrival_time;
    int ride_time = passengers[num]->ride_time;
    time_t start = time(0);
    while (time(0) - start < arrival_time)
        ; // * WAIT FOR IT'S ARRIVAL_TIME
    pthread_mutex_lock(&print_mutex);
    printf(ANSI_YELLOW "Passenger %d arrived on board with request type %s and wait time %d and ride time %d \n" ANSI_RESET, num, decoder_cab_type(passengers[num]->cab_type), wait_time, passengers[num]->ride_time);
    pthread_mutex_unlock(&print_mutex);

    int request = passengers[num]->cab_type;
    if (!request)
    {
        // ? ASKED OF TYPE AVAILABLE_CAB_RIDE_POOL

        // todo LOCKING AND CHECKING FOR POOL OR EMPTY CAB
        // printf("%d haan\n",  num);
        pthread_mutex_lock(&mutex_pool);
        struct timeval tv;
        struct timespec ts; // ! DECLARE TIME_SPEC VARIABLES
        // * INITIALISE TIME VARIABLES
        gettimeofday(&tv, NULL);
        int timeInMs = 1000 * wait_time;
        ts.tv_sec = time(NULL) + timeInMs / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
        ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
        ts.tv_nsec %= (1000 * 1000 * 1000);
        // clock_gettime(CLOCK_REALTIME, &ts);
        // ts.tv_sec += 5;
        int a = myCond(request, num);
        int b = 0;
        // printf("%d %d %d\n", a, b, num);
        if(a)
        {
            b = pthread_cond_timedwait(&cond_empty, &mutex_empty, &ts);
        }
        // printf("mid %d %d\n", b, num);
        while (a && b == 0)
        {
            a = myCond(request, num);
            if (a)
                b = pthread_cond_timedwait(&cond_pool, &mutex_pool, &ts);
        }
        if (a != 0 || b != 0)
        {
            printf(ANSI_RED "Passenger %d leaved without service\n" ANSI_RESET, num);
            pthread_mutex_unlock(&mutex_pool);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex_pool);
        sleep(ride_time); // * RIDE THE CAB
        printf(ANSI_BLUE "Passenger %d left %d cab\n" ANSI_RESET, num, passengers[num]->cab_num);
        ll ind = passengers[num]->cab_num;
        if (cabss[ind]->num_pass[0] == num)
        {
            // * WAS THE ONLY ONE TO BOARD CAB
            cabss[ind]->num_pass[0] = -1;
            cabss[passengers[num]->cab_num]->status = 0; // * VACATED FOR RIDE

            // * SIGNAL BOTH PREMIUM AND POOL REQUESTS
            pthread_cond_signal(&cond_pool);
            pthread_cond_signal(&cond_empty);
        }
        else
        {
            // * WAS THE SECOND PERSON TO BOARD THE CAB
            cabss[ind]->num_pass[1] = -1;

            // * SIGNAL ONLY TO POOL REQUESTS
            pthread_cond_signal(&cond_pool);
            cabss[passengers[num]->cab_num]->status = 1; // * VACATED FOR RIDE
        }
    }
    else
    {
        // ? IF TYPE AVAILABLE_CAB_RIDE_PREMIUM
        // todo LOCKING AND CHECKING FOR POOL OR EMPTY CAB
            //  printf("%d oh haan\n",  num);
        pthread_mutex_lock(&mutex_empty);
        struct timeval tv;
        struct timespec ts; // ! DECLARE TIME_SPEC VARIABLES
        // * INITIALISE TIME VARIABLES
        gettimeofday(&tv, NULL);
        int timeInMs = 1000 * wait_time;
        ts.tv_sec = time(NULL) + timeInMs / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
        ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
        ts.tv_nsec %= (1000 * 1000 * 1000);
        // clock_gettime(CLOCK_REALTIME, &ts);
        // ts.tv_sec += wait_time;
        int a = myCond(request, num);
        int b = 0;
        // printf("%d %d %d || \n", a, b, num);
        if(a)
        {
            b = pthread_cond_timedwait(&cond_empty, &mutex_empty, &ts);
        }
        while (a && b == 0)
        {
            a = myCond(request, num);
            if (a)
                b = pthread_cond_timedwait(&cond_empty, &mutex_empty, &ts);
        }
        // printf("%d %d %d | \n", a, b, num);
        if (a != 0 || b != 0)
        {
            printf(ANSI_RED "Passenger %d leaved without service\n" ANSI_RESET, num);
            pthread_mutex_unlock(&mutex_empty);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex_empty);
        sleep(ride_time);                            // * RIDE THE CAB
        cabss[passengers[num]->cab_num]->status = 1; // * READY FOR RIDE
        printf(ANSI_BLUE "Passenger %d left %d cab\n" ANSI_RESET, num, passengers[num]->cab_num);
        ll ind = passengers[num]->cab_num;

        // * WAS THE ONLY PERSON TO BOARD THE CAB
        cabss[ind]->status = 0;
        cabss[ind]->num_pass[0] = -1;

        // * SIGNAL TO BOTH POOL AND PREMIUM REQUESTS
        pthread_cond_signal(&cond_pool);
        pthread_cond_signal(&cond_empty);
    }

    // if made available then use the nearest one available
    // lock server
    // C.S of choosing server
    sem_post(&ride_done);
    passengers[num]->payment_status = 1; // ready to do payment
    sem_wait(&available_servers);
    printf(ANSI_RED "Going bye %d\n" ANSI_RESET, num);
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
        sleep(1);                             // Assume 1 second to do the payment
        passengers[num]->payment_status = -1; // NULL value , passenger leaves
        sem_post(&passenger_lock);            // release lock for other servers to find riders
        sem_post(&available_servers);         // ready to take a passenger's payment
        for (ll n = 0; n < N; n++)
        {
            // printf("%d ", passengers[n]->payment_status);
        }
        // printf("\n");
    }
    pthread_exit(NULL);
}

int main(void)
{
    srand(time(0));
    printf("Welcome to Ober Cab Services :- Hope you have a nice day\n");
    printf("Enter number of passengers:\n");
    printf("Enter number of cabs:\n");
    printf("Enter number of servers:\n");
    scanf("%d %d %d", &N, &M, &K);

    // Initialize semaphores
    sem_init(&available_cab_ride_pool, 0, N);    // Number of free cabs
    sem_init(&waiting_passenger, 0, M);          // Number of riders
    sem_init(&available_cab_ride_premium, 0, N); // Number of free cabs for premium
    sem_init(&available_servers, 0, 0);          // Number of free available_servers
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

    // * CREATE THREADS
    int numbers[2000];
    for (int g = 0; g < 2000; g++)
    {
        numbers[g] = g;
    }
    for (ll n = 0; n < N; n++)
    {
        pthread_create(&pss_threads[n], NULL, function_pass, &numbers[n]);
    }
    for (ll n = 0; n < K; n++)
    {
        int b = n;
        pthread_create(&srvr_threads[n], NULL, function_srvr, &numbers[n]);
    }

    // * WAIT FOR THREADS TO JOIN ONE BY ONE
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
