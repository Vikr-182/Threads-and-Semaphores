# Threads-and-Semaphores
Uses of threads and semaphores using POSIX API.

## Quicksort
In this problem, quicksort was implemented using 3 seperate methods.
- Simple Quicksort  :-
Basic version of quicksort using median of median method for selection pivot.
- Concurrent Quicksort :-
Forking 2 times, giving the sets of and children seperate arrays to sort, after which parent merges the arrays.
- Multithreaded Quicksort :-
Creating 5 different threads for 4 segments of the array. First, the array is partitioned into 4 parts by finding 3 medians, once for the array and once for left and right subparts.
### Implementation 
All of them have access to shared functions declared in *constraints.h* file. Each have their own implementation of quicksort which calls the standard quicksort declared in the same file.

## Ober Cab Services
A thread-synchronization problem. A cab service was implemented featuring -  
- Passengers :- They come at random times with 2 kinds of requests - PREMIUM or POOL. If time exceeds their wait time and no cab has arrived, they leave unserviced.
- Cabs :- A POOL cab type can service another POOL request provided it is not full. The maximum capacity for a POOL cab is 2 whereas for a PREMIUM cab is 1 only.
- Servers :- After their ride, passengers are to pay through the payment server which only accepts one passenger's payment at a time.

### Implementation 
The task is accompolished using threads. A seperate thread is kept for passengers as well as servers. 

####  Passenger Thread
Upon it's arrival, this waits for a random amount of time as per the corresponding passenger's arrival time. After which, it iterates over the list of cabs using a mutex lock for each cab until it finds one. If this searching time exceeds maximum wait time, the passengers exits.
<br/>
After it's ride, the passenger is made to wait until a server thread *signals* it is free via a binary semaphore. After which the thread exits.

####  Server Thread
Iterates over passenger list using mutex lock until it finds one passenger hacing ride completed. After which it turns the binary semaphore on so that passenger can continue with the payment. The server remembers which passenger's payment is being done.
