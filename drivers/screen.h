#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
// Attribute byte for our default colour scheme .
#define WHITE 0xf
#define YELLOW 0xe
#define BRIGHT_MAGENTA 0xd
#define BRIGHT_RED 0xc
#define BRIGHT_CYAN 0xb
#define BRIGHT_GREEN 0xa
#define BRIGHT_BLUE 0x9
#define DARK_GRAY 0x8
#define GRAY 0x7
#define BROWN 0x6
#define MAGENTA 0x5
#define RED 0x4
#define CYAN 0x3
#define GREEN 0x2
#define BLUE 0x1
#define BLACK 0x0



// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void clear_screen();                                //clear screen
void kprint_at(char *message, int col, int row);    //print message starting at ...
void kprint(char *message);
void print_coloristic();