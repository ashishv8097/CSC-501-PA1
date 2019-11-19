#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <math.h>

double pow(double x, int y)
{
	double result=1;
	
	while( y>0 )
	{
		if( (y%2) == 1 )
		{
			result = result * x;
		}
		x = x*x;
		y = y/2;
	}

	return result;
}

double log(double x)
{
        if( x>0 )
        {
                double y=1-x;
                double log_val=0;
                int i;

                for(i=1; i<21; i++)
                {
                        log_val = log_val + (pow(y,i)/i);
                }

                log_val = log_val * -1;

                return log_val;
        }
        else
        {
                return(SYSERR);
        }
}

double expdev(double lambda)
{
	double dummy;
	
	do
		dummy = (double) rand()/RAND_MAX;
	while (dummy == 0.0);
    
	return -log(dummy)/lambda;
}
