
#include <acknex.h>
#include <default.c>
#include <windows.h>
#include <strio.c>

#define PRAGMA_POINTER

#define PRAGMA_PATH "data"
#define PRAGMA_PATH "data\\fx"
#define PRAGMA_PATH "data\\models"
#define PRAGMA_PATH "data\\sounds"
#define PRAGMA_PATH "data\\sounds\\player"
#define PRAGMA_PATH "data\\sounds\\props"
#define PRAGMA_PATH "data\\sprites"

#include "defines.h"
#include "resources.h"
#include "particles.h"
#include "shader_pipeline.h"
#include "props.h"
#include "cct.h"
#include "level_logic.h"
#include "player.h"

void on_ent_remove_event(ENTITY *ent){
	
	if(ent->obj_type == TYPE_PLAYER){
		
		delete_cct(ent);
		delete_player_struct(ent);
	}
	
	if(ent->obj_type == TYPE_ELEVATOR || ent->obj_type == TYPE_DOOR || ent->obj_type == TYPE_PLATFORM || ent->obj_type == TYPE_SECRET_WALL){
		
		delete_props(ent);
	}
}

void mouse_lock_in_window(){
	
	static var autolock_mouse_locked = false;
	
	if(!autolock_mouse_locked && window_focus){
		
		autolock_mouse_locked = true;
		RECT rect;
		GetClientRect(hWnd, &rect);
		ClientToScreen(hWnd, &rect);
		ClientToScreen(hWnd, &rect.right);
		ClipCursor(&rect);
	}
	if(autolock_mouse_locked && !window_focus){
		
		autolock_mouse_locked = false;
		ClipCursor(NULL);
	}
}

void set_video_settings(){
	
	video_set(800, 600, 16, 2);
	video_aspect = 1.333;
}

void set_engine_settings(){
	
	warn_level = 6;
	
	doppler_factor = 0;
	
	max_entities = 10000;
	preload_mode = 3 + 4;
	
	d3d_near_models = 0.5;
	d3d_near_sprites = 0.5;
	d3d_near_decals = 0.99;
	
	fps_max = 60;
	fps_min = 30;
	time_smooth = 0.9;
	
	random_seed(0);
	
	mouse_pointer = 0;
}

void main_update()
{
	if(!game_running)
		return;
		
	game_ticks += time_frame;	
	mouse_lock_in_window();
		
	if(key_e) { fps_max = 20; }
	else { fps_max = 60; }
	
	ENTITY * ent;
	for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)) {
		if(ent->obj_type != TYPE_PLAYER)
			continue;
		player_update(ent);
	}
}

void main(){
	
	on_ent_remove = on_ent_remove_event;
	on_f4 = pipeline_update;
	on_f5 = pipeline_resolution;
	
	game_running = true;
	
	set_level_names();
	set_video_settings();
	set_engine_settings();
	
	level_load_();
	pipeline_start();


	on_caps = player_toggle_run;
	on_r = level_restart;
	
	on_frame = main_update;
}