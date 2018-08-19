/** 
  * Game boy advance pong demo
  * This was borrowed from a thing I found online for how to write a GBA game
  * The link for this is here:
  * https://www.reinterpretcast.com/writing-a-game-boy-advance-game
  * Typed and modified to create an actual game by Jesse Deisinger
**/

#include "gba_j.h"
#include "/opt/devkitpro/libgba/include/gba.h"
//Form 16-bit BGR GBA color fromthree compoentn values
static inline rgb15 RBG15(int r, int g, int b) {
	return r | (g << 5) | (b << 10);
}

// Set the position of object to specified x and y coordinates
static inline void set_object_position(volatile obj_attrs *object, int x, int y) {
	object->attr0 = (object->attr0 & ~OBJECT_ATTR0_Y_MASK) |
							(y & OBJECT_ATTR0_Y_MASK);
	object->attr1 = (object->attr1 & ~OBJECT_ATTR1_X_MASK) |
							(x & OBJECT_ATTR1_X_MASK);
}

//Clamp a value to given bounds
static inline int clamp(int value, int min, int max) {
	return (value < min ? min : (value > max ? max : value));
}

//Reset ball and paddles
static inline void reset(int *player_x, int *player_y, int *ball_x, int *ball_y) {
	*player_x = 5;
	*player_y = 96;
	*ball_x = 22;
	*ball_y = 96;
}

