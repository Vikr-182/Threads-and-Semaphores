#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
typedef long long ll;

#define MAX_SLOTS 11
#define MAX_PREPARATION_TIME 6
#define MAX_ROBOTS 30
#define MAX_STUDENTS 100
#define MAX_TABLES 20
#define MAX_VESSELS 25
#define MAX_CAPACITY_TO_FEED 30
#define MAX_WAITING_TIME 20


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

ll N, M, K;
ll Alldone;

pthread_t thread_robots[MAX_ROBOTS];
pthread_t thread_students[MAX_STUDENTS];
pthread_t thread_tables[MAX_TABLES];

pthread_mutex_t lock_tables[MAX_TABLES];
pthread_mutex_t lock_students[MAX_STUDENTS];

typedef struct Robot
{
	ll rob_id;
	ll num_vessels;
	ll time_to_cook;
	ll ready_to_prepare;
	ll status;
} rb;

typedef struct Table
{
	ll table_id;
	ll slots[10];
	ll students_eating;
	ll available_slots;
	ll capacity_total;
	ll status;
} tbl;

typedef struct Student
{
	ll stud_id;
	ll table_num;
	ll time_of_arrival;
} stud;

rb *robots[MAX_ROBOTS];
tbl *tables[MAX_TABLES];
stud *students[MAX_STUDENTS];

ll maxormin(ll a,ll b,ll flag)
{
	return (flag ? (a>b))
}

ll get_random(ll limit, ll flag)
{
	ll a = rand() % limit;
	if (a < 0)
	{
		a *= -1;
	}
	a += (!flag & !a) * (limit / 2);
	return a;
}

void robots_init()
{
	for (ll m = 0; m < M; m++)
	{
		robots[m] = (rb *)malloc(sizeof(rb));
		robots[m]->rob_id = m;
		robots[m]->num_vessels = -1;
		robots[m]->time_to_cook = get_random(MAX_PREPARATION_TIME, 0);
		robots[m]->ready_to_prepare = -1; // this robot is not ready to prepare
		robots[m]->status = -1;			  // waiting
	}
}

void tables_init()
{
	for (ll n = 0; n < N; n++)
	{
		tables[n] = (tbl *)malloc(sizeof(tables));
		tables[n]->table_id = n;
		tables[n]->capacity_total = get_random(MAX_CAPACITY_TO_FEED, 0);
		tables[n]->available_slots = tables[n]->capacity_total;
		tables[n]->status = 1;				// ready to be filled upon
		for (ll i = 0; i < 10; i++)
		{
			tables[n]->slots[i] = 0;		// slots currently free
		}
		
	}
}

void students_init()
{
	for (ll k = 0; k < K; k++)
	{
		students[k] = (stud *)malloc(sizeof(stud));
		students[k]->stud_id = k;
		students[k]->table_num = -1;
		students[k]->time_of_arrival = get_random(MAX_WAITING_TIME,1);
	}
}