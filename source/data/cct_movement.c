
// perform gravity trace for the given entity with the given cct
void ent_gravity_trace(ENTITY *ent, CCT *cct){
	
	if(!ent){
		diag("\nERROR! Can't create gravity trace for given entity, it doesn't exist!");
		return;
	}
	
	// start and end positions of the gravity trace
	VECTOR from, to, target_vec;
	vec_set(&from, vector(ent->x, ent->y, ent->z + cct_grav_trace_start_offset));
	vec_set(&to, &from);
	to.z -= cct_grav_trace_end_offset + cct->foot_height;
	
	// perform gravity trace
	vec_set(&ent->min_x, vector(-(cct->bbox_x - 1), -(cct->bbox_y - 1), -0.1));
	vec_set(&ent->max_x, vector((cct->bbox_x - 1), (cct->bbox_y - 1), 0.1));
	c_ignore(PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
	c_trace(&from, &to, TRACE_FLAGS | IGNORE_PUSH | SCAN_TEXTURE | USE_BOX);
	vec_set(&ent->min_x, vector(-cct->bbox_x, -cct->bbox_y, -0.1));
	vec_set(&ent->max_x, vector(cct->bbox_x, cct->bbox_y, cct->bbox_z));
	
	vec_fill(&cct->surface_normal, 0);
	vec_fill(&cct->surface_speed, 0);
	
	vec_set(&target_vec, &to);
	
	cct->ground_info = SOLID;
	
	if(HIT_TARGET){
		
		vec_set(&target_vec, &target);
		vec_set(&cct->surface_normal, &normal);
		
		// level block
		if(you == NULL){
			
			// we are on invisible slope ? 
			// (which is used for stairs, so no sliding on stairs)
			if(tex_flag1){ cct->ground_info = STAIRS; }
		}
		
		// models
		if(you != NULL){
			
			// if this entity can move us
			if(your->obj_move_npc == true){
				
				// then add it's XY speed to our movement
				// TODO !
				// here we need to get structure of YOU entity
				// then we need to take it's velocity and add it to our
				// surface speed !
				// cct->surface_speed.x = your->speed_x;
				// cct->surface_speed.y = your->speed_y;
				// Z speed is not necessary - this is done by the height adaption
			}
			
			if(your->obj_type == TYPE_ELEVATOR || your->obj_type == TYPE_PLATFORM){
				
				// TODO !
				// here we need to get structure of YOU entity
				// then we need to get it's current state and if it's MOVING
				// then we need to set our ground info to MOVING as well
				// this could be used f.e. to shake player's camera
				/*if(your->obj_state == 1 || your->obj_state == 2){
					cct->ground_info = MOVING;
				}*/
			}
		}
	}
	
	cct->soil_height = target_vec.z - cct->foot_height;
}

// calculate the damage from a fall and return that value:
function ent_fall_damage(CCT *cct){
	
	return(10 + integer(((cct->falling_timer - cct->falling_damage_limit) * cct_fall_damage_const)));
}

// handle all stuff related to landing
// taking damage from landing etc
void ent_landed(ENTITY *ent, CCT *cct){
	
	if(cct->falling_timer > cct->land_timer_limit){
		
		// if we need to take damage from falling
		if(cct->falling_timer > cct->falling_damage_limit){
			
			// play damage sound from falling
			if(ent->obj_type == TYPE_PLAYER){
				
				if(snd_playing(ent->obj_snd_handle)){ snd_stop(ent->obj_snd_handle); }
				ent->obj_snd_handle = snd_play(player_land_ogg, player_snd_volume, 0);
			}
			
			ent->obj_pain_type = TYPE_FALL_DAMAGE;
			ent->obj_health -= ent_fall_damage(cct);
		}
		
		cct->land_timer = minv(cct->falling_timer * 6, 12);
	}
}

// handle stuff related movement speed
// f.e. don't allow cheating by holding forward and left-right keys
void ent_handle_movement_speed(CCT *cct, var spd){
	
	if(vec_length(vector(cct->force.x, cct->force.y, 0)) > spd){
		
		var len = sqrt(cct->force.x * cct->force.x + cct->force.y * cct->force.y);
		cct->force.x *= ((spd) / len);
		cct->force.y *= ((spd) / len);
	}
	
	// handle running and 'always running'
	if(cct->always_run == true){
		
		if(cct->run == true){
			
			cct->force.x *= cct->walk_speed_factor;
			cct->force.y *= cct->walk_speed_factor;
		}
		else{
			
			cct->force.x *= cct->run_speed_factor;
			cct->force.y *= cct->run_speed_factor;
		}
	}
	else{
		
		if(cct->run == true){
			
			cct->force.x *= cct->run_speed_factor;
			cct->force.y *= cct->run_speed_factor;
		}
		else{
			
			cct->force.x *= cct->walk_speed_factor;
			cct->force.y *= cct->walk_speed_factor;
		}
	}
}

// handle all gravity movement when out of the water
void ent_vertical_movement_on_foot(ENTITY *ent, CCT *cct){
	
	if(ent->z > cct->soil_height + (5 + 20 * cct->is_grounded) * time_step || cct->force.z > 0){
		
		cct->is_grounded = false;
		cct->force.z = maxv(cct->force.z - cct_gravity * time_step, -cct_gravity_max);
	}
	else{
		
		c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
		c_move(ent, nullvector, vector(0, 0, cct->soil_height - ent->z), MOVE_FLAGS | IGNORE_YOU | GLIDE);
		cct->is_grounded = true;
		cct->force.z = 0;
		cct->jump_out_of_water = false;		
	}
	
	if(cct->is_grounded == true || cct->jump_out_of_water == true){
		
		if(cct->jump == true){
			
			if(cct->jump_allowed == true){
				
				cct->force.z = cct->jump_height;
				cct->jump_allowed = false;
				if(cct->jump_out_of_water == true){ cct->jump_out_of_water = false; }
			}
		}
		else{ cct->jump_allowed = true; }
		
	}
	
	if(cct->force.z){
		
		c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
		c_move(ent, nullvector, vector(0, 0, maxv(cct->force.z * time_step, cct->soil_height - ent->z)), MOVE_FLAGS | IGNORE_YOU | GLIDE);
		
		// hit the ceiling ?
		// then disable jumping forces !
		if(HIT_TARGET && normal.z < -0.5){ cct->force.z = minv(cct->force.z, 0); }
	}
}

// handle all gravity movement when in water
void ent_vertical_movement_in_water(ENTITY *ent, CCT *cct){
	
	// reset falling timer
	cct->falling_timer = 0;
	
	// reset grounded and jumping
	cct->is_grounded = false;
	cct->jump_allowed = true;
	
	// if alive
	if(ent->obj_health > 0){
		
		// reset slowly gravity from on foot state
		cct->force.z -= (cct->force.z - 0) * 0.5 * time_step;
		c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
		c_move(ent, nullvector, vector(0, 0, maxv(cct->force.z * time_step, cct->soil_height - ent->z)), MOVE_FLAGS);
	}
	else{
		
		// otherwise, we need to drown slowly
		if(ent->z > cct->soil_height + (5 + 20 * cct->is_grounded) * time_step){
			
			cct->is_grounded = false;
			cct->force.z = maxv(cct->force.z - cct_gravity_fluid * time_step, -cct_gravity_fluid);
		}
		else{
			
			c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
			c_move(ent, nullvector, vector(0, 0, cct->soil_height - ent->z), MOVE_FLAGS | IGNORE_YOU | GLIDE);
			cct->is_grounded = true;
			cct->force.z = 0;		
		}
		
		if(cct->force.z){
			
			c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
			c_move(ent, nullvector, vector(0, 0, maxv(cct->force.z * time_step, cct->soil_height - ent->z)), MOVE_FLAGS | IGNORE_YOU | GLIDE);
		}
	}
	
	// water friction is on
	cct->friction = cct_water_fric;
	
	// get input
	cct->swim_z_force = (cct->swim_speed) * (cct->jump - (cct->dive));
	
	// handle running and 'always running'
	if(cct->always_run == true){
		
		if(cct->run == true){
			
			cct->swim_z_force *= cct->walk_speed_factor;
		}
		else{
			
			cct->swim_z_force *= cct->run_speed_factor * 0.75;
		}
	}
	else{
		
		if(cct->run == true){
			
			cct->swim_z_force *= cct->run_speed_factor * 0.75;
		}
		else{
			
			cct->swim_z_force *= cct->walk_speed_factor;
		}
	}
	
	// accelerate the entity relative speed by the force
	// replaced min with max (to eliminate 'creep')
	var temp_friction = maxv((1 - time_step * cct->friction), 0);
	cct->swim_z_speed = (time_step * cct->swim_z_force) + (temp_friction * cct->swim_z_speed);
	cct->swim_z_abs_dist = cct->swim_z_speed * time_step;
	
	// absolute distance 
	c_ignore(PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
	cct->water_z_distance = c_move(ent, nullvector, vector(0, 0, cct->swim_z_abs_dist), MOVE_FLAGS | IGNORE_PUSH);
	
	if(cct->water_z_distance > 0){
		
		cct->water_z_speed = cct->water_z_distance / time_step;
		cct->water_z_is_moving = 1;	
	}
	else{
		
		cct->water_z_speed = 0;
		cct->water_z_is_moving = 0;
	}
	
	// help us to surface (if we are alive)
	if(cct->water_origin_diff_z < 26 && cct->swim_z_force >= 0 && ent->obj_health > 0){
		
		ent->z -= (ent->z - (cct->water_z_height - 22)) * 0.25 * time_step;
	}
}

// handle all horizontal (XY) movement when out of the water
void ent_horizontal_movement_on_foot(ENTITY *ent, CCT *cct){
	
	// get input
	cct->force.x = cct->movement_speed * (cct->forward - cct->backward) * cct->water_depth_factor;
	cct->force.y = cct->movement_speed * (cct->straif_left - cct->straif_right) * cct->water_depth_factor;
	
	// if object is dead, stop all input forces
	if(ent->obj_health <= 0){
		
		cct->force.x = 0;
		cct->force.y = 0;
	}	
	
	// limit max movement speed (XY)
	// means DON'T ALLOW to cheat, by holding both movement keys
	// f.e. forward and left (or right)
	ent_handle_movement_speed(cct, cct->movement_speed);
	
	// if this is a player ?
	// and he is alive ...
	if(ent->obj_type == TYPE_PLAYER && ent->obj_health > 0){
		
		// rotate input with camera PAN angle (input)
		vec_rotate(&cct->force, vector(camera->pan, 0, 0));
	}
	
	// first, decide whether the actor is standing on the floor or not
	if(cct->is_grounded == true){
		
		// landed on ground
		ent_landed(ent, cct);
		
		// reset falling time
		cct->falling_timer = 0;
		
		// ground friction
		cct->friction = cct_gnd_fric;
		
		// slide down on slopes
		ent_slide_on_slopes(ent, cct);
	}
	else{
		
		// airborne - reduce all relative forces
		// to prevent him from jumping or further moving in the air
		cct->friction = cct_air_fric;
		
		cct->force.x *= cct_reduce_input_in_air_factor; // don't set the force completely to zero, otherwise
		cct->force.y *= cct_reduce_input_in_air_factor; // player could be stuck on top of a non-wmb entity
		
		// reset absolute forces
		vec_fill(&cct->abs_force, 0);
		
		// add falling time, only falling if moving downward
		if((cct->force.z <= 0)){ cct->falling_timer += time_step; }
	}
	
	// foot check here
	ent_foot_check(ent, cct);
	
	// accelerate the entity relative speed by the force
	var fric = maxv((1 - time_step * cct->friction), 0);
	cct->velocity.x = (time_step * cct->force.x) + (fric * cct->velocity.x);
	cct->velocity.y = (time_step * cct->force.y) + (fric * cct->velocity.y);
	cct->velocity.z = 0;
	
	// calculate relative distances to move
	cct->dist.x = cct->velocity.x * time_step;
	cct->dist.y = cct->velocity.y * time_step;
	cct->dist.z = 0;
	
	// calculate absolute distance to move
	cct->abs_dist.x = cct->abs_force.x * time_step;
	cct->abs_dist.y = cct->abs_force.y * time_step;
	cct->abs_dist.z = cct->abs_force.z * time_step;
	
	// add the speed given by the ground elasticity and the jumping force
	if(cct->is_grounded == true){
		
		// if the actor is standing on a moving platform, add it's horizontal displacement
		cct->abs_dist.x += cct->surface_speed.x;
		cct->abs_dist.y += cct->surface_speed.y;
	}
	
	// push forces from props
	if(vec_length(vector(cct->push_force.x, cct->push_force.y, 0)) > 0){
		
		cct->abs_dist.x += cct->push_force.x;
		cct->abs_dist.y += cct->push_force.y;
		
		vec_fill(&cct->push_force, 0);
	}
	
	// relative distance
	move_friction = 0;
	c_ignore(PLAYER_GROUP, PATHFIND_GROUP, 0);
	cct->moving_distance = c_move(ent, nullvector, &cct->dist, MOVE_FLAGS | IGNORE_PUSH | IGNORE_YOU | GLIDE);
	
	if(cct->moving_distance > 0.5){
		
		cct->moving_speed = cct->moving_distance / time_step;
		cct->is_moving = true;
	}
	else{
		
		cct->moving_speed = 0;
		cct->is_moving = false;
	}
	
	// absolute distance
	c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
	c_move(ent, nullvector, &cct->abs_dist, MOVE_FLAGS | IGNORE_YOU | GLIDE);
}

// handle all horizontal (XY) movement when in water
void ent_horizontal_movement_in_water(ENTITY *ent, CCT *cct){
	
	// get input
	cct->force.x = cct->swim_speed * (cct->forward - cct->backward);
	cct->force.y = cct->swim_speed * (cct->straif_left - cct->straif_right);
	
	// if object is dead, stop all input forces
	if(ent->obj_health <= 0){
		
		cct->force.x = 0;
		cct->force.y = 0;
	}	
	
	// limit max movement speed (XY)
	// means DON'T ALLOW to cheat, by holding both movement keys
	// f.e. forward and left (or right)
	ent_handle_movement_speed(cct, cct->swim_speed);

	// if this is a player ?
	// and he is alive ...
	if(ent->obj_type == TYPE_PLAYER && ent->obj_health > 0){
		
		// rotate input with camera PAN angle (input)
		vec_rotate(&cct->force, vector(camera->pan, 0, 0));
	}
	
	// foot check here
	ent_foot_check(ent, cct);
	
	// accelerate the entity relative speed by the force
	var fric = maxv((1 - time_step * cct->friction), 0);
	cct->velocity.x = (time_step * cct->force.x) + (fric * cct->velocity.x);
	cct->velocity.y = (time_step * cct->force.y) + (fric * cct->velocity.y);
	cct->velocity.z = 0;
	
	// calculate relative distances to move
	cct->dist.x = cct->velocity.x * time_step;
	cct->dist.y = cct->velocity.y * time_step;
	cct->dist.z = 0;
	
	// calculate absolute distance to move
	cct->abs_dist.x = cct->abs_force.x * time_step;
	cct->abs_dist.y = cct->abs_force.y * time_step;
	cct->abs_dist.z = cct->abs_force.z * time_step;
	
	// add the speed given by the ground elasticity and the jumping force
	if(cct->is_grounded == true){
		
		// if the actor is standing on a moving platform, add it's horizontal displacement
		cct->abs_dist.x += cct->surface_speed.x;
		cct->abs_dist.y += cct->surface_speed.y;
	}
	
	// relative distance
	move_friction = 0;
	c_ignore(PLAYER_GROUP, PATHFIND_GROUP, 0);
	cct->moving_distance = c_move(ent, nullvector, &cct->dist, MOVE_FLAGS | IGNORE_PUSH | IGNORE_YOU | GLIDE);
	
	if(cct->moving_distance > 0.5){
		
		cct->moving_speed = cct->moving_distance / time_step;
		cct->is_moving = true;
	}
	else{
		
		cct->moving_speed = 0;
		cct->is_moving = false;
	}
	
	// absolute distance
	c_ignore(PUSH_GROUP, PLAYER_GROUP, SWITCH_ITEM_GROUP, PATHFIND_GROUP, 0);
	c_move(ent, nullvector, &cct->abs_dist, MOVE_FLAGS | IGNORE_YOU | GLIDE);
}

// handle all the movement
void ent_movement(ENTITY *ent, CCT *cct){
	
	// allowed to move?
	if(ent->obj_allow_move == 1){
		
		// if we are swimming
		if(cct->water_state >= IN_WATER){
			
			// stop all movement from on foot state
			ent_stop_movement_on_foot(cct);
			
			// gravity
			ent_vertical_movement_in_water(ent, cct);
			
			// x-y movement
			ent_horizontal_movement_in_water(ent, cct);
		}
		else{
			
			// stop all movement from in water state
			ent_stop_movement_in_water(cct);
			
			// gravity
			ent_vertical_movement_on_foot(ent, cct);
			
			// x-y movement
			ent_horizontal_movement_on_foot(ent, cct);
		}
	}
	
	// reset landing timer
	cct->land_timer -= (cct->land_timer - 0) * 0.5 * time_step;
	if(ent->obj_health <= 0){ cct->land_timer = 0; }
	
	// detect water
	ent_detect_water_state(ent, cct);
}

// slide on steep slopes, if we are allowed to
void ent_slide_on_slopes(ENTITY *ent, CCT *cct){
	
	// we aren't standing on stairs and we are allowed to slide down ?
	if(cct->ground_info != STAIRS && cct->soil_allow_sliding == true){
		
		// reset absolute forces slowly (don't stop right after the slope ends)
		cct->abs_force.x -= (cct->abs_force.x - 0) * 0.25 * time_step;
		cct->abs_force.y -= (cct->abs_force.y - 0) * 0.25 * time_step;
		cct->abs_force.z = 0;
	}
	else{
		
		// reset absolute forces
		vec_fill(&cct->abs_force, 0);
	}
	
	// if slope is too steep
	if(cct->surface_normal.z < 0.9){
		
		// reduce ahead force because player force it is now deflected upwards
		cct->force.x *= cct->surface_normal.z;
		cct->force.y *= cct->surface_normal.z;
		
		// only if we are on slopes
		if(cct->ground_info != STAIRS && cct->soil_allow_sliding == true){
			
			// gravity draws him down the slope
			cct->abs_force.x = cct->surface_normal.x * cct_gravity * 2 * cct_slope_fac;
			cct->abs_force.y = cct->surface_normal.y * cct_gravity * 2 * cct_slope_fac;	
		}
	}
}

// stop all movement for the given cct
void ent_stop_movement(CCT *cct){
	
	vec_fill(&cct->force, 0);
	vec_fill(&cct->abs_force, 0);
	vec_fill(&cct->dist, 0);
	vec_fill(&cct->abs_dist, 0);
	vec_fill(&cct->speed, 0);
	vec_fill(&cct->velocity, 0);
	vec_fill(&cct->push_force, 0);

	cct->is_moving = 0;
	cct->moving_distance = 0;
	cct->moving_speed = 0;
	
	cct->forward = 0;
	cct->backward = 0;
	cct->straif_left = 0;
	cct->straif_right = 0;
	cct->jump = 0;
	cct->run = 0;
	
	cct->swim_z_force = 0;
	cct->swim_z_speed = 0;
	cct->swim_z_abs_dist = 0;
	
	cct->water_z_distance = 0;
	cct->water_z_speed = 0;
	cct->water_z_is_moving = 0;
}

// stop all movement from on foot state
void ent_stop_movement_on_foot(CCT *cct){
	
	// no pushing forces in water
	vec_fill(&cct->push_force, 0);
	
	// reset absolute distance to move
	cct->abs_force.x -= (cct->abs_force.x - 0) * 0.25 * time_step;
	cct->abs_force.y -= (cct->abs_force.y - 0) * 0.25 * time_step;
	cct->abs_force.z = 0;
	
	cct->water_out_switch = false;
}

// stop all movement from on water state
void ent_stop_movement_in_water(CCT *cct){
	
	// reset water in switch only if player stands on the ground
	if(cct->is_grounded == true){
		
		cct->water_in_switch = false;
	}
	
	cct->swim_z_force = 0;
	cct->swim_z_speed = 0;
	cct->swim_z_abs_dist = 0;
	
	cct->water_z_distance = 0;
	cct->water_z_speed = 0;
	cct->water_z_is_moving = 0;
}