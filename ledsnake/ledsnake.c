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
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                x"
	"x                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                x"
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
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx         x"
	"x         x                                                                                                          x         x"
	"x         x                                                                                                          x         x"
	"x         x      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx      x         x"
	"x         x      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx      x         x"
	"x         x                                                                                                          x         x"
	"x         x                                                                                                          x         x"
	"x         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx         x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
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
	,
	{
	16,
	"xxxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x               x                                                                                                              x"
	"x               x                                                                                                              x"
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
	"xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               x                              x                              x                              x                 x"
	"x               xxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxxxxx"
	"x                              x                              x                              x                                 x"
	"x                              x                              x                              x                                 x"
	"x                              x                              x                              x                                 x"
	"x                              x                              x                              x                                 x"
	"x                              x                              x                              x                                 x"
	"x                              x                              x                              x                                 x"
	"xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx              xxxxxxxxxxxxxxxxx                 x"
	}
	,
	{
	16,
	"                                                                                                                                "
	"                xxxxxxxxxxxxxxxx                                              xxxxxxxxxxxxxxxx                                  "
	"                x              x                                              x              x                                  "
	"                x              x                                              x              x                                  "
	"                x              x                                              x              x                                  "
	"                x              x                                              x                             xxxxxxxxxxxxxxxxx   "
	"                x              x               x     xxxxxxxxxx               x                             x               x   "
	"                x              x               x              x               x                             x               x   "
	"                x              x               x              x               x                             x               x   "
	"                               x               x              x               xxxxxxxxxxxxxxxx              x               x   "
	"                               x               x              x                                             x               x   "
	"                               x               x              x                                             x               x   "
	"                               x               x              x                                             x               x   "
	"                xxxxxxxxxxxxxxxx               x              x                                             x               x   "
	"                                               x              x                                             x               x   "
	"                                               xxxxxxxxxxxxxxxx                                             xxxxxxxxxxx     x   "
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"x         x x  x                                                                                                               x"
	"        x          x                                                               x                                            "
	"       x     xxx                                                                                         x                      "
	"           x    x    x   x                               x                                                                      "
	"       x        x                                                   x                                                  x        "
	"         x     x                                                                                                                "
	"           x x      x                                                      x               x                                    "
	"                         x                   x                                                                                  "
	"                                                       x                                                                        "
	"                                     x                xxx                                                                       "
	"                                                       x                                                          xx            "
	"        x       x                                                                 x      x                      xxxxxx          "
	" x                       x                                                                             x          xx            "
	"                                                              x        x                                                        "
	"                                                      x                                      x                                  "
 	"                                 x                                                  x                                           "
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                xxxxxx                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxx                                       xxxxxxxxxxxx                                       xxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxx                                             xxxxxxxxxxxxxx                                             xxxxxxxxxxxx"
	"xxxxxxx                                                 xxxxxxxxxxxxxxxx                                                 xxxxxxx"
	"xxxx                                                     xxxxxxxxxxxxxx                                                     xxxx"
	"xx                                                        xxxxxxxxxxxx                                                        xx"
	"                                                             xxxxxx                                                             "
	"                                                              xxxx                                                              "
	"xx                                                             xx                                                             xx"
	"xxxx                                                          xxxx                                                          xxxx"
	"xxxxxxx                                                       xxxx                                                       xxxxxxx"
	"xxxxxxxxxxxx                                                   xx                                                   xxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxx                                           xxxx                                           xxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxx                             xxxxxxxxxxxx                             xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       xxxxxxxxxxxxxxxxxxxx       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    "
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx            "
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                     "
	"                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                             "
	"                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                     "
	"                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                             "
	"                                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                      "
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                              "
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                      "
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                              xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxx                                                                       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxx                                                                                                               "
	"xxxxxxxx                                                                                                                        "
	"                                                                                                                                "
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxx       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxx         xxxxxxxxxxxxxxxxxxxxxxxx             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xx           xxxxxxxxxxxxxxxxxxx                  xxxxxxxxxxxxxxxxxxxxxxxxxxxxx                      xxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x             xxxxxxxxxxxxxx                       xxxxxxxxxxxxxxxxxxxxxxxxxxx                         xxxxxxxxxxxxxxxxxxxxxxxxx"
	"x             xxxxxx                                xxxxxxxxxxxxxxxxxxxxxxxx                             xxxxxxxxxxxxxxxxxxxxxxx"
	"                                         xxxx         xxxxxxxxxxxxxxxxxxx           xxxxxxxxxx               xxxxxx             "
	"                                     xxxxxxxxxx        xxxxxxxxxxxxxxx             xxxxxxxxxxxxxx                               "
	"                                 xxxxxxxxxxxxxxx         xxxxxxxxxx              xxxxxxxxxxxxxxxxxxx                            "
	"                             xxxxxxxxxxxxxxxxxxxx          xxxx                xxxxxxxxxxxxxxxxxxxxxxx                          "
	"x                   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                          xxxxxxxxxxxxxxxxxxxxxxxxxxx                  xxxxxx"
	"x             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       xxxxxxxxxxxxx"
	"xx           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxx         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxx       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx        xxxxxxxxxxxxxx         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxx       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx             xxxxxxxxx             xxxxxxxxxxxxxxxxxxxxxx            xxxxxxxxxxxxxxx"
	"xxx         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               xxxxxxx               xxxxxxxxxxxxxxxxxx                 xxxxxxxxxxxxx"
	"xx           xxxxxxxxxxxxx        xxxxxxx                  xxxxx                xxxxxxxxxxxxxxxxx                    xxxxxxxxxxx"
	"x             xxxx                    xx                   xxxxx                 xxxxxxxxxxxxxx                       xxxxxxxxxx"
	"x             x                               xxxx         xxxx       xxxxx      xxxxxxxxxxxxx          xxxxxxxxx      xxxxxxxxx"
	"                                            xxxxxxxx       xxx       xxxxxxx     xxxxxxxxxxxx         xxxxxxxxxxxxx     xxxxxxxx"
	"                                           xxxxxxxxx      xxxx      xxx   xx      xxxxxxxxx         xxxxxx    xxxxxx            "
	"                          xxxxxxx          xxxxxxxxxx    xxxx      xxx     xx      xxxxxx         xxxxxx       xxxxxx           "
	"                   xxxxxxxxxxxxxxxxx         xxxxxxxxxxxxxxx               xx                    xxxxxx        xxxxxxx          "
	"x             xxxxxxxxxxxxxxxxxxxxxxxx          xxxxxxxxxx                  xx                   xxxxxx        xxxxxxx      xxxx"
	"x             xxxxxxxxxxxxxxxxxxxxxxxxx                                     xx                     xxxx        xxxxxx       xxxx"
	"xx           xxxxxxxxxxxxxxxxxxxxxxxxxxx                        xxx         xxx             xx        xxxxx    xxxxxx      xxxxx"
	"xxx         xxxxxxxxxxxxxxxxxxxxxxxxxxxxx                      xxxxxx       xxxxx         xxxxx        xxxx    xxxxxx      xxxxx"
	"xxxx       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                  xxxxxxxxx    xxxxxxxxxxxxxxxxxxxxx               xxxxxxx    xxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx            xxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"     xxxxxxxxxxxxxxx   xxxxxxxxxxxxxxxxxxxxxx        x          xxx          xxxxxxx        xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"    xxxx  xxx   xxxx  xxxx    xx    xx    xx                   x               xxx         xxxxxxxx    xx    xx    xx    xx    x"
	"  xxxxx    xx   xxxxxxxxxx    xx    xx    xx                                          xxxxxxxxxxxxx    xx    xx    xx    xx    x"
	"xxxxxx      x   xxxxxxxxxx                xxx     x         x          x  xxx       xxxxxxxxxxx  xx                            x"
	"xxx             xxxxxxxxxx                xxxx  xxxxx     xxxxx    x  xxxxxxxx     xxxxxxxx      xx                            x"
	"xxx             xxxxxxxxxx                xxxxxxxxxxxxx  xxxxxxxx  xxxxxxxxxxxxxxxxxxxxxxxx      xx      xx      xx      xx    x"
	"xxx     xx       xxxxxxxxxxx            xxxxxxxxxxxxxxx  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  xx     xxxx    xxxx    xxxx   x"
	"xx     xxxx       xxxxxxxxxxx x      x xxxxxxxxxxxxxxx    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  xx     xxxx    xxxx    xxxx   x"
	"x      xxxx        xxxxxxxxxx          xxxxxxxxxxxxxxx     xxxxxxxxxxxxxxx  xxxxx    xx    xx    xx                            x"
	"                    xxxxxxxxx          xxxxxxxxxxxxxx      xxxxxxxxxxxx       xxx    xx    xx    xx                            x"
	"xx                xx                         xxxxxxx        xxxxxxxxxxx        xx                xx      xxxx                  x"
	"xx                xx                         xxxxxxx         xxxxxxxxx         xxx              xx      x    x                 x"
	"xx                xx    x    x    x    x   xxxxxxx             xxxxxxx         xxxx   x    x   xx       x    x                 x"
	"xx                xx                       xxxxxxxxxxxx   xxxxxxxxxxxxxx     xxxxxx            xx       x    x                 x"
	"xx                                         xxxxxxxxxxxx   xxxxxxxxxxxxxxx   xxxxxxx                     x    x      x     x    x"
	"xx                                                              xxxxxxxxx   xxxxxxx                                 xxxxxxx    x"
	}
	,
	{
	16,                                                                                                                                
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"                                                                                                                                "
	"xxxxxxxxxx                                 xxxxxxxxxxxxx                                xxxxxxxxxxxxx                           "
	"xxxxxxxxxxxx                            xxxxxxxxxxxxxxxxxx                           xxxxxxxxxxxxxxxxxx                         "
	"xxxxxxxxxxxxx                        xxxxxxxxxxxxxxxxxxxxxx                        xxxxxxxxxxxxxxxxxxxxx                        "
	"xxxxxxxxxxxxx                      xxxxxxxxxxxxxxxxxxxxxxxx                     xxxxxxxxxxxxxxxxxxxxxxxxx                      x"
	"xxxxxxxxxxxxxx                  xxxxxxxxxxxxxxxxxxxxxxxxxxx                   xxxxxxxxxxxxxxxxxxxxxxxxxxx                    xxx"
	"xxxxxxxxxxxxx                 xxxxxxxxxxxxxxxxxxxx  xxxxxx                  xxxxxxxxxxxxxxxxxxxx  xxxxxxx                  xxxxx"
	"xxx  xxxxxxx               xxxxxxxxxxxxxxxxxxxxx    xxxxx                 xxxxxxxxxxxxxxxxxxxxx   xxxxxx                 xxxxxxx"
	"xx    xxx                xxxxxxxxxxxxxxxxxxxxxxx                       xxxxxxxxxxxxxxxxxxxxxxx                         xxxxxxxxx"
	"xx                     xxxxxxxxxxxxxxxxxxxxxxxxx                    xxxxxxxxxxxxxxxxxxxxxxxxxxx                     xxxxxxxxxxxx"
	"xx                  xxxxxxxxxxxxxxxxxxxxxxxxxxxxx                 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                  xxxxxxxxxxxxxx"
	"xxxx            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx          xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx           xxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"                      x                                                                                                         "
	"xxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"               x                                                                                                                "
	"               x                                                                                                                "
	"xxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"        x                                                                                                                       "
	"        x                                                                                                                       "
	"        x                                                                                                                       "
	"x       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"x                                                                                                                              x"
	"xxxxxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	"xxxxxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
	,
	{
	16,
	"xxxxxxxxxxxxxxxxxxxxxxxxx   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx x xxxxxxxxxxxxx"
	"x                           x                              x                                                                   x"
	"x                           x                              x                                                                   x"
	"x                           x                              x                                                                   x"
	"x   xxxxxxxxxxxxxxxxxxxxxxxxx       x   xxxxxxxxxxxxxxx    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       xxxxxxxxxxxxx   x"
	"x   x                               x       x                                                          x       x           x   x"
	"x   x                               x       x                                                          x       x           x   x"
	"x   x                               x       x                                                          x       x           x   x"
	"x   x   x               xxxxxxxxxxxxxxxxx   x    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx   x    xxxx   xxxxx   x   x"
	"x   x                   x                   x                                                      x   x       x   x       x   x"
	"x   x       xxxxxxxxxxxxx                   x                                                      x   x       x   x       x   x"
	"x   x                   x                   x    xxxxxxxxxxx    x                                  x   x       x   x       x   x"
	"x                       x   xxxxxxxxxxxxxxxxx              x    x                                  x   xxx     x   xxxxxxxxx   x"
	"x                       x   x                              x    x                                              x               x"
	"x                       x   x                              x    x                                              x               x"
	"xxxxxxxxxxxxxxxxxxxxxxxxx   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               x"
	}
	,
	{
	16,
	"                                                                                          xxxxxxxxxxxxxxxx                      "
	"                                                                                         xx              xx                     "
	"                                                                                         x     x    x     x                     "
	"                                                                                         x      x   x     x                     "
	"      xxxxxx         xxxxxx         xxxxxx        x   xx   xxxx   xxxx  xxxx  xxx  x     x       x   x              xx          "
	"     x     x        x     x        x     x        x   x        x  x     x     x    x     x        x   xxxxxxxxxxxxxxx xxx       "
	"    x              x              x               x  x         x  x     x     x    x     x         x             xx x x x       "
	"   x              x              x                x  x         x  x     x     x    x     x          xxxxxxxxxxxxxxxx xx xx      "
	"   x              x              x                xxx      xx  x   xx    xx   xx   x     x          xxxxxxxxxxxxxxxxx   xx      "
	"   x              x              x                xxx     x    x     x     x  x    x     x         x              xxx  xxx      "
	"   x              x              x                x  x    x    x     x     x  x    x     x        x   xxxxxxxxxxxxxxxxx xx      "
	"    x              x              x               x  x    x    x     x     x  x    x     x       x   x               xxx x      "
	"     x     x        x     x        x     x        x   x   x    x     x     x  x    x     x      x   x     x       xxxx xx       "
	"      xxxxxx         xxxxxx         xxxxxx        x   xx   xxxx   xxxx  xxxx  xxx  xxx   x     x    x     x        xxxx  x      "
	"                                                                                         xx              xx           xxxx      "
	"                                                                                          xxxxxxxxxxxxxxxx           xx x       "
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
