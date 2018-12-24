#define GTEST_DONT_DEFINE_FAIL 1
#include <gtest/gtest.h>

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <sys/time.h>

extern "C" {
#include "binsem.h"
#include "ut.h"
}

int gWasSignalHandlerCalled = 0;
int gWasAlarmCalled = 0;
int gWasTimerCalled = 0;

/**************************************************
 *			mock system calls for tests			  *
 **************************************************/
int setitimer (__itimer_which_t __which, const struct itimerval *__restrict __new, struct itimerval *__restrict __old) {
	gWasTimerCalled++;
	return 0;
};

int swapcontext (ucontext_t *__restrict __oucp,const ucontext_t *__restrict __ucp) {
	return 0;
}

unsigned int alarm (unsigned int __seconds) {
	gWasAlarmCalled++;
	return 0;
}

/**************************************************/


void handler(int signal) {
	if (signal == SIGALRM) {
		gWasSignalHandlerCalled = 1;
	}
}

sem_t g_sem;
//sigalarm handler for semaphore down tests
//release the semaphore if alaram signal is caught
void sem_handler(int signal) {
	if (signal == SIGALRM) {
		gWasSignalHandlerCalled = 1;
		binsem_up(&g_sem);
	}
}

void *stall(int val) {
	while(true) {
		printf("%d\n",val);
	}
	return NULL;
}


/*****************************************************************************/
/*                       Binsim library tests                                */
/*****************************************************************************/

TEST(BinsimTest, binsem_init) {
	sem_t sem;
	// init to 1
	binsem_init(&sem, 1);
	EXPECT_EQ((sem_t ) 1, sem);

	// init to smth grater then 1; make sure remains 1
	binsem_init(&sem, 123456);
	EXPECT_EQ((sem_t ) 1, sem);

	// init to zero
	binsem_init(&sem, 0);
	EXPECT_EQ((sem_t ) 0, sem);
}

TEST(BinsimTest, binsem_up) {
	sem_t sem;

	// init to 1; up; make sure remains 1
	binsem_init(&sem, 1);
	binsem_up(&sem);
	EXPECT_EQ((sem_t )1, sem);

	// init to 1; up twice; make sure remains 1
	binsem_init(&sem, 1);
	binsem_up(&sem);
	binsem_up(&sem);
	EXPECT_EQ((sem_t )1, sem);

	// init to 0; up twice; make sure turns 1
	binsem_init(&sem, 0);
	binsem_up(&sem);
	binsem_up(&sem);
	EXPECT_EQ((sem_t )1, sem);

}

TEST(BinsimTest, binsem_down) {
	//register for alarm signal
	//this is required since second down will halt until semaphore value increases
	struct sigaction sa;
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);
	sa.sa_handler = sem_handler;
	EXPECT_EQ(0, sigaction(SIGALRM, &sa, NULL));

	int result;

	gWasSignalHandlerCalled = 0;

	// test first down 
	binsem_init(&g_sem, 1);
	result = binsem_down(&g_sem);
	EXPECT_EQ(0, result);
	EXPECT_EQ(0, gWasSignalHandlerCalled);

	// test second down 
	result = binsem_down(&g_sem);
	EXPECT_EQ(0, result);
	EXPECT_EQ(1, gWasSignalHandlerCalled);

}

/*****************************************************************************/
/*                       ut library tests                                    */
/*****************************************************************************/

TEST(UtTest, ut_init) {

	//init table size to minimum, check allows to add exactly MIN_TAB_SIZE threads
	int num=MIN_TAB_SIZE;
	EXPECT_EQ(0, ut_init(num));
	for (int i=0;i<num;i++)
		EXPECT_GE(ut_spawn_thread((void(*)(int))stall,0),0);
	EXPECT_EQ(TAB_FULL, ut_spawn_thread((void(*)(int))stall,0));

	//init table size to maximum, check allows to add exactly MAX_TAB_SIZE threads
	num=MAX_TAB_SIZE;
	EXPECT_EQ(0, ut_init(num));
	for (int i=0;i<num;i++)
		EXPECT_GE(ut_spawn_thread((void(*)(int))stall,0),0);
	EXPECT_EQ(TAB_FULL, ut_spawn_thread((void(*)(int))stall,0));

	//init table size to 0, check allows to add exactly MAX_TAB_SIZE threads
	EXPECT_EQ(0, ut_init(0));
	for (int i=0;i<MAX_TAB_SIZE;i++)
		EXPECT_GE(ut_spawn_thread((void(*)(int))stall,0),0);
	EXPECT_EQ(TAB_FULL, ut_spawn_thread((void(*)(int))stall,0));

	//init table size to more the MAX_TAB_SIZE, check allows to add exactly MAX_TAB_SIZE threads
	EXPECT_EQ(0, ut_init(MAX_TAB_SIZE+1));
	for (int i=0;i<MAX_TAB_SIZE;i++)
		EXPECT_GE(ut_spawn_thread((void(*)(int))stall,0),0);
	EXPECT_EQ(TAB_FULL, ut_spawn_thread((void(*)(int))stall,0));
}

