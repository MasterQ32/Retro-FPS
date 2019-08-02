
// handle all player's states
void player_handle_state_machine(ENTITY *ent, CCT *cct){
	
	// if we are alive ?
	if(ent->obj_health > 0){
		
		// we are in water ?
		if(cct->water_state >= IN_WATER){
			
			// not inside of the water region ?
			if(cct->water_origin_diff_z < 25){
				
				ent->obj_state = SWIM;
			}
			else{
				
				ent->obj_state = DIVE;
			}
		}
		else{
			
			// not in pain or shoot state ?
			if(ent->obj_state != PAIN && ent->obj_state != SHOOT){
				
				// player jumped ?
				if(cct->is_grounded == false){
					
					// if in air for way too long, then switch to IN AIR state
					if(cct->falling_timer > 0.5){
						
						ent->obj_state = IN_AIR;
					}
					else{
						
						// jumped ?
						if(cct->jump_allowed == false){
							
							ent->obj_state = JUMPED;
						}
					}
				}
				else{
					
					// on ground
					if(cct->is_grounded == true){
						
						// landed ?
						if(cct->land_timer > 0.5){
							
							ent->obj_state = LANDED;
						}
						else{
							
							// if we are moving
							if(cct->is_moving == true){
								
								// handle WALK and RUN states
								// according to "always_run"
								if(cct->always_run == true){
									
									if(cct->run == true){
										
										ent->obj_state = WALK;
									}
									else{
										
										ent->obj_state = RUN;
									}
								}
								else{
									
									if(cct->run == true){
										
										ent->obj_state = RUN;
									}
									else{
										
										ent->obj_state = WALK;
									}
								}
							}
							else{
								
								ent->obj_state = IDLE;
							}
						}
					}					
				}
			}
		}
	}
	else{
		
		ent->obj_state = DEATH;
	}

}