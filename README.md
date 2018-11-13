Q-1)
Idea:
We have 2*n players and n refrees and we want to arrange them in groups such that there are 2 players and a refree.
Now when any person enters the academy he checks for the organiser if he is free then he meets the organiser.
Now out of the persons who have entered the academy organiser forms a group of 3 required people for a match and till the start of match the organiser remains busy
so to implement that we have used a mutex so organiser remanis busy.
We implement the functions like enteracademy meet entercourt etc in order after we gwt the required group.
To get the required group the pair of players wait for the refree and vice - versa is also true.
Also the probability decides whether the next person who entered is player or refree.
To run: gcc 1.c
./a.out
Q-2)

These are the structs for the problem
    evm has:

    id of EVM,
    thread id of thread corresponding to that EVM
    corresponding booth 
    no of slots in EVM
    boolean flag for its working(1 -> working).

    VOTER has:

    id of Voter
    status(NEW_VOTER,WAITING_VOTER,ASSIGNED_VOTER,COMPLETED_VOTER)
    thread id of thread corresponding to that voter
    corresponding booth 
    corresponding evm

    BOOTH has:

    id of Booth
    thread id of thread corresponding to that booth
    no of voters
    max num of slots in evm
    num of voters voted
    num of evms
    mutex to assign voter and corresponding conditions.
    all the voters
    all the evms

Idea:

Now we input the number of booths and the number of voters and evms in each booth.
For each booth we create a new thread and for corresponding voters and evms in that booth we start new threads to depict their functioning.
Now we have the condition that a voter can vote only once and evm can take many votes at time through  different slots.
So to realise this we use a mutex lock and two condition variables.
These condition variables are used for orderly execution of the synchronisation between the evm threads and the voter threads
as the voter first has to wait for the evm to be allocated then he casts his vote ,then evm checks for any left voters so that it can start again.
we lock the particular slot of evm at a particular instant when  its  busy so no two can vote on same slot.

Implementation:
We have the structs defined above.Also we have functions for working of each thread i.e a booth thread a voter thread or a evm thread.
we have a function polling_ready_evm() which allocates the evms to the voters in randomly manner both in selection of the voter for a particular slot or the number of slots in evm.
we have function voter_wait_for_evm() which realises the voter has to wait for evm to be allocated to it.
we have voter_in_slot() to depict the process of voting by a voter on a particular slot at an evm in a booth.

To run gcc -pthread 2.c
./a.out

Q-3)
i have implemented three different mergesort via normal with a single process and sequential execution(31.c) , one with processes being created for each division of the array in subparts (32.c) and the one which uses threads in place of processes(33.c)

32.c

    Forking means creating two processes. One is parent other is child.
    Thus unlike threads hiearchy exsists.
    As they are diffrent processes chance of deadlock is reduced.
    Here shared memory is used, which is a necessity to share data between processes.
    Here recurssive mergeSort are considered as diffrent processes.

33.c


    Here threads are used instead of processes.
    Thus there is no hiearchy here.
    Here locks need to be used wisely. As there is only one lock, deadlock not possiblr.
    Here recurssive mergeSort are considered diffrent threads.
    These threads are joined at last.
    We can say that threads are faster than processes as they are LWPs


for arrays ranging 10^4
the run time are as follows

31.c(normal)

real	0m0.013s
user	0m0.013s
sys	   0m0.000s

32.c (forking)


real	0m0.207s
user	0m1.137s
sys	0m0.191s


33.c(threaded)

real	0m0.164s
user	0m0.050s
sys	0m0.374s

So we can see that normal mergesort is the fastest followed by the threaded and concurrent process merge sort is slowest due to time taken by creation of processes

