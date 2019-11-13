void entrypoint() {

}

void main(){
    char* video_memory = (char*)0xb800A;
    *video_memory = 'X';
}