TEST(UtTest, ut_spawn_thread) {
	gWasTimerCalled=0;
	gWasAlarmCalled=0;
	
	//check thread tid values - each call to add thread should return the following tid value
	EXPECT_EQ(0, ut_init(3));
	EXPECT_EQ(0, ut_spawn_thread((void(*)(int))stall,0));
	EXPECT_EQ(1, ut_spawn_thread((void(*)(int))stall,1));
	EXPECT_EQ(2, ut_spawn_thread((void(*)(int))stall,2));
	//make sure there are no calls to alarm() and setitimer() - meaning threads are not started yet
	EXPECT_EQ(0, gWasAlarmCalled);
	EXPECT_EQ(0, gWasTimerCalled);
}

TEST(UtTest, ut_start) {
	gWasTimerCalled=0;
	gWasAlarmCalled=0;
	
	//call ut_start without adding threads, expect SYS_ERR. check no timers are called
	EXPECT_EQ(0, ut_init(3));
	EXPECT_EQ(SYS_ERR, ut_start());
	EXPECT_EQ(0, gWasAlarmCalled);
	EXPECT_EQ(0, gWasTimerCalled);

	//check normal start - make sure ut_start calls alarm() and setitimer()
	EXPECT_EQ(0, ut_init(1));
	EXPECT_EQ(0, ut_spawn_thread((void(*)(int))stall,0));
	EXPECT_EQ(0, ut_start());
	EXPECT_EQ(1, gWasAlarmCalled);
	EXPECT_EQ(1, gWasTimerCalled);
}

TEST(UtTest, ut_get_vtime) {
	gWasAlarmCalled = 0;
	gWasTimerCalled = 0;

	//init 2 threads
	ut_init(2);
	ut_spawn_thread((void(*)(int))stall,0);
	ut_spawn_thread((void(*)(int))stall,0);
	ut_start();
	EXPECT_EQ(1, gWasAlarmCalled);
	EXPECT_EQ(1, gWasTimerCalled);

	//since we override alram() and setitimer() functions, we need to raise the signals manually
	//this test assumes the code has registered for SIGALRM and SIGVTALRM and check it handles them correctly
	
	//signal VTALRM to first thread three times (as if it was running for 300 ms)
	raise(SIGVTALRM);
	raise(SIGVTALRM);
	raise(SIGVTALRM);

	//swap thread - check that while handling the signal alarm is requested again (for next alarm)
	raise(SIGALRM);
	EXPECT_EQ(2, gWasAlarmCalled);

	//signal to second thread 2 times (as if it was running for 200 ms)
	raise(SIGVTALRM);
	raise(SIGVTALRM);

	EXPECT_EQ(300,ut_get_vtime(0));
	EXPECT_EQ(200,ut_get_vtime(1));

	//swap thread - check that while handling the signal alarm is requested again (for next alarm)
	raise(SIGALRM);
	EXPECT_EQ(3, gWasAlarmCalled);

	//signal to first thread twice (as if it was running for another 200 ms)
	raise(SIGVTALRM);
	raise(SIGVTALRM);
	EXPECT_EQ(500,ut_get_vtime(0));
	EXPECT_EQ(200,ut_get_vtime(1));
}

/*****************************************************************************/
/*                               main                                        */
/*****************************************************************************/

GTEST_API_ int main(int argc, char **argv) {
	printf("Running main() from gtest_main.cc\n");
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

/*****************************************************************************/
/*                               EOF                                         */
/*****************************************************************************/


