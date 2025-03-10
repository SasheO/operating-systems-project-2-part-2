#include "BENSCHILLIBOWL.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h> 

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int menu_item_index = rand()%BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[menu_item_index];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    printf("Restaurant is open!\n");
    BENSCHILLIBOWL *bcb = malloc(sizeof(BENSCHILLIBOWL));
    if (bcb == NULL) {
      perror("Memory not allocated.\n");
      exit(1);
    }
    bcb->max_size=max_size;
    bcb->orders = NULL;
    bcb->expected_num_orders=expected_num_orders;
    bcb->orders_handled=0;
    bcb->current_size=0;
    bcb->next_order_number=1;
    if (pthread_mutex_init(&bcb->mutex, NULL) != 0) { 
        perror("mutex init has failed\n"); 
      exit(1);
    } 
    if (pthread_cond_init(&bcb->can_add_orders, NULL) != 0 ||pthread_cond_init(&bcb->can_get_orders, NULL) != 0){
      perror("pthread_cond init has failed\n"); 
      exit(1);
    }
    return bcb;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  if (!WorkdayIsOver(bcb)){ // only close restaurant if all orders are fulfilled i.e. WorkdayIsOver
    return;
  }

  // deallocate all resources (orders queue, mutex variables, bcb)
  Order *curr = bcb->orders;
  Order *next;
  while (curr != NULL){
    next = curr->next;
    free(curr);
    curr = NULL;
    curr = next;
  }
  bcb->orders = NULL;
  // destroy mutex variables
  while (pthread_mutex_destroy(&bcb->mutex)!=0){
    perror("Issue freeing resources");
  }
  while (pthread_cond_destroy(&bcb->can_add_orders)!=0){
    perror("Issue freeing resources");
    
  }
  while (pthread_cond_destroy(&bcb->can_get_orders)!=0){
    perror("Issue freeing resources");
    
  }
  free(bcb);
  bcb = NULL;  
  printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
bool AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
  /*
  return true if successfully added, false if not
  */

  Order * curr = bcb->orders;
  if (curr == NULL){ // queue is empty. add to the head
    order->order_number = bcb->next_order_number;
    bcb->next_order_number++;
    bcb->orders = order;
    bcb->current_size++;
    return true;
  }

  while (curr->next!=NULL){ // queue isn't empty so iterate to the last one and add it at the end
    curr = curr->next;
  }
  order->order_number = bcb->next_order_number;
  bcb->next_order_number++;
  curr->next = order;
  bcb->current_size++;
  return true;
}

/* remove an order from the queue and return it or return NULL if queue is already empty */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
  Order *curr = bcb->orders;
  if (curr==NULL){ // edge case where list is empty
    return NULL;
  }
  else{
    bcb->current_size--;
    bcb->orders = curr->next;
    curr->next = NULL;
    return curr;
  }

}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size==0){
    return true;
  }
  return false;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size>=bcb->max_size){
    return true;
  }
  return false;
}

bool WorkdayIsOver(BENSCHILLIBOWL* bcb){
  if (bcb->orders_handled>=bcb->expected_num_orders){
    return true;
  }
  else{
    return false;
  }
}

/* prints orders in order list for testing purposes */
void PrintOrders(BENSCHILLIBOWL* bcb){
  Order * curr = bcb->orders;
  while (curr!=NULL){
    printf("order #%d: %s\n", curr->order_number, curr->menu_item);
    curr = curr->next;
  }
}