#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

/*
$ ./ barrier
NTHRS =5
0 reached the barrier
1 reached the barrier
2 reached the barrier
3 reached the barrier
4 reached the barrier
4 passed the barrier
0 passed the barrier
1 passed the barrier
3 passed the barrier
2 passed the barrier
*/

// gcc bariera_L7.c -o bariera
// ./bariera


int NTHRS;

int threads_at_barrier;
pthread_mutex_t mutex;
sem_t sem;

void barrier_point() {
    // am nevoie de zona de memorie critica care sa restrictioneze accesul unui singur thread
    // counterul este incrementat cu fiecare thread care intra in zona critica

    pthread_mutex_lock(&mutex);
    threads_at_barrier++;
    if (threads_at_barrier == NTHRS) {
        //dam drumul threadurilor blocate la semafor
        for (int i = 1; i <= NTHRS; i++) {

// creste valoarea cu 1 si verifica daca sunt thread-uri blocate la semafor, 
// eliberandu-l pe cel care asteapta de cel mai mult timp
// acesta va relua executia din punctul in care a apelat sem_wait(adica trezeste threadul din asteptare)
            sem_post(&sem);
        }
        // resetez nr de threaduri care asteapta, fiindca ele s-au eliberat complet
        threads_at_barrier = 0;
    }
    pthread_mutex_unlock(&mutex);
    // odata ce un thread elibereaza mutexul 
    // permite unui alt thread sa intre in zona critica si sa incrementeze contorul
    // se intampla asta pana cand se atinge nr total de threaduri
    
    

    sem_wait(&sem);
    // scade valoarea din semafor cu 1 
    // si daca atinge 0 face return
    // la valori negative pune pe sleep/asteptare acel thread
    // -1 inseamna ca un thread este in asteptare :) si tot asa
    // sem_post va intra in relatie si va incrementa sem-ul eliiberand treptat threadurile
}



void *tfun(void *id_thread)
{
    int *tid = (int *) id_thread;
    printf ("%d reached the barrier\n", *tid );
    barrier_point();
    printf ("%d passed the barrier\n" , *tid );

    free(tid);
    return NULL;
}

int main() {
    printf("NTHRS = ");
    scanf("%d", &NTHRS);

// fac initializarea mutexului si a semaforului
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0);

// am vectorul de threaduri
    pthread_t thread[NTHRS];
    int i, *arg[NTHRS];

    //iau adresa de inceput a vectorului rg[NTHRS] si apoi aloc memorie de un size int
    // pt fiecare elem din acest vector si il initializez cu un i care e indexul threadului

    for(i = 0; i < NTHRS; i++){
        arg[i] = (int*) malloc(sizeof(int));
        *arg[i] = i;
// creez threadul odata cu lansarea functiei tfun ce ia ca argument indexul thredului in cauza
        pthread_create(&thread[i], NULL, tfun, arg[i]);
    
    }

// se asteapta finalizarea fiecarui thread
    for(i = 0; i < NTHRS; i++){
        if(pthread_join(thread[i], NULL)){
            perror("Eroare la asteptarea threadului.");
            return errno;
        };
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
    
    return 0;
}