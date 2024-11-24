#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

/*
bcb can take 100 customers at a time 
10 fixed threads for the cooks
customers come in, place one order after another.
	- can only come in when less than 100 indoors 
	- place one order after another
when a cook is free, they get the order.
	- reduce queue (needs mutex for synchronization)
	- increment orders represented (needs mutex for synchronization)
*/

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

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
    
    Order *customer_orders;
    customer_orders = (Order*) malloc(sizeof(Order)*ORDERS_PER_CUSTOMER);
    int i;
    for (i=0;i<ORDERS_PER_CUSTOMER;i++){
      customer_orders[i].menu_item = PickRandomMenuItem();
      customer_orders[i].customer_id = *customer_id;
      customer_orders[i].next = NULL;
    }

    // TODO: check if BENSCHILLIBOWL_SIZE not reached (isFull?) in terms of order before adding orders one after another here
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
    int* cook_id = (int*)(long) tid;
	int orders_fulfilled = 0;
  // TODO: check if any orders to take. take it (lock it?). increment orders taken.
	printf("Cook #%d fulfilled %d orders\n", *cook_id, orders_fulfilled);
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
  int customer_id, cook_id;
	bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS); 
  for (cook_id=0; cook_id<NUM_COOKS; cook_id++){
    pthread_create(&cook_threads[cook_id], NULL, BENSCHILLIBOWLCook, &cook_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell
  }
  
  for (customer_id=0; customer_id<NUM_CUSTOMERS; customer_id++){
    pthread_create(&customer_threads[customer_id], NULL, BENSCHILLIBOWLCustomer, &customer_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell    
  }
  
  for (cook_id=0; cook_id<NUM_COOKS; cook_id++){
    pthread_join(cook_threads[cook_id], NULL); // wait for thread to finish
  }

  for (customer_id=0; customer_id<NUM_CUSTOMERS; customer_id++){
    pthread_join(customer_threads[customer_id], NULL); // wait for thread to finish
  }


  // TODO check if any free cook?
    // pthread_join(threads[thread_indx], NULL); // wait for thread to finish

	// printf("bcb->max_size: %d\n", bcb->max_size);
	// printf("bcb->expected_num_orders: %d\n", bcb->expected_num_orders);
	Order *order1 = (Order*) malloc(sizeof(Order));
	order1->menu_item = "Tacos";
  order1->next = NULL;
  Order *order2 = malloc(sizeof(Order));
	order2->menu_item = "Fish";
  order2->next = NULL;
	AddOrder(bcb, order1);
	AddOrder(bcb, order2);
	PrintOrders(bcb);

  CloseRestaurant(bcb);
 
  return 0;
}