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

// create semaphores for burger and fries, mutex
sem_t burger_sem;
sem_t fries_sem;
mutex mtx;

// vars to count/keep track of orders
int waitBurger = 0;
int waitFries = 0;
int in_progress_burgers = 0;
int in_progress_fries = 0;

// Do not change
void process_order() {
    sleep(2);
}

void place_order(int type) {
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process; 
     *     if true -> wait (print waiting)
     *     otherwise place this order (print order)
     *  Use type_names[type] to print the order type
     */
    
    // lock
    mtx.lock();

    // if k orders are already in process, wait
    if ((type == BURGER && in_progress_burgers >= k) || (type == FRIES && in_progress_fries >= k)) {
        if (type == BURGER) {
            waitBurger++;
            cout << "Waiting: " << type_names[type] << endl;
            mtx.unlock();
            sem_wait(&burger_sem);
        }
        // case for fries order
        else {
            waitFries++;
            cout << "Waiting: " << type_names[type] << endl;
            mtx.unlock();
            sem_wait(&fries_sem);
        }
    }
    // otherwise, place the order
    else {
        // if order is burger
        if (type == BURGER) {
            in_progress_burgers++;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&burger_sem);
        } 
        else {
            in_progress_fries++;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&fries_sem);
        }
    }


    process_order();        // Do not remove, simulates preparation

    /**
     *  Add logic for synchronization after order processed
     *  Allow next order of the same type to proceed if there is any waiting; if not, allow the other type to proceed.
     */

    // lock
    mtx.lock();

    // if waiting, allow order of the same type to proceed
    if (type == BURGER) {
        in_progress_burgers--;
        if (waitBurger > 0) {
            waitBurger--;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&burger_sem);
        } 
        else if (waitFries > 0) {
            in_progress_fries--;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&fries_sem);
        } 
        else {
            mtx.unlock();
        }
    }
    // if fries order
    else {
        in_progress_fries--;
        if (waitFries > 0) {
            waitFries--;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&fries_sem);
        } 
        else if (waitBurger > 0) {
            in_progress_burgers--;
            cout << "Order: " << type_names[type] << endl;
            mtx.unlock();
            sem_post(&burger_sem);
        } 
        else {
            mtx.unlock();
        }
    }
}

int main() {
    // Initialize necessary variables, semaphores etc.
    
    // Read data: done for you, do not change
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
    return 0;
}