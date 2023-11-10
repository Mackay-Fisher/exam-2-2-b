#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;

// what primitives (mutex (use anywhere you're making an update))
// waiting and order prints
// use an array of two semaphores
//I created two semaphores, one for fries and one for burgers
//Added a mutex to lock any varible call from the main thread to lock conduitiona staments so that each one would be processed individual by the thread
mutex mtx;
sem_t fries; 
sem_t burgers;
int burger_wait = 0;
int fries_wait = 0;
int total_orders = 0;

// Do not change
void process_order() {
    sleep(2);
}
void place_order(int type) {
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process; 
     *     if true -> wait (print waiting)    sem_wait(sync_semaphore[])
     *     otherwise place this order (print order) // must be locked
     *  Use type_names[type] to print the order type
     */
    
    mtx.lock();
    //Locking the mutex to make sure that the thread is the only one that can access the varibles and that teh codniton cannot be changed while the thread is running
    if (total_orders >= k) {
        cout << "Waiting: " << type_names[type] << endl;
        if (type == BURGER) {
            //updating the values of the burges that are waiting to be processed will be decremenetd when the wait is over
            burger_wait +=1;
            mtx.unlock();
            /**
             * Creates a sepertion where we first check to see if we shoudl icnreas the wait value of the burger or fires then we have to unlcik to call sem_wait to decrement the value of the semaphore
             * across the board and then we have to relock the mutex so that the thread can wait for the semaphore to be posted
            */

            //unlocking the mutex so that the thread can wait for the semaphore
            sem_wait(&burgers);
            //relocking for the next conditonal stament

            /**
             * This is where the thread will wait for the semaphore to be posted and then it will relock the mutex so that it can process the order once it does we will start a new liocked process with an updated 
             * qued value so that we cna the process this will stop teh codniton of the last value having avalue of 1 or somthign weird so we don't call pahtom numbers creating to distinct parts to opperate in.
            */


            mtx.lock();
            //decrementing the number of burgers that are waiting to be processed
            burger_wait-=1;


            /**
             * Same logic as above but for the fries
            */
        } else if (type == FRIES) {
            //updating the values of the fries that are waiting to be processed will be decremenetd when the wait is over
            fries_wait +=1;
            //unlocking the mutex so that the thread can wait for the semaphore
            mtx.unlock();
            //relocking for the next conditonal stament
            sem_wait(&fries);
            mtx.lock();
            //decrementing the number of fries that are waiting to be processed
            fries_wait-=1;
        }
    }
    /**
    * This is the main logic of the system so after we are waiting and adjust the ordered values of the queue which every type comes out of the wait first burger or fry will then enter this logic statement which will
    * This will first increase the total number of the of orders per each respective type and then unlock the mutex so that the thread can process the order and then relock the mutex so that the thread can
    * The process out the value and then we check to see in the waiting values creating a variable queue for lack of better terms basically combining mu locking with synchronized variables to get a count of the number of remaining variables needed.
    * Then if the type matches and the amount is more than zero left in waiting it will send a post request releasing a waiting call then the type will persist to keep calling until it is empty art which case
    * it will see if the other one is greater than zero and then send a post request to that one and then the thread will exit then when both are zero it will quit.
     */

    //updating the values of the burgers that are waiting to be processed will be decremenetd when the wait is over
    cout << "Orders: " << type_names[type] << endl;
    //increasing the number of burgers that are waiting to be processed
    total_orders += 1;
    //unlocking the mutex so that the thread can process the order

    /**
     * This is meant to catch teh remainng calls form the intial locked value with teh un waite semaphore which now has an updated value so it will now eneter the following logic birck sow we
     * unlock the mutex so that the thread can process the order and then relock the mutex, this also lets up keep our totla order vaklues seperate so they do not get updated constabntly eahc run.
    */

    mtx.unlock();
    //processing the order
    process_order();
    //relocking the mutex so that the thread can process the order
    mtx.lock();

    /**
     * now starubg with the logic to recall now iwth a lessened order value so that we can keep track of the remaining orders and then we will check to see if the type matches and if it does then we will check to see if there are any
     * now we do the logic as above and find weteher or not to queu more value of teh same kind.
    */
    //decrementing the number of burgers that are waiting to be processed
    total_orders -= 1;
    //checking to see if there are any burgers waiting to be processed
    if (type==BURGER)
    {
        if(burger_wait > 0){
            //if there are burgers waiting to be processed then it will send a post request to the burgers semaphore
            sem_post(&burgers);
        }else if(fries_wait > 0){
            //if there are fries waiting to be processed then it will send a post request to the fries semaphore
            sem_post(&fries);
        }
    }else{
        if (fries_wait > 0) {
            //if there are fries waiting to be processed then it will send a post request to the fries semaphore
            sem_post(&fries);
        }else if(burger_wait > 0){
            //if there are burgers waiting to be processed then it will send a post request to the burgers semaphore
            sem_post(&burgers);
        }
    }
    mtx.unlock();
}

int main() {
    // Initialize necessary variables, semaphores etc.
    // Read data: done for you, do not change
    sem_init(&burgers, 0, k);
    sem_init(&fries, 0, k);
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    int t;
    cin >> k;
    cin >> t;
    for (int i = 0; i < t; ++i) {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }
    // Create threads: done for you, do not change
    thread* threads[MAX_THREADS];
    for (int i = 0; i < t; ++i) {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1) {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    
    // Deleting Semaphores
    return 0;
}
