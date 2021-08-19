#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

struct termios orig_termios;

void die(const char *s){
	perror(s);  //for displaying the error
	exit(1);
}

void disableRawMode(){
	if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios) == -1) 
		die("tcsetattr");  
} 

void enableRawMode(){
	
	if(tcgetattr(STDIN_FILENO,&orig_termios) == -1)
	 die("tcsetattr");

	atexit(disableRawMode);   //resets to default state after program execution

	struct termios raw = orig_termios;

	raw.c_iflag &= ~(ICRNL | IXON);  //turns off software flow control and carriage return for new line
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);   //does not apply to modern terminal emulators still
	raw.c_oflag &= ~(OPOST);  // disables carriage + new line 
	raw.c_cflag |= (CS8); 
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //turns off canonical sigint and sigstp
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1)  //TCS Flushesh left input buffer and sets terminal environement to original
		die("tcssetattr");
}
int main(){
	enableRawMode();   //sets the echo mode as off
	while(1){
		char c = '\0';
		if(read(STDIN_FILENO,&c,1) == -1 && errno != EAGAIN) die("read");

		if(iscntrl(c)){
			printf("%d\r\n",c);
		} else{
			printf("%d ('%c')\r\n",c,c);
		}
		if(c=='q') break;
	} //reads till character q or EOF
	return 0;
}