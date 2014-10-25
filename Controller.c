

const int ARCH_VERSION = 3;
const int MAX_TRIALS = 360;
int action = 0;
int lifespan = 0;
const double speed = 0.05;   // taken directly from the original version of this file.
int trial = 0;
int lifespans[360];
int nsomareceptors, collision_flag = 0;
float x,y,h;
float** skinvalues, eyeValues;
float delta_energy;
FILE* time_metrics_file;
FILE* lifespan_metrics_file;
FILE* perceptron_metrics_file;
float heading = -179.0;
int total_food_eaten = 0, beneficial_food_eaten = 0, harmful_food_eaten = 0,
    neutral_food_eaten = 0, all_food_eaten = 0;
const int middle_eye = 15;
double weights[] = {1.0, 1.0, 1.0};
const double learnRate = 0.01;
float sum_e_squared = 0;

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

    weights[0] = weights[1] = weights[2] = 0; // init weights for perceptron
  }

  AGENT_TYPE *a ;
  
  a = w->agents[0] ; /* get agent pointer */

  // Write current energy to time metrics file - regardless of if agent is alive
  fprintf(time_metrics_file, "%d %d %d %f\n", trial, lifespan, simtime, a->instate->metabolic_charge);
  
  /* test if agent is alive. If so, move in a straight line in the initial direction */
  if( a->instate->metabolic_charge > 0.0 ) {   

    lifespan++;

    /* Eat object if detected */
    collision_flag = read_soma_sensor(w, a) ;
    skinvalues = extract_soma_receptor_values_pointer( a ) ;
    nsomareceptors = get_number_of_soma_receptors( a ) ;
    int k;
    for( k=0 ; k < nsomareceptors ; k++ ) {   

      if(k == 7 && skinvalues[k][0] > 0.0 ) {
        // Read eyes
        read_visual_sensor(w, a);
        
        // this give us the receptor pointing at the closest 
        // item we can see. This corresponds to the element we 
        // are about to eat
        int max_receptor = intensity_winner_takes_all( a );
        if (max_receptor >= a->instate->eyes[0]->nreceptors || 
            max_receptor < 0)
          continue;
        float red = a->instate->eyes[0]->values[max_receptor][0];
        float green = a->instate->eyes[0]->values[max_receptor][1];
        float blue = a->instate->eyes[0]->values[max_receptor][2];

        //printf("eating:  r = %f, g = %f, b = %f\n", red, green, blue);
        delta_energy = eat_colliding_object( w, a, k) ;

        if (delta_energy == 0) 
          neutral_food_eaten++;
        else if (delta_energy < 0)
          harmful_food_eaten++;
        else
          beneficial_food_eaten++;
                  
        total_food_eaten++;
        all_food_eaten++;

        printf("Red: %f, Green: %f, Blue: %f\n",red, green, blue);
        printf("RW:  %f, GW   : %f, Bw  : %f\n", weights[0], weights[1], weights[0]); 
        float v = (red * weights[0]) + (green * weights[1]) + (blue * weights[2]);
        int desired = ((delta_energy > 0) ? 1 : 0);
        float error = desired - v;
        printf("Error: %f\n", error);
        weights[0] += (learnRate * error * red);
        weights[1] += (learnRate * error * green);
        weights[2] += (learnRate * error * blue);
        sum_e_squared += error * error;
      }
    }

    /* move the agents body */
    set_forward_speed_agent( a, speed ) ;
    move_body_agent( a ) ;
    /* decrement metabolic charge by basil metabolism rate.
       DO NOT REMOVE THIS CALL */
    basal_metabolism_agent( a ) ;
  }
  else {
    // Write lifespan metrics
    fprintf(lifespan_metrics_file, "%d %d %d %d %d\n", 
            lifespan, total_food_eaten, beneficial_food_eaten, 
            neutral_food_eaten, harmful_food_eaten);

    // Write e^2 error if it has eaten anything
    if (total_food_eaten != 0) { 
      fprintf(perceptron_metrics_file, "%d %f\n", 
              all_food_eaten, sqrt(sum_e_squared));
      sum_e_squared = 0;
    }

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

    /* pick random starting position and heading */
    x = 0; 
    y = 0;
    heading += 1.0;

    /* set new position and heading of agent */
    set_agent_body_position( a, x, y, heading);
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
 
