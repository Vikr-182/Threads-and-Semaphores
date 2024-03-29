#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define ANSI_RESET "\x1b[0m"
#define ANSI_BLACK "\x1b[30m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_WHITE "\x1b[38m"
#define ANSI_RED_BG "\x1b[41m"
#define ANSI_GREEN_BG "\x1b[42m"
#define ANSI_YELLOW_BG "\x1b[43m"
#define ANSI_BLUE_BG "\x1b[44m"
#define ANSI_PURPLE_BG "\x1b[45m"
#define ANSI_DARKBLUE_BG "\x1b[46m"
#define ANSI_GREY_BG "\x1b[47m"
#define ANSI_CLEAR_SCREEN "\e[1;1H\e[2J"
#define ANSI_RESET_SCREEN "\033\143"


#define POOL_CAB_REQUEST 1
#define PREMIUM_CAB_REQUEST 0
#define MAX_PASS 200
#define MAX_CABS 50
#define MAX_SERVERS 100
#define MAX_RIDE_TIME 40	  // maximum ride time of 4s
#define MAX_ARRIVAL_TIME 10 // maximum arrival time of customers
#define MAX_WAIT_TIME 5     // maximum wait time of 5s
#define handleerror(msg) \
do {perror(msg);exit(EXIT_FAILURE);}while(0);
typedef int ll;
ll N, M, K;

struct timespec start,end;

typedef struct Cab
{
	int cab_num;
	int num_pass[2];
	int cab_type;
	int ride_time;
	int status;
} cb;

typedef struct Passenger
{
	int arrival_time;
	int max_wait_time;
	int ride_time;
	int cab_type;
	int payment_status;
    int cab_num;
} pss;

typedef struct Server
{
	int payment_status;
	int payment_rider;
	int payment_of_cab;
} srvr;

pss *passengers[MAX_PASS];
srvr *servers[MAX_SERVERS];
cb *cabss[MAX_CABS];

pthread_t pss_threads[MAX_PASS];
pthread_t cb_threads[MAX_CABS];
pthread_t srvr_threads[MAX_SERVERS];
pthread_cond_t cond[MAX_PASS];
pthread_mutex_t mutex_pass[MAX_PASS];
pthread_mutex_t mutex_cabs[MAX_CABS];
pthread_mutex_t print_mutex;
pthread_cond_t cond_pool;
pthread_cond_t cond_empty;
pthread_cond_t cond_ppop;
pthread_mutex_t mutex_pool;
pthread_mutex_t mutex_empty;
pthread_mutex_t mutex_popl;

int Alldone = 0;

// semaphores
sem_t waiting_passenger;
sem_t available_cab_ride_pool;
sem_t available_cab_ride_premium;
sem_t available_servers;
sem_t ride_done;

// mutex
sem_t server_lock;
sem_t cab_lock;
sem_t passenger_lock ;

ll get_random(ll limit, ll flag) // specifies whether 0 is allowed
{
	ll a = rand() % limit;
	if (a < 0)
	{
		a *= -1;
	}
	if ((flag & !a))
	{
		a += limit / 2;
	}
	if(limit==1 && !a && !flag)
	{
		a = 1;
	}
	return a;
}

char * decoder_cab_type(ll fg)
{
	if(fg)
	{
		char *A = "POOL TYPE";
		return (char *)A;
	}
	else
	{
		char *A = "PREMIUM TYPE";
		return (char *)A;
	}
}

void cab_init()
{
	// pthread_mutex_init(&mutex_cabs,NULL);
	pthread_mutex_init(&mutex_empty,NULL);
	pthread_mutex_init(&mutex_popl,NULL);
	pthread_mutex_init(&mutex_pool,NULL);
	pthread_cond_init(&cond_empty,NULL);
	pthread_cond_init(&cond_pool,NULL);
	pthread_cond_init(&cond_ppop,NULL);	
	for (ll n = 0; n < MAX_CABS; n++)
	{
		cabss[n] = (cb *)malloc(sizeof(cb));
		cabss[n]->cab_num = n;
		cabss[n]->cab_type = 0;   			// empty cab
		cabss[n]->num_pass[0] = -1;   			// 0 passengers
		cabss[n]->num_pass[1] = -1;				// 0 passengers
		cabss[n]->ride_time = -1; 			// NULL ride ride_time
		cabss[n]->status = 1;	 			// waiting
	}
}

void passenger_init()
{
	for (ll n = 0; n < MAX_PASS; n++)
	{
		passengers[n] = (pss *)malloc(sizeof(pss));
		passengers[n]->arrival_time = get_random(MAX_ARRIVAL_TIME,1);
		passengers[n]->cab_type = get_random(20000, 1)/10000;						// either will pool or will ask for premium cab
		passengers[n]->ride_time = get_random(MAX_RIDE_TIME, 0); 			// random ride time
		passengers[n]->payment_status = -1;								// NULL payment status, 1 corresponds to ready for payment, 2 corresponds to payment done
		passengers[n]->max_wait_time = get_random(MAX_WAIT_TIME, 0);
        passengers[n]->cab_num = -1;
	}
}

void server_init()
{
	for (ll n = 0; n < MAX_SERVERS; n++)
	{
		servers[n] = (srvr *)malloc(sizeof(srvr));
		servers[n]->payment_status = 0; 	// not accepted any payment_status
		servers[n]->payment_rider = -1;
		servers[n]->payment_of_cab = -1;
	}
}
