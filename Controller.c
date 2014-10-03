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


const int ARCH_VERSION = 1;
const int MAX_TRIALS = 1000;
int action = 0;
int lifetime = 0;
const double speed = 0.05;   // taken directly from the original version of this file.
int trial = 0;
int lifetimes[1000];
int nsomareceptors;
float x,y,h;
float** skinvalues;
float delta_energy;
FILE* time_metrics_file;
FILE* lifespan_metrics_file;

void agents_controller( WORLD_TYPE *w )
{ 

  simtime++;
	
  // First call of this life, do some setup 
  if (action++ == 0)
  { printf("*************************************\n");
    printf("ARCHITECTURE VERSION: %d\n",ARCH_VERSION);
    printf("*************************************\n");

    const int name_size = 30;
    char time_metrics_filename[name_size];
    char lifespan_metrics_filename[name_size]; 
    sprintf(time_metrics_filename, "arch_%d_time_metrics.txt", ARCH_VERSION);
    time_metrics_file = fopen(time_metrics_filename, "a+w");
    if (time_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",time_metrics_filename);

    sprintf(lifespan_metrics_filename, "arch_%d_lifespan_metrics.txt", ARCH_VERSION);
    lifespan_metrics_file = fopen(lifespan_metrics_filename, "a+w");
    if (lifespan_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",lifespan_metrics_filename);
  }

  AGENT_TYPE *a ;
  
	a = w->agents[0] ; /* get agent pointer */
	
	/* test if agent is alive. If so, move in a straight line in the initial direction */
	if( a->instate->metabolic_charge > 0.0 )
	{   lifetime++;

	    /* Eat object if detected */
	    skinvalues = extract_soma_receptor_values_pointer( a ) ;
	    nsomareceptors = get_number_of_soma_receptors( a ) ;
            int k;
	    for( k=0 ; k<nsomareceptors ; k++ )
	    {  
	       if( (k==0 || k==1 || k==7 ) && skinvalues[k][0] > 0.0 )
	       {
		  printf("eating something\n");
	 	  delta_energy = eat_colliding_object( w, a, k) ;
	       }
               
	    }

            /* move the agents body */
	    set_forward_speed_agent( a, speed ) ;
	    move_body_agent( a ) ;

	    /* decrement metabolic charge by basil metabolism rate.  DO NOT REMOVE THIS CALL */
	    basal_metabolism_agent( a ) ;
    	}
    	else
    	{
	   printf("Agent died: simtime = %d, lifetime = %d\n",simtime, lifetime);
           
           // Record and reset lifetime
           lifetimes[trial] = lifetime;
           lifetime = 0;  
           /**************************/           

           // TODO: Record and reset other metrics here 

           /**************************/           

           // Remake agent in random orientation, reset flatworld condition        
           restore_objects_to_world( Flatworld ) ;  /* restore all of the objects back into the world */
	   reset_agent_charge( a ) ;               /* recharge the agent's battery to full */
	   a->instate->itemp[0] = 0 ;              /* zero the number of object's eaten accumulator */
	   x = distributions_uniform( Flatworld->xmin, Flatworld->xmax ) ; /* pick random starting position and heading */
	   y = distributions_uniform( Flatworld->ymin, Flatworld->ymax ) ;
	   h = distributions_uniform( -179.0, 179.0) ;
	   printf("\nagent_controller- new coordinates after restoration:  x: %f y: %f h: %f\n",x,y,h) ;
	   set_agent_body_position( a, x, y, h ) ;    /* set new position and heading of agent */
           /*********************************************************/           
   
           trial++;
           if (trial >= MAX_TRIALS)  // experiment complete, write and close out data files then exit
	   {  printf("%d trials completed, exiting\n", trial-1);
	      // TODO: write time, lifespan metrics out to files
	      fclose(time_metrics_file); 
              fclose(lifespan_metrics_file);
              exit(0);
	   }
    	}

} 

