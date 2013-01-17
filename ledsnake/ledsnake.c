#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>


enum { LEFT, RIGHT, UP, DOWN };
#define HEIGHT 16
#define WIDTH 128

char *host = "10.0.1.242";
int port = 1021;
int dopause = 0;

#define MAXLENGTH 256
#define LENGTH(x)               sizeof(x)/sizeof(x[0])

struct { int size; char map[2049]; } levels[] = {
	{
	16,
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x               x                                                                                                              x"
	"x               x                                                                                                              x"
	"x               x                                                                                                              x"
	"x               x                                                                                                              x"
	"x               x                                                                                                              x"
	"x               xxxxxxxxxxxxxxxx                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"x                              x                                                                                               x"
	"xxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	-1,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x               x                                x                                x                                x           x"
	"x               x                                x                                x                                x           x"
	"x               x                                x                                x                                x           x"
	"x               x                                x                                x                                x           x"
	"x               x                                x                                x                                x           x"
	"x               x                x               x                x               x                x               x           x"
	"x               x                x               x                x               x                x               x           x"
	"x               x                x               x                x               x                x               x           x"
	"x               x                x               x                x               x                x               x           x"
	"x                                x                                x                                x                           x"
	"x                                x                                x                                x                           x"
	"x                                x                                x                                x                           x"
	"x                                x                                x                                x                           x"
	"x                                x                                x                                x                           x"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
};

int level = 1;
char schoord[MAXLENGTH][2] = { {HEIGHT/2, WIDTH/2} };
int direction = UP;
int startOffset=0, endOffset=1, size = 1;
struct termios old_tio, new_tio;
int alive = 1;
char berry[2];

static void draw();
static int istaken(char choord[2]);
static void die(const char *s);
static void move();
static void configterminal();
static void handleinput();
static void hideberry();
static int play(int l);


void
draw() {
	unsigned char field[HEIGHT*2][WIDTH/8] = { { 0 } };
	struct sockaddr_in si_other;
	int s, i;
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		die("socket");
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	if(inet_aton(host, &si_other.sin_addr) == 0)
		die("inet_aton");

	for(i = startOffset; i != endOffset; i = (i+1) % (MAXLENGTH)) {
		field[schoord[i][0]*2 + 1][schoord[i][1] / 8] |= 1 << (schoord[i][1] % 8);
		field[schoord[i][0]*2][schoord[i][1] / 8] |= 1 << (schoord[i][1] % 8);
	}
	for(i = 0; i < WIDTH * HEIGHT; i++) {
		if(levels[level].map[i] != ' ') {
			field[(i / WIDTH) * 2][(i % WIDTH) / 8] |= 1 << ((i % WIDTH) % 8);
		}
	}

	field[berry[0]*2 + 1][berry[1] / 8] |= 1 << (berry[1] % 8);

	if (sendto(s, field, sizeof(field), 0, (struct sockaddr *)&si_other, sizeof(si_other))==-1)
		die("send");
	close(s);
}

int
istaken(char choord[2]) {
	int i;
	for(i = startOffset; i != endOffset; i = (i+1) % (MAXLENGTH)) {
		if(schoord[i][0] == choord[0] && schoord[i][1] == choord[1])
			return 1;
	}
	if(levels[level].map[choord[0] * WIDTH + choord[1]] != ' ') {
		return 1;
	}
	return 0;
}

void
die(const char *s) {
	perror(s);
	exit(0);
}

void
move() {
	int newOffset = endOffset;
	int oldOffset = (endOffset - 1 + MAXLENGTH) % MAXLENGTH;
	switch(direction) {
	case RIGHT:
		schoord[newOffset][0] = schoord[oldOffset][0];
		schoord[newOffset][1] = (schoord[oldOffset][1] + 1) % WIDTH;
		break;
	case LEFT:
		schoord[newOffset][0] = schoord[oldOffset][0];
		schoord[newOffset][1] = (schoord[oldOffset][1] + WIDTH - 1) % WIDTH;
		break;
	case DOWN:
		schoord[newOffset][0] = (schoord[oldOffset][0] + 1) % HEIGHT;
		schoord[newOffset][1] = schoord[oldOffset][1];
		break;
	case UP:
		schoord[newOffset][0] = (schoord[oldOffset][0] + HEIGHT - 1) % HEIGHT;
		schoord[newOffset][1] = schoord[oldOffset][1];
		break;
	}
	if(istaken(schoord[newOffset])) {
		alive = 0;
	}
	if(schoord[newOffset][0] == berry[0] && schoord[newOffset][1] == berry[1]) {
		hideberry();
		size++;
	}
	else {
		startOffset = (startOffset+1) % MAXLENGTH;
	}
	endOffset = (endOffset+1) % MAXLENGTH;
}


void
configterminal() {
	tcgetattr(STDIN_FILENO,&old_tio);
	new_tio=old_tio;
	new_tio.c_lflag &=(~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
}

void
handleinput() {
	fd_set fds;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	if((select(1, &fds, NULL, NULL, dopause ? NULL : &timeout)) == -1)
		die("select failed");
	if(!FD_ISSET(0, &fds))
		return;
	dopause = 0;
	switch(getchar()) {
	case 'a':
		if(direction != RIGHT)
			direction = LEFT;
		break;
	case 'd':
		if(direction != LEFT)
			direction = RIGHT;
		break;
	case 'w':
		if(direction != DOWN)
			direction = UP;
		break;
	case 's':
		if(direction != UP)
			direction = DOWN;
		break;
	case 'p':
		dopause = 1;
		break;
	}
}

void
hideberry() {
	srand(time(NULL));
	do {
		berry[0] = rand() % HEIGHT;
		berry[1] = rand() % WIDTH;
	} while(istaken(berry));
}

int
play(int l) {
	dopause = 1;
	level = l;
	hideberry();
	direction = LEFT;
	schoord[0][0] = HEIGHT/2;
	schoord[0][1] = WIDTH/2;
	startOffset = 0;
	endOffset = 1;
	size = 1;
	while(alive) {
		draw();
		usleep(100000);
		handleinput();
		move();
		if(startOffset == endOffset || size == levels[level].size)
			return 1;
	}
	return 0;
}

int
main(int argc, char *argv[]) {
	configterminal();
	int i;
	for(i = 0; i < LENGTH(levels); i++) {
		if(!play(i))
			break;
	}
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
	return 0;
}
