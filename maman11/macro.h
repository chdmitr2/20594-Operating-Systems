/*
* Chudnovsky Dmitriy
* ID:324793900
*/

#ifndef MACRO_H
   #define MACRO_H
#include "ut.h"
enum{Lock,Unlock};
enum{zero,one};
#define Check(A) ((A) == Lock ? Lock : Unlock)
#define minus -1
#define ten_th 10000
#define set_num_of_threads(B,C) ((B) >= MIN_TAB_SIZE && (B) <= MAX_TAB_SIZE ? (C) == (B) : (C) == MAX_TAB_SIZE)
#define Quantum 1
#define update_vtime 10
#define ERR1 perror("Sends the signal failed")
#define ERR2 perror("Error on signal handler installation")
#define ERR3 perror("Error on signal handler initialization")
#define ERR4 perror("Thread table size is 0")
#define ERR5 perror("Thread table size is full")
#define ERR6 perror("Error occurred on context fetching")
#define ERR7 perror("Error occurred on allocating stack")
#define ERR8 perror("No threads in thread table to run")
#define ERR9 perror("There is no such thread in thread table")


/* This is the signal handler which swaps between the threads,
like in demo2 + demo3 */
void handler(int signal); 

/* this function initialize the data structures for SIGALRM handling,
       set up vtimer for accounting and install the signal handler */
int  init_SIG();

#endif /* MACRO_H */
