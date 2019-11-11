#include "global.h"

void biryani_ready(ll num_of_vessels)
{
    ll count = num_of_vessels;
    ll found = 0;
    while (count)
    {
        for (ll n = 0; count && n < N; n++)
        {
            pthread_mutex_lock(&lock_tables[n]);
            if(count && tables[n]->status)
            {
                count--;
                tables[n]->status = 0;                                      // place the vessel at this table number
                printf("The vessel %lld is being filled\n",n);
            }
            pthread_mutex_unlock(&lock_tables[n]);
        }
    }
}

void *student_arrives(void *arg)
{
    ll num = *(ll *)arg;
    ll time_of_wait = students[num]->time_of_arrival;
    time_t start = clock();
    while(clock()-start < time_of_wait);
    printf("Student %lld comes into the mess\n",num);

}

void *prepare_biryani(void *arg)
{
    ll num = *(ll *)arg;
    printf("may aaya %lld\n",num);
    // let this robot prepare biryani_ready
    while (!Alldone)
    {
        robots[num]->num_vessels = get_random(MAX_VESSELS, 0);           // number of vessels cooked
        robots[num]->time_to_cook = get_random(MAX_PREPARATION_TIME, 0); // time to cook them
        printf("Robot %lld has started preparing %lld vessels\n",num,robots[num]->num_vessels);
        printf("Cooking for %lld\n",robots[num]->time_to_cook);
        sleep(robots[num]->time_to_cook);
        // now prepare all the vessels
        printf("Robot %lld finished preparing all vessels\n", num);
        biryani_ready(robots[num]->num_vessels);
    }
    printf("Robot done working bye from robot side\n");
}

int main(int argc, char const *argv[])
{
    srand(time(0));
    scanf("%lld %lld %lld", &N, &M, &K);
    tables_init();
    robots_init();
    students_init();
    for (ll m = 0; m < M; m++)
    {
        pthread_create(&thread_robots[m],NULL,prepare_biryani,(void *)&m);
    }
    for (ll n = 0; n < N; n++)
    {
	pthread_create(&pthread_tables[n],NULL,,(void *)&n);
    }
    for (ll k = 0; k < K; k++)
    {
        pthread_create(&pthread_students[k],NULL,,(void *)&k);
    }
    // sleep(100);
    // Alldone = 1;
    for (ll m = 0; m < M; m++)
    {
        pthread_join(thread_robots[m],NULL);
    }
    for (ll m = 0; m < N; m++)
    {
    	pthread_join(&thread_tables[m],NULL);
    }
    for (ll m = 0; m < K; m++)
    {
    	pthread_join(&thread_students[m],NULL);
    }
    
    
    return 0;
}
