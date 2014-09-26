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

#define ARCH_VERSION 1
#define MAX_TRIALS = 1000;

int action = 0;
int lifetime = 0;
const int speed = 0.05;   // taken directly from the original version of this file.
int trial = 1;

void agents_controller( WORLD_TYPE *w )
{ 

  simtime++;
	
  // First call of this life, do some setup 
  if (action++ == 0)
  { printf("*************************************\n");
    printf("ARCHITECTURE VERSION: %d\n",ARCH_VERSION);
    printf("*************************************\n");
    
    char time_metrics_filename[15], lifespan_metrics_filename[15];
    snprintf(time_metrics_filename, sizeof(time_metrics_filename), "arch_%d_timestep.txt");
    FILE* time_metrics_file = fopen(time_metrics_filename, 'w');
    if (time_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",time_metrics_filename);

    snprintf(time_metrics_filename, sizeof(time_metrics_filename), "arch_%d_timestep.txt");
    FILE* lifespan_metrics_file = fopen(lifespan_metrics_filename, "a+w");
    if (lifespan_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",lifespan_metrics_filename);
  }

  AGENT_TYPE *a ;
  
	a = w->agents[0] ; /* get agent pointer */
	
	/* test if agent is alive. If so, move in a straight line in the initial direction */
	if( a->instate->metabolic_charge > 0.0 )
	{  
            lifetime++;
    	}
    	else
    	{
	   printf("Agent died: simtime = %d, lifetime = %d\n",simtime, lifetime);
           //TODO:  Reset lifetime, write data to file, remake agent in random orientation
           
           trial++;
           if (trial > MAX_TRIALS)
	   {  printf("%d trials completed, exiting\n", trial);
	      fclose(time_metrics_file); 
              fclose(lifespan_metrics_file);
              exit(0);
	   }
    	}

	/* move the agents body */
	set_forward_speed_agent( a, speed ) ;
	move_body_agent( a ) ;

	/* decrement metabolic charge by basil metabolism rate.  DO NOT REMOVE THIS CALL */
	basal_metabolism_agent( a ) ;

} 

