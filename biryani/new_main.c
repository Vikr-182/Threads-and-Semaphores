#include "global.h"

void biryani_ready(ll num_of_vessels, ll num_robot)
{
    ll count = num_of_vessels;
    ll found = 0;
    while (count)
    {
        // * KEEP ITERATING UNTIL ALL VESSELS ARE PLACED
        for (ll n = 0; !Alldone && count && n < N; n++)
        {
            if(Alldone || count > CAP_STUDENTS )
            {
                return ;
            }
            pthread_mutex_lock(&lock_tables[n]);                    // ! LOCK MUTEX FOR TABLES HERE
            if (count && (tables[n]->status == 1) && !Alldone)      // * TABLE READY TO BE SERVED TO
            {
                count--;
                tables[n]->status = 0;                                                      // * TABLE BUSY NOW
                tables[n]->capacity_total = get_random(CAP_STUDENTS + 1, 0);                // * GENERATE NUMBER OF PLATES
                tables[n]->available_slots = get_random(tables[n]->capacity_total+1, 0);    // * NUMBER OF SLOTS FOR SERVING
                printf( ANSI_BLUE "The table %lld is being filled by vessel from %lld robot with ", n, num_robot);
                printf("slots decided %lld\n"  ANSI_RESET,tables[n]->available_slots);
                for (ll m = 0; m < tables[n]->available_slots; m++)
                {
                    tables[n]->slots[m] = 1;                                                // * READY TO SERVE
                }
            }
            pthread_mutex_unlock(&lock_tables[n]);                 // ! UNLOCKING THE MUTEX FOR TABLES HERE
    }
    
}

void *prepare_biryani(void *arg)
{
    ll num = *(ll *)arg;
    while (!Alldone)
    {
        robots[num]->num_vessels = maxormin(get_random(MAX_VESSELS,0),CAP_STUDENTS,0);  // * MINIMUM OF THE TWO STUDENTS LEFT AND RANDOM NUMBER
        printf(ANSI_YELLOW "Robot %lld has started preparing %lld vessels . It will take %lld time ", num, robots[num]->num_vessels, robots[num]->time_to_cook);
        sleep(robots[num]->time_to_cook);                                               // * TIME TO PREPARE THE VESSELS
        printf(ANSI_YELLOW "Robot %lld finished preparing all vessels now will start loading\n" ANSI_RESET, num);
        prepare_biryani(robots[num]->num_of_vessels,num);
    }
    
}

int main(int argc, char const *argv[])
{
    srand(time(0));
    bar();bar();printf("Welcome to Kadamba mess\n");bar();bar();
    printf("Enter number of tables:\nEnter number of robots:\nEnter number of students:\n");
    scanf("%lld %lld %lld", &N, &M, &K);
    CAP_STUDENTS = K;
    tables_init();
    robots_init();
    students_init();
    ll numbered_array[300000];
    for (ll p = 0; p < 300000; p++)
    {
        numbered_array[p] = p;
    }

    return 0;
}
