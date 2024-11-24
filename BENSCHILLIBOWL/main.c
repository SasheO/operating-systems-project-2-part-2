#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

/*
TODO: implement this as bounded buffer problem (slide 5). 
bcb can take 100 orders on the queue at a time 
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
// #define BENSCHILLIBOWL_SIZE 100
// #define NUM_CUSTOMERS 90
// #define NUM_COOKS 10
// #define ORDERS_PER_CUSTOMER 3
// #define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

#define BENSCHILLIBOWL_SIZE 10
#define NUM_CUSTOMERS 9
#define NUM_COOKS 3
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
    int orders_added = 0;
    Order *customer_orders;
    customer_orders = (Order*) malloc(sizeof(Order)*ORDERS_PER_CUSTOMER);
    int i;
    int is_added=false;
    for (i=0;i<ORDERS_PER_CUSTOMER;i++){
      customer_orders[i].menu_item = PickRandomMenuItem();
      customer_orders[i].customer_id = *customer_id;
      customer_orders[i].next = NULL;
    }

    // TODO: check if BENSCHILLIBOWL_SIZE not reached (isFull?) in terms of order before adding orders one after another here
    i = 0;
    while(i<ORDERS_PER_CUSTOMER){ // TODO: change the if to the mutex values
      is_added = false;
      pthread_mutex_lock(&bcb->mutex);
      if(AddOrder(bcb,&customer_orders[i])){
        i++;
      }
      pthread_mutex_unlock(&bcb->mutex); 
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
    printf("Cood %d reporting for duty\n", *cook_id);
	int orders_fulfilled = 0;
  while(true){
    if (!IsEmpty(bcb)){ // TODO: make this if statement mutex
      pthread_mutex_lock(&bcb->mutex);
      current_order_handling = GetOrder(bcb);
      printf("Cook %d handled order %d\n", *cook_id, current_order_handling->order_number);
      orders_fulfilled++;
      free(current_order_handling);
      current_order_handling = NULL;
      pthread_mutex_unlock(&bcb->mutex); 
    }
  }
  
    printf("! Cood %d\n", *cook_id);

	
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
  int customer_id;
  int i;
  int *cook_id;
	bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS); 
  // /*
  for (i=0; i<NUM_COOKS; i++){
    cook_id = malloc(sizeof(int));
    *cook_id = i;
    pthread_create(&cook_threads[i], NULL, BENSCHILLIBOWLCook, cook_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell
  }
  
  for (customer_id=0; customer_id<NUM_CUSTOMERS; customer_id++){
    pthread_create(&customer_threads[customer_id], NULL, BENSCHILLIBOWLCustomer, &customer_id); // pass targetCellAddrs[i] into given func such as computeSumCell to carry out the computation (sum) on the cell    
  }
  
  // for (cook_id=0; cook_id<NUM_COOKS; cook_id++){
  //   pthread_join(cook_threads[cook_id], NULL); // wait for thread to finish
  // }

  for (customer_id=0; customer_id<NUM_CUSTOMERS; customer_id++){
    pthread_join(customer_threads[customer_id], NULL); // wait for thread to finish
  }
  // */


  
	// Order *order1 = (Order*) malloc(sizeof(Order));
	// order1->menu_item = "Tacos";
  // order1->next = NULL;
  // Order *order2 = malloc(sizeof(Order));
	// order2->menu_item = "Fish";
  // order2->next = NULL;
  // IsEmpty(bcb);
	// AddOrder(bcb, order1);
	// AddOrder(bcb, order2);
  
	// PrintOrders(bcb);

  // CloseRestaurant(bcb);
 
  return 0;
}