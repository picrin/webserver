#include <stdio.h>
#include <pthread.h>

pthread_mutex_t value_mutex = PTHREAD_MUTEX_INITIALIZER;
int value = 0;

const int number = 100000000;
int adder(){
  int i;
  for(i = 0; i < number; i++){
    //pthread_mutex_lock(&value_mutex);
    __sync_fetch_and_add(&value, 1);

    //value ++;
    //pthread_mutex_unlock(&value_mutex);
  }
}

int subtractor(){
  int i;
  for(i = 0; i < number; i++){
    //pthread_mutex_lock(&value_mutex);
    //value ++;
    __sync_fetch_and_add(&value, 1);

    //pthread_mutex_unlock(&value_mutex);

  }
}

int main(){
  pthread_t id0;
  pthread_t id1;
  pthread_create (
      &id0,
      NULL,
      (void*(*)())&adder,
      NULL); 
  pthread_create (
      &id1,
      NULL,
      (void*(*)())&subtractor,
      NULL); 
  pthread_join(id0, NULL);
  pthread_join(id1, NULL);
  printf("%d\n", (2* number - value));
}
