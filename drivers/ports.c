unsigned char port_byte_in(unsigned short port){
    unsigned char result;   

    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    //byte from port in dx goes to al register

 return result;
}

void port_byte_out(unsigned short port, unsigned char data){
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned char port_word_in(unsigned short port){
    unsigned char result;   

    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    //word from port in dx goes to al register

 return result;
}

void port_word_out(unsigned short port, unsigned char data){
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}