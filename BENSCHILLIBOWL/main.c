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

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
  // TODO: create thread 
    int customer_id = (int)(long) tid;
    Order *customer_orders[ORDERS_PER_CUSTOMER];
    int i;
    for (i=0;i<ORDERS_PER_CUSTOMER;i++){
      customer_orders[i]->menu_item = PickRandomMenuItem();
      customer_orders[i]->customer_id = customer_id;
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
    int cook_id = (int)(long) tid;
	int orders_fulfilled = 0;
	printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
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
	bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS); 
	printf("bcb->max_size: %d\n", bcb->max_size);
	printf("bcb->expected_num_orders: %d\n", bcb->expected_num_orders);
	Order *order = malloc(sizeof(Order));
	order->menu_item = "Tacos";
	AddOrder(bcb, order);
	order = malloc(sizeof(Order));
	order->menu_item = "Fish";
	AddOrder(bcb, order);
	order = GetOrder(bcb);
	PrintOrders(bcb);
	printf("%s\n", order->menu_item);
	free(bcb); // TODO: free bcb, free orders and all other dynamically allocated memory
  return 0;
}