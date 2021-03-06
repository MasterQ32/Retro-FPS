
// randomize given vector within the given range
void vec_randomize(var *vec, var range)
{
	vec[0] = random(1) - 0.5;
	vec[1] = random(1) - 0.5;
	vec[2] = random(1) - 0.5;
	vec_normalize(vec, random(range));
}

// slow down given particle
void particle_slow_down(PARTICLE *p, var value)
{
	VECTOR temp_pos;
	vec_set(&temp_pos, &p->vel_x);

	vec_normalize(&temp_pos, minv(vec_length(&temp_pos), value * time_step));
	vec_inverse(&temp_pos);

	vec_add(&p->vel_x, &temp_pos);
}

// fading event function for bubble particle
void bubbles_fade_event(PARTICLE *p)
{
	particle_slow_down(p, 0.5);
	p->size += 0.05 * time_step;
	if (p->z > p->skill_x - 16)
	{
		p->lifespan = 0;
	}
	if (region_check(reg_water_str, &p->x, &p->x) == 0)
	{
		p->lifespan = 0;
	}
}

// bubbles particle
void bubbles_particle(PARTICLE *p)
{
	p->skill_x = p->vel_x;
	p->vel_x = 0;
	p->alpha = 0;

	if (p->z < p->skill_x - 16)
	{
#ifndef PARTICLE_EFFECTS
		p->bmap = bubbles_tga;
		p->size = 2 + random(4);
#else
		p->bmap = particle_png;
		p->size = 0.5 + random(0.5);
#endif
		p->alpha = 25 + random(10);
	}

	VECTOR temp_pos;
	vec_randomize(&temp_pos, 6 + random(4));
	p->vel_z -= 5;
	vec_add(&p->vel_x, &temp_pos);

	p->gravity = -0.75;
	p->lifespan = 200;
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER);
	p->event = bubbles_fade_event;
}

// spawn given amount of bubbles at the given position with the given range
void bubbles_spawn(VECTOR *pos, var num, VECTOR *range, var water_zone_height)
{
	var i = 0;
	for (i = 0; i < num; i++)
	{
		VECTOR temp_pos;
		temp_pos.x = pos->x + range->x - random(range->x * 2);
		temp_pos.y = pos->y + range->y - random(range->y * 2);
		temp_pos.z = pos->z - range->z + (range->z * 2 - random(range->z * 4));
		effect(bubbles_particle, 1, &temp_pos, vector(water_zone_height, 0, 0));
	}
}

// bullet impact fade event
void bullet_impact_fade_event(PARTICLE *p)
{
	particle_slow_down(p, 0.5);
}

// impact particle effects
void bullet_impact_particle(PARTICLE *p)
{
	VECTOR temp_vec;
	vec_randomize(&temp_vec, (2 + random(2)));
	vec_add(&p->vel_x, &temp_vec);
	vec_fill(&p->blue, random(96));
	p->bmap = particle_png;
	p->size = 1 + random(1);
	p->gravity = 0.75;
	p->alpha = 70 + random(30);
	p->lifespan = 8 + random(1);
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER | LIGHT);
	p->event = bullet_impact_fade_event;
}

// blood particles effect
void blood_impact_particle(PARTICLE *p)
{
	VECTOR temp_vec;
	vec_randomize(&temp_vec, (2 + random(2)));
	vec_add(&p->vel_x, &temp_vec);
	vec_set(&p->blue, vector(0, 0, 32 + random(64)));
	p->bmap = particle_png;
	p->size = 1 + random(1);
	p->gravity = 0.75;
	p->alpha = 70 + random(30);
	p->lifespan = 8 + random(1);
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER | LIGHT);
	p->event = bullet_impact_fade_event;
}

// water splash effect event function
void water_splash_event(PARTICLE *p)
{
	if (abs(p->vel_z) < 0.1 && p->z < p->skill_x)
	{
		vec_fill(&p->vel_x, 0);
		p->z = p->skill_x;
	}
	else
	{
		p->vel_z -= time_step;
		if (p->z < p->skill_x)
		{
			p->alpha = 0;
		}
	}

	p->alpha -= 5 * time_step;
	if (p->alpha < 0)
	{
		p->lifespan = 0;
	}
}

