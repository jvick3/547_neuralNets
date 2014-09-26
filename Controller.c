/*
 *  Controller.c
 *  For the UNM Neural Networks class, this should be the only file you will need to modify.
 *  World and agent initialization code are located in the main().  An
 *  example of a non-neural controller is included here.
 *  Note that most all of the functions called here can be found in the 
 *  file FlatworldIICore.c
 *  
 *
 * Taylor Berger and James Vickers, UNM CS 547, Fall 2014 project
 * Copyright 2009 UNM. All rights reserved.
 *
 */

#define ARCH_VERSION 0

int action = 0;
int lifetime = 0;

void agents_controller( WORLD_TYPE *w )
{ 
	
  if (action++ == 0)
  { printf("*************************************\n");
    printf("ARCHITECTURE VERSION: %d\n",ARCH_VERSION);
    printf("*************************************\n");
  }

  AGENT_TYPE *a ;
  
	a = w->agents[0] ; /* get agent pointer */
	
	/* test if agent is alive. If so, increment simtime and do nothing (sit still...) */
	if( a->instate->metabolic_charge > 0.0 )
	{  
	    simtime++;
            lifetime++;
    	}
    	else
    	{
	   printf("Agent died: simtime = %d, lifetime = %d\n",simtime, lifetime);
           exit(0);
    	}

        /* decrement metabolic charge by basil metabolism rate.  DO NOT REMOVE THIS CALL */
       basal_metabolism_agent( a ) ;	
  
}
