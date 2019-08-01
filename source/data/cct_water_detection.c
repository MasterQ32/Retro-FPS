
// detect water regions and change our water movement factor, if we are in water
void ent_detect_water(ENTITY *ent, CCT *cct){
	
	cct->water_reg_total = 0;
	cct->water_depth_factor = 1;
	
	if(cct->water_reg_total == 0){
		
		cct->water_origin_diff_z = 0;
		cct->water_z_height = 0;
	}
	
	while(region_get(reg_water_str, cct->water_reg_total, &cct->water_reg_min, &cct->water_reg_max) != 0){
		
		cct->water_reg_total += 1;
		
		if(ent->x < cct->water_reg_min.x || ent->x > cct->water_reg_max.x){ continue; }
		if(ent->y < cct->water_reg_min.y || ent->y > cct->water_reg_max.y){ continue; }
		if((cct->water_reg_min.z - ent->z) > 0 || (ent->z - cct->water_reg_max.z) > 0){ continue; }
		
		cct->water_origin_diff_z = cct->water_reg_max.z - ent->z;
		cct->water_z_height = cct->water_reg_max.z;
		cct->water_depth_factor = clamp(1 - (cct->water_origin_diff_z / 25), 0.1, 1);	
	}
}

// set water state for the given cct
void ent_set_water_state(CCT *cct){
	
	// if we are in water
	if(cct->water_origin_diff_z > 0 && cct->water_origin_diff_z < (cct->bbox_z / 2)){
		
		cct->water_state = ON_WATER;
	} 
	else{
		
		if(cct->water_origin_diff_z > (cct->bbox_z / 2)){
			
			cct->water_state = IN_WATER;
		} 
		else{
			
			cct->water_state = OUT_OF_WATER;
		}
	}
}

// detect if entity is inside of the water region, or not
void ent_detect_water_state(ENTITY *ent, CCT *cct){
	
	// detect water region
	ent_detect_water(ent, cct);
	
	// if player ?
	if(ent->obj_type == TYPE_PLAYER){
		
		// camera is inside of the water ?
		// then our head is in water !
		if(region_check(reg_water_str, &camera->x, &camera->x) != 0){
			
			cct->water_state = HEAD_IN_WATER;
		}
		else{
			
			// handle all other states as usual
			ent_set_water_state(cct);
		}
		
	}
	else{
		
		// handle all other states as usual
		ent_set_water_state(cct);
	}
}