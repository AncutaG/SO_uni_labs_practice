#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


// gcc shm_collatz.c -o shm_collatz
// ./shm_collatz 9 16 25 36

//int shm_open ( const char * path , int flags , mode_t mode );
// path= am numele obiectului (diferit de open care are file path-ul/ numele file-ului)

int main (int argc, char* argv[]) {

    char shm_name[] = "shm_obj";

    int page_size = getpagesize(),
        // exclud executabilul din argumente
        n = argc - 1, 

        // dimensiunea alocata obiectului trebuie sa fie multiplu al dimensiunii paginii
        shm_size = n * page_size, 

        // cream obiectul de memorie partajata (daca el nue exista: O_CREAT) cu drepturile asociate de read si write (O_RDWR) la deschidere 
      
        // ooferind drepturi de citire si scriere pt owner si grupurile de useri, in rest ceilalti nu au niciun drept
        shm_fd = shm_open(shm_name, O_CREAT | O_RDWR , 0660);

// aloc dimensiune obiectului creat conform shm_size -ului
// daca eva nu este in regula sterg obiectul de memorie creat si afisez ceva explicit
   if ( ftruncate (shm_fd , shm_size ) == -1) 
        {
        perror ("Spatiul de memorie nu a putut fi alocat pentru obiectul creat. Obiectul este sters.");
        
        // stergerea obiectului in caz de error
        shm_unlink ( shm_name );
        return errno ;
    }


// memoria partajata se incarca in spatiul procesului

// per general sintaxa:
// void * mmap ( void * addr ,size_t len , int prot , int flags ,int fd , off_t offset );

// addr = NULL (las hernelul sa decida unde  unde sa incarce pe spatiuld de memorie)
// len = dimensiunea alocata obiectului
// ROT_READ | PROT_WRITE -> drepturi de scriere si citire depline din parinte

// MAP_SHARED e un tip de memorie care permite
// ca modificarile facute din parinte sa fie vazute si de catre alte procese

// shm_fd e file descriptorul obiectului de mem

//offset – locul in obiectul de memorie partajata (de la ce byte) 
// de la care sa se faca incarcarea in spatiul procesului

    char *shm_ptr = mmap( NULL, shm_size , PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // nu am dat offset aici fiindca impart in copii zona intreaga de memorie in functie de shm_ptr+offset

   /* if(shm_ptr == MAP_FAILED)
            {
                perror("Eroare la impartirea zonei de memorie alocate.\n");
                shm_unlink(shm_name); //sterge memoria creata cu shm_open, in caz de eroare
                return errno;
            }
            */

    printf("Parintele a inceput cu PID= %d\n", getpid());
    pid_t pid;

    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == 0) {
            // argumentul executabilului
            int  nr_curent = atoi(argv[i+1]);

            // fiecare copil tr sa aiba un offset alocat 
            // deci fiecare are bucata sa
            int offset = i * page_size;


            while (nr_curent > 1) {
                // aleg sprintf pentru a pune acel numar transformat, intr-un buffer per copil alocat
                // in acest mod fiecare copil are zona lui de memorie scrisa
                int written = sprintf(shm_ptr + offset,"%d ",nr_curent);

                // suprascriu offsetul cu cat copilul a scris
                // astfel incat sa iau in continuare urmatoarea bucata de memorie la urmatorul copil
                offset = offset + written;

                // am readaptat functia conditie_collatz 
                nr_curent = (nr_curent % 2 == 0) ? nr_curent / 2 : 3 * nr_curent + 1;
            }

            printf("Parintele s-a oprit %d Copilul a inceput cu PID= %d \n" , getppid(), getpid());

            
            //munmap(shm_ptr, shm_size);
            return 0;
        }
    }
    //parintele afiseaza rezultatele copiilor sai
    if (pid > 0) {
        // parintele trebuie sa astepte pana cand 
        // wait returneaza PID-ul copilului in cauza (atunci copilul a terminat)
        while (wait(NULL) > 0 );

     //apoi afisam numerele din bucatile de memorie per copil 
        for(int i = 0; i < n; i++) {
           // este primul nr de la care pornesc
            printf("%s: ",argv[i+1]);
            
            // ca sa stie sa mearga la urmatoare bucata de memorie cu urmat nr din sir
            int offset = i * page_size;

            // restul sirului  + ultimul nr pus separat
            printf("%s 1 \n",shm_ptr + i * page_size);
        }
        printf("Parintele parintelui cu PID= %d Parintele- copil cu PID= %d \n" , getppid(), getpid());
        
        // dezalocare, nemaiavand nevoie de zona de memorie din spatiul de mem al procoesului
        munmap(shm_ptr, shm_size);
        // sterg si obiectul
        shm_unlink(shm_name);
    }
}

/*
OBS:
mergea si cu alocare per copil in genul:

int* shm_ptr = mmap(0,getpagesize(), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, getpagesize()*i);
            if(shm_ptr == MAP_FAILED){
                perror(NULL);
                shm_unlink(shm_name);
                return errno;
            }

si apoi dezalocam cu:
munmap(0, getpagesize()*argc);
*/

