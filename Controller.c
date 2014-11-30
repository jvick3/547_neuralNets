

const int ARCH_VERSION = 6;
const int MAX_TRIALS = 360;
int action = 0;
int lifespan = 0;
const double speed = 0.05;   
int trial = 0;
int lifespans[MAX_TRIALS];
int nsomareceptors, collision_flag = 0;
float x,y,h;
float** skinvalues, eyeValues;
float delta_energy;
FILE* time_metrics_file;
FILE* lifespan_metrics_file;
FILE* perceptron_metrics_file;
float heading = 0.0;
float start_heading = 5;
int total_food_eaten = 0, beneficial_food_eaten = 0, harmful_food_eaten = 0,
    neutral_food_eaten = 0, all_food_eaten = 0;
const int middle_eye = 15;
const double weights[3] = {-1.211087, 1.629145, -1.166483};

void agents_controller( WORLD_TYPE *w ) { 

  simtime++;
  
  // First call of this life, do some setup 
  if (action++ == 0) { 
    printf("*************************************\n");
    printf("ARCHITECTURE VERSION: %d\n",ARCH_VERSION);
    printf("*************************************\n");

    const int name_size = 30;
    char time_metrics_filename[name_size];
    char lifespan_metrics_filename[name_size]; 
    sprintf(time_metrics_filename, "arch_%d_time_metrics.txt", ARCH_VERSION);
    time_metrics_file = fopen(time_metrics_filename, "w");
    if (time_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",time_metrics_filename);

    sprintf(lifespan_metrics_filename, "arch_%d_lifespan_metrics.txt", ARCH_VERSION);
    lifespan_metrics_file = fopen(lifespan_metrics_filename, "w");
    if (lifespan_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",lifespan_metrics_filename);

    char perceptron_metrics_filename[name_size]; 
    sprintf(perceptron_metrics_filename, "arch_%d_perceptron_metrics.txt", ARCH_VERSION);
    perceptron_metrics_file = fopen(perceptron_metrics_filename, "w");
    if (perceptron_metrics_file == NULL)
      printf("ERROR: Could not open %s for metric data writing!\n",perceptron_metrics_filename);

  }

  AGENT_TYPE *a ;
  
  a = w->agents[0] ; /* get agent pointer */

  // Write current energy to time metrics file - regardless of if agent is alive
  fprintf(time_metrics_file, "%d %d %d %f\n", trial, lifespan, simtime, a->instate->metabolic_charge);
  
  /* test if agent is alive. If so, move in a straight line in the initial direction */
  if( a->instate->metabolic_charge > 0.0 ) {   

    lifespan++;

    // Read soma sensors 
    collision_flag = read_soma_sensor(w, a) ;
    skinvalues = extract_soma_receptor_values_pointer( a ) ;
    nsomareceptors = get_number_of_soma_receptors( a ) ;
    
    // Read visual sensors
    read_visual_sensor(w, a);

    // Change direction towards brightest object in view. 
    int max_vis_receptor = classified_winner_takes_all(a);
    if( max_vis_receptor >= 0 ) 
    {
	float max_direction = visual_receptor_position( a->instate->eyes[0], max_vis_receptor ) ;      
	heading += max_direction;
	set_agent_body_angle( a, heading);
    }
    else
     {
       heading += 10.0;
       set_agent_body_angle( a, heading );
     }
    
    if (collision_flag)
      {
	if (skinvalues[0][0] > 0.0 || skinvalues[1][0] > 0.0 || skinvalues[7][0] > 0.0)  
	  {    
	    // Find brightest eyelet
	    int max_bright_eye = intensity_winner_takes_all(a);
	    if (max_bright_eye >= 0)
	      { float red = a->instate->eyes[0]->values[max_bright_eye][0];
		float green = a->instate->eyes[0]->values[max_bright_eye][1];
		float blue = a->instate->eyes[0]->values[max_bright_eye][2];
	        		
		float v = (red * weights[0]) + (green * weights[1]) + (blue * weights[2]);
		if (v > 0)    // only eat if object is classified as good
		  {  
		    printf("eating\n");

		    int k;
		    if (skinvalues[0][0] > 0.0)
		      k = 0;
		    else if (skinvalues[1][0] > 0.0)
		      k = 1;
		    else
		      k = 7;
	    
		    delta_energy = eat_colliding_object( w, a, k) ;
		    printf("delta_energy = %f\n", delta_energy);

		    if (delta_energy == 0) 
		      neutral_food_eaten++;
		    else if (delta_energy < 0)
		      harmful_food_eaten++;
		    else
		      beneficial_food_eaten++;
                  
		    total_food_eaten++;
		    all_food_eaten++;
		  }
	      }
	  }
    }

    /****************************************************************************/

    /* move the agents body */
    set_forward_speed_agent( a, speed ) ;
    move_body_agent( a ) ;
    /* decrement metabolic charge by basil metabolism rate.
       DO NOT REMOVE THIS CALL */
    basal_metabolism_agent( a ) ;
  }
  else {
    printf("agent dead: trial = %d\n", trial);
    // Write lifespan metrics
    fprintf(lifespan_metrics_file, "%d %d %d %d %d\n", 
            lifespan, total_food_eaten, beneficial_food_eaten, 
            neutral_food_eaten, harmful_food_eaten);

    // Record and reset metrics
    lifespans[trial] = lifespan;
    lifespan = total_food_eaten = beneficial_food_eaten = 0;  
    neutral_food_eaten = harmful_food_eaten = 0;
    /**************************/                      

    // Remake agent in random orientation, reset flatworld condition        
    /* restore all of the objects back into the world */
    restore_objects_to_world( Flatworld ) ; 
    /* recharge the agent's battery to full */
    reset_agent_charge( a ) ;
    /* zero the number of object's eaten accumulator */
    a->instate->itemp[0] = 0 ;

    /* update start position and heading */
    x = 0; 
    y = 0;

    /* set new position and heading of agent */
    start_heading += 1.0;
    //printf("setting angle: heading = %f\n", start_heading);
    set_agent_body_position( a, x, y, start_heading);
    /*********************************************************/           
   
    trial++;
    // experiment complete, write and close out data files then exit
    if (trial >= MAX_TRIALS) { 
      printf("%d trials completed, exiting\n", trial-1);
      fclose(time_metrics_file); 
      fclose(lifespan_metrics_file);
      fclose(perceptron_metrics_file);
      exit(0);
    }
  }
}
 
