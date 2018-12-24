/*   
 *  This file provides a simple library for creating & scheduling user-level threads.               
 *  Author: Dmitriy Chudnovsky
 *  Student ID: 324793900
 */

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "ut.h"
#include "macro.h" 

static volatile tid_t current_index = zero;
static volatile tid_t number_of_treads = zero;
static volatile unsigned int table_size = zero;
static ucontext_t context;
static ut_slot_t  threads_table[MAX_TAB_SIZE];

/****************************************************************************************************************/

void handler(int signal) /* This is the signal handler which swaps between the threads demo2 + demo3 */ 
            {   
             if(SIGALRM == signal) 
               {
                alarm(Quantum);
                tid_t old_index = current_index ;
                tid_t new_index = (old_index + one) % number_of_treads;
                current_index = new_index;
                swapcontext(&threads_table[old_index].uc, &threads_table[new_index].uc);
               } 
             else if(SIGVTALRM == signal)
                    {
                     threads_table[current_index].vtime += update_vtime;
                    }
            }


/****************************************************************************************************************/

int init_SIG() /*like demo3*/
            { 

             struct sigaction sa;
             struct itimerval itv;

             sa.sa_flags = SA_RESTART;/* Initialize the data structures for SIGALRM handling. */
             sigfillset(&sa.sa_mask);
             sa.sa_handler = handler; 

             itv.it_interval.tv_sec = zero;/* set up vtimer for accounting */
             itv.it_interval.tv_usec = ten_th;
             itv.it_value = itv.it_interval;

             if(sigaction(SIGVTALRM, &sa, NULL)< zero ||
                setitimer(ITIMER_VIRTUAL, &itv, NULL) < zero ||
                sigaction(SIGALRM, &sa, NULL)< zero) /* Install the signal handler*/
               {
                ERR2;
                exit(one);
                return SYS_ERR;
               }
             return zero;
            }

/****************************************************************************************************************/

int ut_init(int new_size)/*Initialize the library data structures. Create the threads table.*/
           {
            int current_size = zero;
            if(init_SIG())
              {
               ERR3;
               return SYS_ERR;
              }
            table_size = set_num_of_threads(new_size,current_size);
            current_index = zero;
            return zero;
           }

/****************************************************************************************************************/

tid_t ut_spawn_thread(void(*func)(int),int arg)
                     {
                      tid_t index = number_of_treads;/* Increment spawn thread index */
                      if(table_size == zero)/* check thread table is valid */
                        {
                         ERR4;
                         return SYS_ERR;
                        }
                      if(index >= table_size) 
                        {
                         ERR5;
                         return TAB_FULL;
                        }
                      if(getcontext(&threads_table[index].uc) == minus)/* get and make thread context*/
                        {
                         ERR6;
                         return SYS_ERR;
                        }
                      char* stack = (char*)malloc(sizeof(char) * STACKSIZE);/* allocate stack for thread */
                      if(!stack)
                        {
                         ERR7;
                         return SYS_ERR;
                        }

                      threads_table[index].uc.uc_link = &context;/* init thread args like demo2*/
                      threads_table[index].uc.uc_stack.ss_sp = stack;
                      threads_table[index].uc.uc_stack.ss_size = (sizeof(char) * STACKSIZE);

    
    
                      threads_table[index].vtime = zero;
                      threads_table[index].func = func;
                      threads_table[index].arg = arg;

                      makecontext(&threads_table[index].uc, (void(*)(void))threads_table[index].func,
                                  one, threads_table[index].arg);
                      number_of_treads++;
                      return index;
                     }

/******************************************************************************************************************/

int ut_start(void)  /*check thread table is valid and that there are threads to run*/
            {
             if(table_size == zero || number_of_treads == zero)/*Check errors*/
               {
                ERR8;
                return SYS_ERR;
               }
             alarm(Quantum);/*begin running*/
             swapcontext(&context, &threads_table[zero].uc);
             return zero;
            }

/*******************************************************************************************************************/

unsigned long ut_get_vtime(tid_t tid)/*check thread is in thread table*/
                          {
                           if(table_size == zero || tid >= table_size || tid >= number_of_treads)/*Check errors*/
                             {
                              ERR9;
                              return SYS_ERR;
                             }
                           return threads_table[tid].vtime;/*acquire thread running time */
                          }

/*******************************************************END**********************************************************/
