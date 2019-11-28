#include "screen.h"
#include "ports.h"
#include "../libc/mem.h"

int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

int color_mode(int background, int foreground){
    return background<<4|foreground;
}

void print_coloristic(){
    int i=0, j=0;
    for(i; i<16; i++){
        for(j=0; j<16;j++){
            print_char(' ', j, i, color_mode(j, i));
        }
    }
}

void kprint_at(char *message, int col, int row, char attr) {
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* Loop through message and print it */
    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, attr);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(char *message, char attr) {
    kprint_at(message, -1, -1, attr);
}

void kprint_backspace() {
    int offset = get_cursor_offset()-2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(0x08, col, row, color_mode(BLACK,WHITE));

}

int print_char(char c, int col, int row, char attr) {
    unsigned char *video_mem = (unsigned char*) VIDEO_ADDRESS;
    if (!attr) attr = color_mode(BLACK, WHITE); //set default color if not set

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        //letter | color (because of LE)
        video_mem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        video_mem[2*(MAX_COLS)*(MAX_ROWS)-1] = (BLACK, BRIGHT_RED);
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } 
    else if (c==0x08){
        video_mem[offset] = ' ';
        video_mem[offset+1] = attr;
    }
    else {
        video_mem[offset] = c;
        video_mem[offset+1] = attr;
        offset += 2;
    }

    // Check if offset is greater than screen size and scroll if necessary
    if(offset >= MAX_ROWS*MAX_COLS*2){
        int i=1;
        for(i; i<MAX_ROWS; i++){
            memory_copy(get_offset(0, i) + VIDEO_ADDRESS, 
                        get_offset(0,i-1) + VIDEO_ADDRESS, 
                        MAX_COLS*2);             //copy whole line
        }

        char *last_line = get_offset(0, MAX_ROWS-1)+VIDEO_ADDRESS;
        for(i = 0; i< MAX_COLS*2; i++) last_line[i]=0;
        offset -= 2*MAX_COLS;
    }

    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = color_mode(BLACK, WHITE);
    }
    set_cursor_offset(get_offset(0, 0));
}


int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }