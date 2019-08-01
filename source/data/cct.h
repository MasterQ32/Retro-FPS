#ifndef _CCT_H_
	#define _CCT_H_
	
	// This is basic character controller (cct) header
	// it's used for registering, deleting and contains all cct parameters
	//
	// This header also includes such headers as
	// - cct_helper.h (all helper functions needed for proper cct behaviour)
	// - cct_gravity.h (all functions needed to perform gravity traces etc)
	// - cct_movement.h (all functions needed to perform all movement)
	// - cct_state_machine.h (all functions needed to setup a state machine)
	
	// ground info defines
	#define SOLID 0
	#define STAIRS 1
	#define MOVING 2
	
	// state machine states
	#define IDLE 0
	#define WALK 1
	#define RUN 2
	#define JUMPED 3
	#define IN_AIR 4
	#define LANDED 5
	#define PAIN 6
	#define SHOOT 7
	#define DEATH 8
	
	var cct_gravity = 4; // gravity strength
	var cct_gravity_max = 90; // gravity strength max
	
	var cct_gnd_fric = 0.5; // ground friction
	var cct_air_fric = 0.1; // air friction
	var cct_grav_trace_start_offset = 4; // offset for start position of the gravity trace
	var cct_grav_trace_end_offset = 24; // length of the gravity trace
	
	var cct_def_foot_step_height = 8; // default footstep height
	var cct_ground_z_offset = 2; // offset from ground
	var cct_slope_fac = 0.5; // walkable slope angle
	
	var cct_reduce_input_in_air_factor = 0.2; // reduce input forces when we are in air
	
	// cct main structure
	typedef struct {
		
		VECTOR force;
		VECTOR abs_force;
		
		VECTOR dist;
		VECTOR abs_dist;
		
		VECTOR speed;
		VECTOR velocity;
		VECTOR push_force;
		VECTOR push_velocity;
		
		VECTOR surface_normal;
		VECTOR surface_speed;
		
		VECTOR origin;
		
		var bbox_x;
		var bbox_y;
		var bbox_z;
		
		var ground_info;
		
		var foot_height;
		var foot_check_length;
		var foot_step_height;
		var soil_height;
		var soil_allow_sliding;
		var is_grounded;
		
		var jump;
		var jump_allowed;
		var jump_height;
		
		var movement_speed;
		var friction;
		var falling_timer;
		var land_timer;
		var land_timer_limit;
		
		var run;
		var always_run;
		
		var walk_speed_factor;
		var run_speed_factor;
		var water_depth_factor;
		
		var moving_distance;
		var moving_speed;
		var is_moving;
		
	} CCT;
	
	// register and initializes new cct (character controller)
	// and save it's pointer into given entities obj_cct_struct skill
	CCT *register_cct(ENTITY *ent);
	
	// initialize all major variables for the given cct
	// this function called inside of register_cct right after creating new cct
	void init_cct(CCT *cct);
	
	// returns pointer of the cct from given entity's obj_cct_struct skill
	CCT *get_cct(ENTITY *ent);
	
	// removes cct from the memory
	// pointer is taken from given entity's obj_cct_struct skill
	void delete_cct(ENTITY *ent);
	
	// remove given cct from the memory
	void delete_cct(CCT *cct);
	
	#include "cct_helper.h"
	#include "cct_gravity.h"
	#include "cct_movement.h"
	#include "cct.c"
#endif