static inline void writeBallToVram(volatile uint16 *ball_tile_mem) {
/*	for(int i = 0; i < (sizeof(tile_4bpp)/2); ++i) {
		ball_tile_mem[i] = 0x3333;
	}*/
	ball_tile_mem[1] = 0x4443;
	ball_tile_mem[0] = 0x3444;
	ball_tile_mem[3] = 0x4332;
	ball_tile_mem[2] = 0x2334;
	ball_tile_mem[5] = 0x4322;
	ball_tile_mem[4] = 0x2234;
	ball_tile_mem[7] = 0x3222;
	ball_tile_mem[6] = 0x2223;
	ball_tile_mem[9] = 0x3222;
	ball_tile_mem[8] = 0x2223;
	ball_tile_mem[11] = 0x4322;
	ball_tile_mem[10] = 0x2234;
	ball_tile_mem[13] = 0x4332;
	ball_tile_mem[12] = 0x2334;
	ball_tile_mem[15] = 0x4443;
	ball_tile_mem[14] = 0x3444;
}
static inline void writeToTileMem(volatile uint16 *cur_tile_mem,unsigned int map[]) {
			for(int i = 0; i < 16; i++) {
				cur_tile_mem[i] = map[i];
			}
}
static inline void writeScore(volatile uint16 *score_tile_mem, uint8 number) {
	unsigned int zero[] = { 0x1200, 0x0021, 0x0120, 0x0210, 0x0010, 0x0100, 0x0010, 0x0100, 0x0010, 0x0100, 0x0010, 0x0100, 0x0120, 0x0210, 0x1200, 0x0021 };
	unsigned int one[] = { 0x1200, 0x0001, 0x1120, 0x0001, 0x1210, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1110, 0x0111 };
	unsigned int two[] = { 0x1200, 0x0021, 0x0120, 0x0210, 0x0010, 0x0120, 0x0000, 0x0012, 0x1000, 0x0001, 0x2100, 0x0000, 0x0210, 0x0000, 0x1110, 0x0111 };
	unsigned int three[] = { 0x1100, 0x0211, 0x0000, 0x0120, 0x0000, 0x0100, 0x1000, 0x0211, 0x0000, 0x0120, 0x0000, 0x0100, 0x0000, 0x0120, 0x1100, 0x0211 };	
	unsigned int four[] = { 0x0100, 0x0010, 0x0100, 0x0010, 0x2100, 0x0212, 0x1100, 0x0111, 0x0000, 0x0012, 0x0000, 0x0010, 0x0000, 0x0010, 0x0000, 0x0000 };
	unsigned int *map;
	switch(number) {
		case 0:
			map = zero;
			break;
		case 1:
			map = one;
			break;
		case 2: 
			map = two;
			break;
		case 3:
			map = three;
			break;
		case 4:
			map = four;
			break;
		case 5:
			break;
	}
	writeToTileMem(score_tile_mem,map);
}
int main(void) {
	// Write the tiles for our sprites into the fourth tile block in VRAM.
	// Four tiles for an 8x32 paddle sprite, and one tile for an 8x8 ball
	// sprite. Using 4bpp, 0x1111 is four pixels of colour index 1, and
	// 0x2222 is four pixels of colour index 2.
	//
	// NOTE: We're using our own memory writing code here to avoid the
	// byte-granular writes that something like 'memset' might make (GBA
	// VRAM doesn't support byte-granular writes).
	//  (uint16 *)tile_mem[4][1] refers to a pointer to a 16 bit int, located at the 4th
	//  tile_4bpp, which is an array of 8 32 bit ints.
	// So basically, this is the 16 bit address of the first 32 bit int in tile 4
	volatile uint16 *paddle_tile_mem = (uint16 *)tile_mem[4][1];
	volatile uint16 *ball_tile_mem	= (uint16 *)tile_mem[4][5];
	for(int i = 0; i < 4 * (sizeof(tile_4bpp) / 2); ++i) {
		paddle_tile_mem[i] = 0x1111;		
	}
	volatile uint16 *score_tile_mem	= (uint16 *)tile_mem[4][6];
	volatile uint16 *comp_score_tile_mem = (uint16 *)tile_mem[4][7];
	
	writeBallToVram(ball_tile_mem);
	writeScore(score_tile_mem,0);
	//Set the palettes, the first one is white, the second oen is something else
	object_palette_mem[0] = RBG15(0x00, 0x00, 0x00);
	object_palette_mem[1] = RBG15(0x1F, 0x00, 0x00);
	object_palette_mem[2] = RBG15(0x0B, 0x00, 0x00);
	object_palette_mem[3] = RBG15(0x1A, 0x00, 0x1A);
	object_palette_mem[4] = RBG15(0x00, 0x00, 0x00);
	//Create the sprites
	volatile obj_attrs *paddle_attrs = &oam_mem[0];
	paddle_attrs->attr0 = 0x8000;
	paddle_attrs->attr1 = 0x4000;
	paddle_attrs->attr2 = 1;

	volatile obj_attrs *ball_attrs = &oam_mem[1];
	ball_attrs->attr0 = 0;
	ball_attrs->attr1 = 0;
	ball_attrs->attr2 = 5;
	
	volatile obj_attrs *score_attrs = &oam_mem[2];
	score_attrs->attr0 = 0;
	score_attrs->attr1 = 0;
	score_attrs->attr2 = 6;
	
	volatile obj_attrs *comp_score_attrs = &oam_mem[3];
	comp_score_attrs->attr0 = 0;
	comp_score_attrs->attr1 = 0;
	comp_score_attrs->attr2 = 7;
	
	volatile obj_attrs *comp_paddle_attrs = &oam_mem[4];
	comp_paddle_attrs->attr0 = 0x8000;
	comp_paddle_attrs->attr1 = 0x4000;
	comp_paddle_attrs->attr2 = 1;

	//Initialize state variables for paddle/ball, set their start position
	const int 	player_width = 8,
					player_height = 32;
	const int	ball_width = 8,
					ball_height = 8;
	int player_velocity = 2;
	int ball_velocity_x = 2,
		 ball_velocity_y = 1;
	int player_x = 5;
	int player_y = 96;
	int comp_x = 227;
	int comp_y = 96;
	int ball_x = 22;
	int ball_y = 96;

	set_object_position(paddle_attrs, player_x, player_y);
	set_object_position(comp_paddle_attrs, comp_x, comp_y);
	set_object_position(ball_attrs, ball_x, ball_y);
	set_object_position(score_attrs, 80, 0);
	set_object_position(comp_score_attrs, 160, 0);

	REG_DISPLAY = 0x1000 | 0x0040;
	uint32 key_states = 0;
	uint8 comp_score = 0;
	uint8 player_score = 0;
	writeScore(comp_score_tile_mem,comp_score);
	writeScore(score_tile_mem,player_score);
	while(1) {
		// Skip past the rest of any current V-Blank, then skip past
		// the V-Draw
		while(REG_DISPLAY_VCOUNT >= 160);
		while(REG_DISPLAY_VCOUNT <  160);
		// Get current key states (REG_KEY_INPUT stores the states
		// inverted)
		key_states = ~REG_KEY_INPUT & KEY_ANY;

		// Note that our physics update is tied to the framerate,
		// which isn't generally speaking a good idea. Also, this is
		// really terrible physics and collision handling code.
		int player_max_clamp_y = SCREEN_HEIGHT - player_height;
		if (key_states & KEY_UP)
			player_y = clamp(player_y - player_velocity, 0,
			                 player_max_clamp_y);
		if (key_states & KEY_DOWN)
			player_y = clamp(player_y + player_velocity, 0,
			                 player_max_clamp_y);
		if (key_states & KEY_UP || key_states & KEY_DOWN)
			set_object_position(paddle_attrs, player_x, player_y);

		int ball_max_clamp_x = SCREEN_WIDTH  - ball_width,
		    ball_max_clamp_y = SCREEN_HEIGHT - ball_height;
		if ((ball_x >= player_x &&
		     ball_x <= player_x + player_width) &&
		    (ball_y >= player_y &&
		     ball_y <= player_y + player_height)) {
			ball_x = player_x + player_width;
			ball_velocity_x = -ball_velocity_x;
		} 
		else if ((ball_x <= comp_x &&
		     ball_x >= comp_x - player_width) &&
		    (ball_y >= comp_y &&
		     ball_y <= comp_y + player_height)) {
			ball_x = comp_x - player_width;
			ball_velocity_x = -ball_velocity_x;
		} else {
			if (ball_x == 0 || ball_x == ball_max_clamp_x) {
					if(ball_x == 0) {
						reset(&player_x,&player_y,&ball_x,&ball_y);
						set_object_position(paddle_attrs, player_x, player_y);
						writeScore(comp_score_tile_mem,++comp_score);
					}
					else {
						reset(&player_x,&player_y,&ball_x,&ball_y);
						set_object_position(paddle_attrs, player_x, player_y);
						writeScore(score_tile_mem,++player_score);
					}
					ball_velocity_x = -ball_velocity_x;
			}
			else if (ball_x > (SCREEN_WIDTH/2)) {
				if(ball_y > comp_y) {
					comp_y = clamp(comp_y + player_velocity,0,player_max_clamp_y);
				}
				else if(ball_y < comp_y) {
					comp_y = clamp(comp_y - player_velocity,0,player_max_clamp_y);
				}
				set_object_position(comp_paddle_attrs, comp_x, comp_y);
			}
			if (ball_y == 0 || ball_y == ball_max_clamp_y)
				ball_velocity_y = -ball_velocity_y;
		}

		ball_x = clamp(ball_x + ball_velocity_x, 0, ball_max_clamp_x);
		ball_y = clamp(ball_y + ball_velocity_y, 0, ball_max_clamp_y);
		set_object_position(ball_attrs, ball_x, ball_y);
	}
	return 0;
}