// init splash effect
void water_splash_init(PARTICLE *p, var size_, var size_random)
{
	// we recieve water region height
	// as velocity X
	p->skill_x = p->vel_x;
	p->vel_x = 0;

	// XY randomization
	// as velocity Y
	p->skill_y = p->vel_y;
	p->vel_y = 0;

	// Z randomization
	// as velocity Z
	p->skill_z = p->vel_z;
	p->vel_z = 0;

	p->vel_x = random(p->skill_y * 2) - p->skill_y;
	p->vel_y = random(p->skill_y * 2) - p->skill_y;
	p->vel_z = p->skill_z + random(3);

	p->bmap = particle_png;

	var random_color = random(32);
	p->red = 11 + random(20) + random_color;
	p->green = 32 + random(50) + random_color;
	p->blue = 7 + random(20) + random_color;

	p->size = size_ + random(size_random);
	p->lifespan = 50;
	p->alpha = 100;
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER);
	p->event = water_splash_event;
}

// simple water splash effect
void water_splash_particle(PARTICLE *p)
{
	water_splash_init(p, 0.5, 0.5);
}

// water splash effect from explosions
void water_explo_splash_particle(PARTICLE *p)
{
	water_splash_init(p, 1, 2);
}

// explosion fading event function
void explosion_fade_event(PARTICLE *p)
{
	particle_slow_down(p, 0.5);

	p->alpha -= 7.5 * time_step;
	if (p->alpha < 0)
	{
		p->lifespan = 0;
	}
}

// quake like explosion particles
void explosion_particle(PARTICLE *p)
{
	vec_set(&p->vel_x, vector(random(32) - 16, random(32) - 16, random(32) - 16));
	p->gravity = 0.25;

	p->bmap = particle_png;
	vec_set(&p->blue, vector(32, 64 + random(64), 128 + random(128)));

	p->size = 0.5 + random(1);
	p->lifespan = 50;
	p->alpha = 100;
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER);
	p->event = explosion_fade_event;
}

// fading event function for underwater explosion
void explosion_underwater_fade_event(PARTICLE *p)
{
	particle_slow_down(p, 0.9);

	if (p->z > p->skill_x)
	{
		p->lifespan = 0;
	}

	p->alpha -= 5 * time_step;
	if (p->alpha < 0)
	{
		p->lifespan = 0;
	}
}

// explosion effect underwater
void explosion_underwater_particle(PARTICLE *p)
{
	p->skill_x = p->vel_x;
	p->vel_x = 0;

	vec_set(&p->vel_x, vector(random(16) - 8, random(16) - 8, random(16) - 8));
	p->gravity = -1;

#ifndef PARTICLE_EFFECTS
	p->bmap = bubbles_tga;
#else
	p->bmap = particle_png;
	p->size = 0.5 + random(1);
	vec_fill(&p->blue, 128 + random(128));
#endif

	p->lifespan = 50;
	p->alpha = 100;
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER);
	p->event = explosion_underwater_fade_event;
}

// smoke trail fading event function
void smoketrail_fade_function(PARTICLE *p)
{
	particle_slow_down(p, 0.9);

	p->alpha -= p->skill_z * time_step;
	if (p->alpha < 0)
	{
		p->lifespan = 0;
	}
}

// simple smoketrail particles
void smoketrail_particle(PARTICLE *p)
{

#ifndef PARTICLE_EFFECTS
	vec_set(&p->vel_x, vector(random(4) - 2, random(4) - 2, random(4) - 2));
	p->bmap = smoke_tga;
	p->size = 10 + random(5);
	vec_fill(&p->blue, 128 + random(96));
	p->alpha = 35;
	p->skill_z = 2;
#else
	vec_set(&p->vel_x, vector(random(4) - 2, random(4) - 2, random(4) - 2));
	p->bmap = particle_png;
	p->size = 1 + random(1);
	vec_fill(&p->blue, random(96));
	p->alpha = 100;
	p->skill_z = 5;
#endif

	p->gravity = 0.25;
	p->lifespan = 50;
	p->flags |= (MOVE | TRANSLUCENT | NOFILTER);
	p->event = smoketrail_fade_function;
}