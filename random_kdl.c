/*=========================================================================
  A fblock that generates random kdl_twsit and kdl_frams structs.
  Copyright (C) 2013 Evert Jans
 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see [http://www.gnu.org/licenses/].
			
=========================================================================*/

/* #define DEBUG 1 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "ubx.h"

#include <kdl.h>
#include "../youbot_driver/types/motionctrl_jnt_state.h"

#include "types/random_kdl_config.h"
#include "types/random_kdl_config.h.hexarr"

ubx_type_t random_kdl_config_type = def_struct_type(struct random_kdl_config, &random_kdl_config_h);

char rnd_meta[] =
	"{ doc='A random number generator function block to fill kdl_twist and kdl_frame',"
	"  real-time=true,"
	"}";

ubx_config_t rnd_config[] = {
	{ .name="min_max_config", .type_name = "struct random_kdl_config" },
	{ NULL },
};

ubx_port_t rnd_ports[] = {

	{ .name="seed", .attrs=PORT_DIR_IN, .in_type_name="unsigned int" },
	{ .name="base_msr_twist", .attrs=PORT_DIR_OUT, .out_type_name="struct kdl_twist" },
	{ .name="base_msr_odom", .attrs=PORT_DIR_OUT, .out_type_name="struct kdl_frame" },
	{ .name="arm1_state", .attrs=PORT_DIR_OUT, .out_type_name="struct motionctrl_jnt_state"},

	{ NULL },
};

struct random_kdl_info {
	int min;
	int max;
        struct kdl_twist twist;
        struct kdl_frame frame;
	struct motionctrl_jnt_state armState;
};

/* convenience functions to read/write from the ports */
def_read_fun(read_uint, unsigned int)
//def_write_fun(write_uint, unsigned int)
def_write_fun(write_kdl_twist, struct kdl_twist)
def_write_fun(write_kdl_frame, struct kdl_frame)
def_write_fun(write_arm_state, struct motionctrl_jnt_state)

static int rnd_init(ubx_block_t *c)
{
	int ret=0;

	DBG(" ");
	if ((c->private_data = calloc(1, sizeof(struct random_kdl_info)))==NULL) {
		ERR("Failed to alloc memory");
		ret=EOUTOFMEM;
		goto out;
	}


 out:
	return ret;
}


static void rnd_cleanup(ubx_block_t *c)
{
	DBG(" ");
	free(c->private_data);
}

static int rnd_start(ubx_block_t *c)
{
	DBG("in");
	uint32_t seed, ret;
	unsigned int clen;
	struct random_kdl_config* rndconf;
	struct random_kdl_info* inf;

	inf=(struct random_kdl_info*) c->private_data;

	/* get and store min_max_config */
	rndconf = (struct random_kdl_config*) ubx_config_get_data_ptr(c, "min_max_config", &clen);
	inf->min = rndconf->min;
	inf->max = (rndconf->max == 0) ? INT_MAX : rndconf->max;

	/* seed is allowed to change at runtime, check if new one available */
	ubx_port_t* seed_port = ubx_port_get(c, "seed");
	ret = read_uint(seed_port, &seed);
	if(ret>0) {
		DBG("starting component. Using seed: %d, min: %d, max: %d", seed, inf->min, inf->max);
		srandom(seed);
	} else {
		DBG("starting component. Using min: %d, max: %d", inf->min, inf->max);
	}
	return 0; /* Ok */
}

static double randomize(struct random_kdl_info *inf) {
        
        double ret = rand() / (double)RAND_MAX;
        ret = ret * inf->max;

	return ret;
}

static void randomTwist(struct random_kdl_info* inf) {
        
	inf->twist.vel.x = randomize(inf);
	inf->twist.vel.y = randomize(inf);
	inf->twist.vel.z = randomize(inf);
	inf->twist.rot.x = randomize(inf);
	inf->twist.rot.y = randomize(inf);
	inf->twist.rot.z = randomize(inf);
}

static void randomFrame(struct random_kdl_info* inf) {
        
	inf->frame.p.x = randomize(inf);
	inf->frame.p.y = randomize(inf);
	inf->frame.p.z = randomize(inf);
	inf->frame.M.data[0] = randomize(inf);
	inf->frame.M.data[1] = randomize(inf);
	inf->frame.M.data[2] = randomize(inf);
	inf->frame.M.data[3] = randomize(inf);
	inf->frame.M.data[4] = randomize(inf);
	inf->frame.M.data[5] = randomize(inf);
	inf->frame.M.data[6] = randomize(inf);
	inf->frame.M.data[7] = randomize(inf);
	inf->frame.M.data[8] = randomize(inf);
}

static void randomArmState(struct random_kdl_info* inf) {
	
	inf->armState.pos[0] = randomize(inf);
	inf->armState.pos[1] = randomize(inf);
	inf->armState.pos[2] = randomize(inf);
	inf->armState.pos[3] = randomize(inf);
	inf->armState.pos[4] = randomize(inf);

	inf->armState.vel[0] = randomize(inf);
	inf->armState.vel[1] = randomize(inf);
	inf->armState.vel[2] = randomize(inf);
	inf->armState.vel[3] = randomize(inf);
	inf->armState.vel[4] = randomize(inf);

	inf->armState.eff[0] = randomize(inf);
	inf->armState.eff[1] = randomize(inf);
	inf->armState.eff[2] = randomize(inf);
	inf->armState.eff[3] = randomize(inf);
	inf->armState.eff[4] = randomize(inf);
}


static void rnd_step(ubx_block_t *c) {

	struct random_kdl_info* inf;
	inf=(struct random_kdl_info*) c->private_data;

	ubx_port_t* twist_port = ubx_port_get(c, "base_msr_twist");
        ubx_port_t* frame_port = ubx_port_get(c, "base_msr_odom");
	ubx_port_t* arm_port = ubx_port_get(c, "arm1_state");

	randomTwist(inf);
        DBG("x value from twist rotation: %f\n", inf->twist.rot.x);
        write_kdl_twist(twist_port, &inf->twist);

	randomFrame(inf);
        DBG("fourth value from frame matrix: %f\n", inf->frame.M.data[3]);
        write_kdl_frame(frame_port, &inf->frame);

	randomArmState(inf);
	DBG("fourth value from position matrix arm state: %f\n", inf->armState.pos[3]);
	write_arm_state(arm_port, &inf->armState);
}

/* put everything together */
ubx_block_t random_comp = {
	.name = "random_kdl/random_kdl",
	.type = BLOCK_TYPE_COMPUTATION,
	.meta_data = rnd_meta,
	.configs = rnd_config,
	.ports = rnd_ports,

	/* ops */
	.init = rnd_init,
	.start = rnd_start,
	.step = rnd_step,
	.cleanup = rnd_cleanup,
};

static int random_init(ubx_node_info_t* ni)
{
	ubx_type_register(ni, &random_kdl_config_type);
	return ubx_block_register(ni, &random_comp);
}

static void random_cleanup(ubx_node_info_t *ni)
{
	ubx_type_unregister(ni, "struct random_kdl_config");
	ubx_block_unregister(ni, "random_kdl/random_kdl");
}

UBX_MODULE_INIT(random_init)
UBX_MODULE_CLEANUP(random_cleanup)
UBX_MODULE_LICENSE_SPDX(BSD-3-Clause)
