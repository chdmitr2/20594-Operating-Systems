/* 
 *  A binary semaphore
 *  Dmitriy Chudnovsky
 *  ID: 324793900
 */

#include <stdio.h>
#include <signal.h>
#include "binsem.h"
#include "macro.h"
#include "ut.h"
/************************************************************************/

void binsem_init(sem_t *s, int init_val)/*Initializes a binary semaphore*/
                {
	         *s = Check(init_val);
                }

/************************************************************************/

void binsem_up(sem_t *s)/*The Up operation*/
              {
	       *s = Unlock;
              }

/*************************************************************************/

int binsem_down(sem_t *s)/*The Down operation*/
               {
	        while(xchg(s,zero) == zero)
                     {
		      if(raise(SIGALRM) < zero )
                        {
			 ERR1;
			 return SYS_ERR;
		        }
	             }
	        return zero;
               }

/*********************************END*************************************/
