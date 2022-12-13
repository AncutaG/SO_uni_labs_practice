#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

/*
./ strrev hello
olleh
*/

// gcc str_rev_L6.c -o str_rev -pthread
// /str_rev Aleluia

// functia de oglindit care va lansa thread-ul
// void *(* start_routine )( void *)


void *my_oglindit(void *sir) 
{
    // am cele doua siruri
    char* s_input=(char *)sir;
    // am facut o alocare cu cast la tipul care trebuie returnat
    char* oglindit=(char*)malloc(strlen(s_input));

	for(int i=strlen(s_input)-1; i>=0; i--) 
        oglindit[strlen(s_input)-1-i] = s_input[i];
	return oglindit;
}

// sau  cu char** argv
int main(int argc, char *argv[])
{
    char *result;

 // am variabila pentru  thread
    pthread_t thr;

 // am un pointer la o functie
 // si NULL pus implicit,  pt atributele implicit setate de sistemul de operare

    if(pthread_create(&thr, NULL, my_oglindit, argv[1]))
    //se creaza un thread nou - initializeaza thr cu noul fir de executie l
    // lansez firul de executie cu functia "my_oglindit", dand argumentul argv[1] adica stringul ce trebuie oglindit
    {
        perror("Thread-ul nu a putut fi creat. \n");
        return errno;
    }

   
    // int pthread_join ( pthread_t thread , void ** value_ptr );
    // (void **) face un cast de tip din char** in void**

    //Asteapta finalizarea executiei thread-ului din variabila thr 
    // si pune ceea ce functia a returnat la exit, in adresa de memorie data prin result
    if(pthread_join(thr, (void **)&result))
    {
		perror("Adresa de memorie pentru a pune rezultatul din thread este nula. \n");
		return 0;
	}

	printf("%s\n", result);

    //dezaloc memoria de la malloc
	free(result); 

    return 0;
}