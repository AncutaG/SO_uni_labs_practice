
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/*
24 29 34 
69 80 91 
150 170 190 
*/


// imi definesc global nr de linii si coloane
#define m 2
#define p 3
#define n 3


// imi initializez matricele
int matr_1[m][p] = {
        {1,2,3},
        {4,5,6}
        //{10,10,10}
};

int matr_2[p][n] = {
        {10,10,10},
        {1,2,3},
        {4,5,6}
       
};

int matrice_rezultata[m][n];

struct pos{
    int i, j;
};

// void *(* start_routine )( void *) ,
void* matr_inmultire(void* position){
    struct pos* index = position;
    int i = index->i;
    int j = index->j;

    // dezaloc memoria pentru structura
    free(position);

    matrice_rezultata[i][j] = 0;

// c_2 tr sa coincida cu r_1, altfel nu vom putea inmulti 2 matrice
    for(int k=0;k<p;++k)
        matrice_rezultata[i][j] += matr_1[i][k]*matr_2[k][j];

    return NULL;
}

int main(){
    // am un vector de thread-uri creat in functie de dimensiunea matricei rezultate care este r * c
    pthread_t threads[m * n];

    int thread_id = 0;

// initializeaz˘a pointer-ul thread cu noul fir de executie 
// lansat prin apelarea functiei start routine cu argumentele oferite de arg.

// int pthread_create (  pthread_t * thread , 
//                       const pthread_attr_t * attr ,
//                       void *(* start_routine )( void *) ,
//                       void * arg );

    for(int i=0;i<m;++i)
        for(int j=0;j<n;++j){
            
            // alocam memorie pentru un struct pos, in cadrul structurii 
            // care mai departe ne va da pozitia unui elem ca linie si coloana
            struct pos* index = malloc(sizeof(struct pos));
          
            index->i = i;
            index->j = j;

            // creez thread-uri succesive, prin vectorul de threaduri odata cu executia functiei de inmultire
            if(pthread_create(&threads[thread_id++], NULL, matr_inmultire, index)){
                perror("Nu a putut fi creat vectorul de thread-uri la apelul functiei.");
                return errno;
            }
        }
//se astepta succesiv finalizarea executiei fiecarui thread din vectorul de threaduri
// astfel incat la final sa am 

//int pthread_join ( pthread_t thread , void ** value_ptr );
// in acest caz value_ptr  este null (deci nu voi regasi returnul din functie in value_ptr)
//si nu am de ce sa pun dupa finalizarea executiei functiei intr-un value_ptr fiindca am matricea rezultata modificata
    for(int i=0;i<thread_id;++i){
        // daca pthread_join nu s- incheiat cu succes
        if(pthread_join(threads[i], NULL)){
            return errno;
        }
        printf("Asteptare thread: %d\n", i);
    }

    // afisare matrice 
    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j)
            printf("%d ", matrice_rezultata[i][j]);
        printf("\n");
    }
    return 0;
}