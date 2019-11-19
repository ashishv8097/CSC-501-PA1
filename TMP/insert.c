/* insert.c  -  insert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <sched.h>

/*------------------------------------------------------------------------
 * insert.c  --  insert an process into a q list in key order
 *------------------------------------------------------------------------
 */
int insert(int proc, int head, int key)
{
	int next, prev;

	if(sched_flag == EXPDISTSCHED)		/*insert will be at tail*/
	{
		int select, next;
		select = head;
	
		while( q[q[select].qnext].qkey <= key ) 
		{
			select = q[select].qnext;
		}
		
		next = q[select].qnext;

		q[proc].qnext = next;
		q[proc].qprev = select;
		q[proc].qkey  = key;

		q[select].qnext = proc;
		q[next].qprev = proc;

		return OK;
	}
	else
	{
		int next, prev;			/* runs through list		*/
		next = q[head].qnext;
		while (q[next].qkey < key)	/* tail has maxint as key	*/
			next = q[next].qnext;
		q[proc].qnext = next;
		q[proc].qprev = prev = q[next].qprev;
		q[proc].qkey  = key;
		q[prev].qnext = proc;
		q[next].qprev = proc;
		return(OK);
	}
}
