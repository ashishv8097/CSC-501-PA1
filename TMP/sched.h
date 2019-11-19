#ifndef SCHED_H
	#define SCHED_H
		#define EXPDISTSCHED 1
		#define LINUXSCHED 2

		void setschedclass(int sched_class);
		int getschedclass(void);
		int expdev_process(void);
		int linux_process(void);

		struct process_book
		{
        		int priority;
			int counter;
        		int goodness;
			int runinepoch;
		};

		extern int sched_flag;
		extern struct process_book ppage[];
		extern int epoch;
#endif
