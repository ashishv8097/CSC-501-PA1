/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;

	if ( sched_flag == EXPDISTSCHED )
		insert(pid,rdyhead,pptr->pprio);
	else if ( sched_flag == LINUXSCHED )
		insert(pid,rdytail,pptr->pprio);	// some line
	else
		insert(pid,rdyhead,pptr->pprio);


	if (resch)
		resched();
	return(OK);
}
