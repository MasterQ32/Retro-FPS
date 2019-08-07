
// create new level object
void level_setup(var num, STRING *str, STRING *snd, COLOR *fog, ANGLE *sun, var lightness, var near, var far, var start, var end)
{
	strcpy(map[num].name, str);
	strcpy(map[num].music, snd);
	vec_set(&map[num].fog_color, fog);
	vec_set(&map[num].sun_angle, sun);

	map[num].sun_lightness = lightness;
	map[num].clip_near = near;
	map[num].clip_far = far;
	map[num].fog_start = start;
	map[num].fog_end = end;
}

// initialize all levels
void init_levels()
{
	level_setup(0, "map.wmb", "", vector(128, 128, 128), vector(270, -25, 0), 0, 0.1, 4500, 100, 2000);
}

// set level settings (such as fog, clipping etc)
void set_level_settings()
{
	set(camera, NOFLAG1);
	vec_set(&d3d_lodfactor, vector(12.5, 25, 50));

	sun_light = map[level_id].sun_lightness;
	sun_angle.pan = map[level_id].sun_angle.pan;
	sun_angle.tilt = map[level_id].sun_angle.tilt;

	camera->clip_near = map[level_id].clip_near;
	camera->clip_far = map[level_id].clip_far;

	camera->fog_start = map[level_id].fog_start;
	camera->fog_end = map[level_id].fog_end;

	fog_color = 4;
	vec_set(&d3d_fogcolor4.blue, &map[level_id].fog_color);
	vec_set(&sky_color.blue, &d3d_fogcolor4.blue);
}

// reset some global level variables
void level_reset()
{
	// stop sounds/music
	snd_stopall(4);
	music_stop();

	// level isn't loaded any more
	level_is_loaded = false;
}

// restart current level
void level_restart()
{
	// don't allow to restart twice at the same time
	if (proc_status(level_restart) > 0)
	{
		return;
	}

	level_load(level_id);
}

// load level with the given id
void level_load(var num)
{
	// don't allow to load twice at the same time
	if (proc_status(level_load) > 0)
	{
		return;
	}

	reset(camera, SHOW);

	level_reset();
	wait_for(level_reset);

	freeze_mode = true;
	level_load(map[level_id].name);
	freeze_mode = false;

	// level wasn't loaded ?
	if (!level_ent)
	{
		diag("\n\nCRITICAL ERROR! Level couldn't be loaded!\n");
		return;
	}

#ifdef FREE_VERSION
	level_ent->material = mtl_shaded;
#endif

	level_ent->group = LEVEL_GROUP;
	level_ent->push = LEVEL_GROUP;

	set_level_settings();
	music_start();
	level_is_loaded = true;

	set(camera, SHOW);
}