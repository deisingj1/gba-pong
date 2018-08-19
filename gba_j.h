//You know what this is, GBA screen res
#define SCREEN_WIDTH		240
#define SCREEN_HEIGHT	160

//IO memory start address
#define MEM_IO 	0x04000000
//Palette Memory start address
#define MEM_PAL 	0x05000000
//VRAM memory start address
#define MEM_VRAM	0x06000000
//Object attribute memory start address
#define MEM_OAM	0x07000000

//Pointer to display register? look this up
#define REG_DISPLAY			(*((volatile uint32 *)(MEM_IO)))
//I think  this is tracking what line was just drawn ?
#define REG_DISPLAY_VCOUNT	(*((volatile uint32 *)(MEM_IO + 0x0006)))
//I assume this register is populated with keypress data?
#define REG_KEY_INPUT		(*((volatile uint32 *)(MEM_IO + 0x0130)))

//Are these the values that the KEY INPUT reg get set to for each key?
//Yes, KEY_INPUT stores the inverse and this will match the whole
//Register
#define KEY_ANY 0x03FF

//This I have no idea. Something with the sprites?
#define OBJECT_ATTR0_Y_MASK 0x0FF
#define OBJECT_ATTR1_X_MASK 0x1FF
//These are aliases for different types, and a struct for object attribs?
typedef unsigned char 	uint8;
typedef unsigned short 	uint16;
typedef unsigned int		uint32;
typedef uint16 rgb15;
typedef struct obj_attrs {
	uint16 attr0;
	uint16 attr1;
	uint16 attr2;
	uint16 pad;
} __attribute__((packed, aligned(4))) obj_attrs;
typedef uint32		tile_4bpp[8];
typedef tile_4bpp	tile_block[512];

#define oam_mem					((volatile obj_attrs *)MEM_OAM)
#define tile_mem					((volatile tile_block *)MEM_VRAM)
#define object_palette_mem 	((volatile rgb15 *)(MEM_PAL + 0x200))
