#ifndef _CCT_BASE_H_
	#define _CCT_BASE_H_
	
	#define SOLID 0
	#define STAIRS 1
	#define MOVING 2
	
	#define IDLE 0
	#define WALK 1
	#define RUN 2
	#define JUMPED 3
	#define IN_AIR 4
	#define LANDED 5
	#define PAIN 6
	#define SHOOT 7
	#define DEATH 8
	
	#define ABOVE 0
	#define WADE 1
	#define UNDERWATER 2
	
	var cct_gravity = 4; // gravity strength
	var cct_gravity_max = 90; // gravity strength max
	
	var cct_gnd_fric = 0.5; // ground friction
	var cct_air_fric = 0.1; // air friction
	var cct_land_timer = 0.25; // land state timer (not the best way, but will do)
	
	var cct_grav_trace_start_offset = 4; // offset for start position of the gravity trace
	var cct_grav_trace_end_offset = 24; // length of the gravity trace
	
	var cct_def_foot_step_height = 8; // default footstep height
	var cct_ground_z_offset = 2; // offset from ground
	var cct_slope_fac = 0.5; // walkable slope angle
	
	var cct_land_volume = 450; // default volume for landing sound effect
	var cct_player_land_volume = 45; // same as above, but for player
	var cct_water_snd_volume = 450; // default volume for water in/out sound effects
	var cct_player_water_volume = 35; // same as above, but for player
	
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
		VECTOR aqua_reg_min;
		VECTOR aqua_reg_max;
		
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
		var land_timer_limit;
		var land_timer;
		
		var aqua_state;
		var aqua_depth_factor;
		var aqua_reg_total;
		var aqua_origin_diff_z;
		var aqua_z_height;
		var aqua_snd_handle;
		var aqua_snd_water_in;
		var aqua_snd_water_out;
		var aqua_toxic_damage_timer;
		var aqua_toxic_damage_timer_limit;
		var aqua_toxic_damage_def;
		
		var run;
		var always_run;
		
		var walk_speed_factor;
		var run_speed_factor;
		
		var moving_distance;
		var moving_speed;
		var is_moving;
		
	} CCT_PHYSICS;
	
	CCT_PHYSICS *register_cct(ENTITY *ent);
	
	void init_cct(CCT_PHYSICS *cct);
	
	CCT_PHYSICS *get_cct(ENTITY *ent);
	
	void delete_cct(ENTITY *ent);
	
	void cct_gravity_trace(ENTITY *ent, CCT_PHYSICS *physics);
	
	void ent_foot_push(VECTOR *input, VECTOR *surf_normal, var strength);
	
	void ent_foot_push_from_normals(ENTITY *ent, CCT_PHYSICS *physics, VECTOR *hit_pos, VECTOR *surf_normal);
	
	void ent_foot_stop_velocity(var is_x_force, ENTITY *ent, CCT_PHYSICS *physics, VECTOR *hit_pos);
	
	void ent_foot_check(ENTITY *ent, CCT_PHYSICS *physics);
	
	void ent_state_machine(ENTITY *ent, CCT_PHYSICS *physics);
	
	void ent_toxic_water_damage(ENTITY *ent, CCT_PHYSICS *physics);
	
	var is_cct_in_rect(ENTITY *cct, ENTITY *rect, var scale);
	
	#include "cct_base.c"
	#include "cct_movement.h"
	#include "cct_aqua_detection.h"
#endif