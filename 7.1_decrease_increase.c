#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAXRESOURCES 5
#define MAXTHREADS 6

int available_resources = MAXRESOURCES;

pthread_mutex_t mtx; 

int decrease_count(int count_resurse) 
{
    // Doamne... asta era lipsa (cu acelasi thread) si nu intelegeam
    //deci daca nu are resurse  threadul actual, el deblocheaza mutexul
    //si permite altui thread sa intre in zona critica si sa elibereze resurse

    while(!pthread_mutex_lock(&mtx) && available_resources < count_resurse)
    {
        pthread_mutex_unlock(&mtx); 
      
    }

    available_resources -= count_resurse;
    printf("Am folosit %d resurse, mai sunt %d ramase\n", count_resurse, available_resources);
    pthread_mutex_unlock(&mtx);

    return 0;
}

int increase_count(int count_resurse) 
{
    pthread_mutex_lock(&mtx); 
    available_resources += count_resurse ;
    printf("Am eliberat %d resurse, mai sunt %d ramase\n", count_resurse, available_resources);
    pthread_mutex_unlock(&mtx);

    return 0;
}

void *my_thread(void *arg) {
    int count = *((int *) arg);
    decrease_count(count);
    increase_count(count);

    return NULL;
}

int main()
{
    if(pthread_mutex_init(&mtx, NULL)) 
    {
          perror("Eroare creare mutex\n");
          return errno ;
    }

    printf("MAX_RESOURCES = %d\n", available_resources);

    int count_resurse;
    pthread_t *thread = malloc(MAXTHREADS * sizeof(pthread_t)); 
    
    // de fapt nu decid cui sa aloc o anume resursa sau sa eliberez
    //  ci fiecare thread primeste/elibereaza un nr de resurse random
    // si aici pricepusem incomplet
    srand(time(0));
    for(int i = 0; i < MAXTHREADS; ++i)
    {
        count_resurse = rand()%(MAXRESOURCES + 1);
        if(pthread_create(&thread[i], NULL, my_thread, &count_resurse))
        {
            perror("Nu s-a putut crea thread-ul\n");
            return errno;
        }
    }

    for(int i = 0; i < MAXTHREADS; ++i)
    {
        if(pthread_join(thread[i], NULL)) 
        {
            perror("Eroare in asteptarea finalizarii executiei. \n");
            return errno;
        }
    }

    pthread_mutex_destroy(&mtx); 
    
    return 0;
}
