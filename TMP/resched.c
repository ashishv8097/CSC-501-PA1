/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
struct process_book ppage[NPROC];
int epoch;
/*-----------------------------------------------------------------------
 *  * resched  --  reschedule processor to highest priority ready process
 *   *
 *    * Notes:	Upon entry, currpid gives current process id.
 *     *		Proctab[currpid].pstate gives correct NEXT state for
 *      *			current process if other than PRREADY.
 *       *------------------------------------------------------------------------
 *        */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int preempt_val;

	if( sched_flag == EXPDISTSCHED )
	{		
		optr = &proctab[currpid];
		
		if(optr->pstate == PRCURR)
                {
                        optr->pstate = PRREADY;
                        insert(currpid,rdytail,optr->pprio);
                }
		
		int next_process = expdev_process(); // LAMDA is defined in math.h

		nptr = &proctab[ (currpid = dequeue(next_process)) ];
		preempt_val = QUANTUM;
	}
	else if(sched_flag == LINUXSCHED )
	{
		int time_consumed = ppage[currpid].counter - (preempt<0 ? 0 : preempt);
		epoch = epoch - time_consumed; /* reduce epoch by the time a process has run*/
		ppage[currpid].counter = preempt<0 ? 0 : preempt;  /*set counter to value of preempt*/

		//determining goodness value
		if(ppage[currpid].counter <= 0)
			ppage[currpid].goodness = 0;
		else
			ppage[currpid].goodness = ppage[currpid].counter + ppage[currpid].priority;

		optr = &proctab[currpid];
                if(optr->pstate == PRCURR)
                {
                        optr->pstate = PRREADY;
                        insert(currpid, rdyhead, ppage[currpid].goodness); /*Push the process in queue*/
		}
		
		/*checking if any process is reamianing to execute*/
		int goodness_sum=0, pid;
		for( pid=0 ; pid<NPROC ; pid++ )
			if( (proctab[pid].pstate == PRREADY) && (ppage[pid].runinepoch == 1) )
				goodness_sum = goodness_sum + ppage[pid].goodness;
	
		/* when epoch is 0 or remaining time is 0 */
		if( (epoch <= 0) || (goodness_sum <= 0) )
		{
			epoch = 0;
			for( pid=0; pid<NPROC; pid++ )
                        {
                                if ( proctab[pid].pstate != PRFREE )
                                {
					ppage[pid].priority = proctab[pid].pprio;
                                        epoch = epoch + ppage[pid].priority;
					if( ppage[pid].runinepoch == 1 )
					{	
						ppage[pid].goodness = ppage[pid].counter + ppage[pid].priority;
						ppage[pid].counter = (ppage[pid].counter/2) + ppage[pid].priority;
					}
					else
					{
						ppage[pid].goodness = ppage[pid].priority;
                                                ppage[pid].counter = ppage[pid].priority;
					}

					ppage[pid].runinepoch = 1;
				}
				else
					ppage[pid].runinepoch = 0;
                        }
		}
		nptr = &proctab[ (currpid = dequeue(linux_process())) ];
                preempt_val = ppage[currpid].counter;
	}
	else
	{
		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR ) && (lastkey(rdytail) < optr->pprio) )
			return(OK);
	
		/* force context switch */
		if (optr->pstate == PRCURR) 
		{
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		preempt_val = QUANTUM;
	}

	nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = preempt_val;		/* reset preemption counter	*/
	#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

