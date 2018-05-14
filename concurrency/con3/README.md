#COMMAND LINE
$ make
$ con3_pro1 <pthreads Number>
$ con3_pro2 <Searcher threads Number> <Inserter threads Number> <Deleter threads Number>

con3_pro1 and con3_pro2 is for concurrency problem 1 and problem 2 respectly.  

Problem 1
Consider a sharable resource with the following characteristics:

As long as there are fewer than three processes using the resource, new processes can start using it right away.
Once there are three processes using the resource, all three must leave before any new processes can begin using it.
Implement a mutual exclusion solution that meets the above constraints.

Solution design for problem1
process()
{
  lock mutex1
  if count == 3 then
    sem wait;
  count++
  unlock mutex1
  
  ......Do something
  
  lock mutex2
  count--
  if count == 0 then
    sem post;
  unlock mutex2
}

Problem 2
Three kinds of threads share access to a singly-linked list: 
searchers, inserters and deleters. Searchers merely examine the list; hence they can execute concurrently with each other. 
Inserters add new items to the end of the list; insertions must be mutually exclusive to preclude two inserters from inserting new items at about the same time. 
However, one insert can proceed in parallel with any number of searches. 
Finally, deleters remove items from anywhere in the list. At most one deleter process can access the list at a time, and deletion must also be mutually exclusive with searches and insertions.

Solution design for problem2

// Using problem 1 solution Concept to approach Searcher Demands.
// Problems:
// 1. When Searcher LOCK MUTEX_DELETE_SEARTCH, it will lock other seacher threads.
// 2. Deleter need to wait all searcher complete their job.
// 3. Using problem 1 solution need to set the limite number of executing threads to avoid deleter cannot get mutex.
Search (){
  
  Mutex_lock (MUTEX1)
  if Mutex_trylock(MUTEX_DELETE_SEARTCH) == Fail then
      if LOCKER == DELETE then
          Mutex_lock(MUTEX_DELETE_SEARTCH);
      else 
          if SearchCounts == 3 then
            Mutex_lock(MUTEX_DELETE_SEARTCH);
            
  count++
  Mutex_unlock (MUTEX1)
  
  
  
  .......Do Some Things...........
  
  Mutex_lock MUTEX2
  count--
  if count == 0 then    
      Mutex_unlock(MUTEX_DELETE_SEARTCH)
  Mutex_lock MUTEX2
}


// MUTEX_DELETE_INSERT will also lock other inserter to avoid mutiple inserters running.
Insert (){
  Mutex_lock (MUTEX_DELETE_INSERT)
  .......Do Some Things...........
  Mutex_unlock(MUTEX_DELETE_INSERT)
}

Delete (){
  Mutex_lock (MUTEX_DELETE_SEARTCH)
  Mutex_lock (MUTEX_DELETE_INSERT)
  .......Do Some Things...........
  Mutex_unlock(MUTEX_DELETE_INSERT)
  Mutex_lock (MUTEX_DELETE_SEARTCH)
}







