#ifndef _PARTICLES_H_
#define _PARTICLES_H_

// This header handles everything related to particles

// randomize given vector within the given range
void vec_randomize(var *vec, var range);

// slow down given particle
void particle_slow_down(PARTICLE *p, var value);

// fading event function for bubble particle
void bubbles_fade_event(PARTICLE *p);

// bubbles particle
void bubbles_particle(PARTICLE *p);

// spawn given amount of bubbles at the given position with the given range
void bubbles_spawn(VECTOR *pos, var num, VECTOR *range, var water_zone_height);

// bullet impact fade event
void bullet_impact_fade_event(PARTICLE *p);

// impact particle effects
void bullet_impact_particle(PARTICLE *p);

// blood particles effect
void blood_impact_particle(PARTICLE *p);

// water splash effect event function
void water_splash_event(PARTICLE *p);

// init splash effect
void water_splash_init(PARTICLE *p, var size_, var size_random);

// simple water splash effect
void water_splash_particle(PARTICLE *p);

// water splash effect from explosions
void water_explo_splash_particle(PARTICLE *p);

// explosion fading event function
void explosion_fade_event(PARTICLE *p);

// quake like explosion particles
void explosion_particle(PARTICLE *p);

// smoke trail fading event function
void smoketrail_fade_function(PARTICLE *p);

// simple smoketrail particles
void smoketrail_particle(PARTICLE *p);

#endif