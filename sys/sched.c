#include <conf.h>
#include <kernel.h>
#include <stdio.h>
#include <q.h>
#include <math.h>
#include <sched.h>
#include <proc.h>

int sched_flag;
int epoch;

void setschedclass(int sched_class)
{
	if(sched_class == 1)
	{
		sched_flag = EXPDISTSCHED;
	}
	else if(sched_class == 2)
	{
		sched_flag = LINUXSCHED;
		epoch = 0;

		int pid;
		for( pid=0 ; pid < NPROC ; pid++ )
		{
			ppage[pid].priority = proctab[pid].pprio;
			ppage[pid].counter = proctab[pid].pprio;
			ppage[pid].goodness = proctab[pid].pprio;

			if ( proctab[pid].pstate != PRFREE )
			{
				epoch = epoch + ppage[pid].priority;
				ppage[pid].runinepoch = 1;
			}
			else
			{
				ppage[pid].runinepoch = 0;
			}
		}
	}
	else
	{
		kprintf("\n\nInvalid Scheduling class given. Enter either 1 on 2.\n\n");
	}
}

int getschedclass(void)
{
	return sched_flag;
}

int expdev_process(void)
{
	int random_priority = expdev(LAMBDA);
	
	int current = q[rdytail].qprev;
        int prev = q[current].qprev;

	while( (random_priority < q[prev].qkey) && (prev != rdyhead) )
	{
		current = q[current].qprev;
		prev = q[current].qprev;
	}
	
	return current;
}

int linux_process(void)
{	
	int pid, max_good;
	int select = NPROC-1;
	max_good = ppage[select].goodness;

	for( pid=NPROC-2; pid > 0 ; pid-- )
	{
		if( (proctab[pid].pstate == PRREADY) && (ppage[pid].goodness > max_good) && (ppage[pid].runinepoch == 1) && (ppage[pid].counter > 0) )
		{
			max_good = ppage[pid].goodness;
			select = pid;
		}
	}
	return select;
}
