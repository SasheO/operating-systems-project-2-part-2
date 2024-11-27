#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

/*
  references: https://www.geeksforgeeks.org/reader-writer-problem-using-monitors-pthreads/?ref=oin_asr7
*/


// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// other values
/*
#define BENSCHILLIBOWL_SIZE 10
#define NUM_CUSTOMERS 9
#define NUM_COOKS 3
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER
*/

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

// Global variable for the customers (represented each as threads)
pthread_t customer_threads[NUM_CUSTOMERS]; // threads that will hold customers. at most BENSCHILLIBOWL_SIZE
pthread_t cook_threads[NUM_COOKS]; // threads that will hold customers. at most BENSCHILLIBOWL_SIZE


/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int* customer_id = (int*)(long) tid;
    Order *customer_order; // needs to be on heap so the same order space does not get allocated to different customers from stack
    int i = 0;
    while(i<ORDERS_PER_CUSTOMER){ 
      pthread_mutex_lock(&bcb->mutex); // acquire lock
      while(IsFull(bcb)){
        pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex); // wait until <BENSCHILLIBOWL_SIZE orders are on the queue i.e. can add orders 
      }
      customer_order = malloc(sizeof(Order));
      customer_order->menu_item = PickRandomMenuItem();
      customer_order->customer_id = *customer_id;
      customer_order->next = NULL;
      if(AddOrder(bcb,customer_order)){
        i++;
        if (i==1){
          printf("Customer %d placed their %dst order, #%d: %s\n", *customer_id, i, customer_order->order_number, customer_order->menu_item);
        }
        else if (i==2){
          printf("Customer %d placed their %dnd order, #%d: %s\n", *customer_id, i, customer_order->order_number, customer_order->menu_item);
        }
        else if (i==3){
          printf("Customer %d placed their %drd order, #%d: %s\n", *customer_id, i, customer_order->order_number, customer_order->menu_item);
        }
        else{
          printf("Customer %d placed their %dth order, #%d: %s\n", *customer_id, i, customer_order->order_number, customer_order->menu_item);
        }
        
        pthread_cond_signal(&bcb->can_get_orders); // since order has been added, tell cooks they can get orders
      }
      pthread_mutex_unlock(&bcb->mutex); // release lock
    }
    
	return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
  Order * current_order_handling; 
  int* cook_id = (int*)(long) tid; 
	int orders_fulfilled = 0; // how many orders this cook has fulfilled
  
  while(!(WorkdayIsOver(bcb))){ // if the shop is still open (shop closes when all expected orders are fulfilled btw)
    pthread_mutex_lock(&bcb->mutex); // acquire lock

    while (IsEmpty(bcb) && !(WorkdayIsOver(bcb))){ // if the shop is still open (shop closes when all expected orders are fulfilled btw) and the queue is empty
      pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex); // wait until there is an order to handle
    }
    
      current_order_handling = GetOrder(bcb);
      if (current_order_handling!=NULL){ // if there is an order to take. GetOrder will return null for example when the shop is closing and another cook fulfilled the last order
        printf("Cook %d handled order %d (%s) from customer %d\n", *cook_id, current_order_handling->order_number, current_order_handling->menu_item, current_order_handling->customer_id);
        orders_fulfilled++;
        free(current_order_handling); // deallocate orders handled
        current_order_handling = NULL;
        bcb->orders_handled++;
        pthread_cond_signal(&bcb->can_add_orders); // since order has been handled, tell customers they can place more orders
      }
      pthread_mutex_unlock(&bcb->mutex); // release lock
    
  }  
  
  printf("Cook #%d fulfilled %d orders\n", *cook_id, orders_fulfilled);
  pthread_cond_signal(&bcb->can_get_orders); // edge case when last order is fulfilled by cook, so the others do not keep waiting indefinitely for signal
	return NULL;
}
 
/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
  srand(time(NULL));
  int *customer_id; // needs to be on heap so the same id location from stack doesn't get assigned to all threads
  int i; // for the for loops
  int *cook_id; // needs to be on heap so the same id location from stack doesn't get assigned to all threads
	bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS); 
  
  for (i=0; i<NUM_COOKS; i++){
    cook_id = malloc(sizeof(int));
    *cook_id = i; 
    pthread_create(&cook_threads[i], NULL, BENSCHILLIBOWLCook, cook_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell
  }
  
  for (i=0; i<NUM_CUSTOMERS; i++){ // create customers
    customer_id = malloc(sizeof(int)); 
    *customer_id = i;
    pthread_create(&customer_threads[i], NULL, BENSCHILLIBOWLCustomer, customer_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell    
  }
  
  for (i=0; i<NUM_CUSTOMERS; i++){ // wait for all customers to be done
    pthread_join(customer_threads[i], NULL); 
  }

  for (i=0; i<NUM_COOKS; i++){ // wait for all cooks to be done
    pthread_join(cook_threads[i], NULL); 
  } 

  CloseRestaurant(bcb);
  return 0;
}