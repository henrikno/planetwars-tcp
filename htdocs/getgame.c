#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cgi.h"

const char *fn = "results.pgn";

/*
[White "dhartmei"]
[Black "example"]
[Result "1-0"]
1283931245
maps/map3.txt
0.126,1.15,2.50,0.16,0.16,0.67,0.67,0.6,0.6,0.13,0.13,0.17,0.17,0.19,0.19,0.21,0
.21,0.74,0.74,0.23,0.23,0.8,0.8,1.1.1.2.23,2.50.2.7.18,1.1.1.3.19,1.1.1.4.15,1.1
.1.5.16,1.1.1.6.10,1.1.1.7.16,1.1.1.8.18,1.1.1.9.9,1.1.1.10.15,1.1.1.11.22,1.1.1
.12.8,1.1.1.13.5,1.1.1.14.22,1.1.1.15.4,1.1.1.16.21,1.1.1.17.24,1.1.1.18.6,1.1.1
.19.11,1.1.1.20.13,1.1.1.21.6,1.1.1.22.27,1.1.1.2.23,1.1.1.3.19,1.1.1.4.15,1.1.1
.5.16,1.1.1.6.10,1.1.1.7.16,1.1.1.8.18,1.1.1.9.9,1.1.1.10.15,1.1.1.13.5,1.1.1.14
.22,1.1.1.15.4,1.1.1.16.21,1.1.1.17.24,1.1.1.18.6,1.1.1.19.11,1.1.1.20.13,1.1.1.
21.6,1.1.1.22.27,1.1.1.2.23,1.1.1.3.19,1.1.1.4.15,1.1.1.5.16,1.1.1.6.10,1.1.1.7.
16,1.1.1.8.18,1.1.1.9.9,1.1.1.10.15,1.1.1.13.5,1.1.1.14.22,1.1.1.15.4,1.1.1.16.2
1,1.1.1.17.24,1.1.1.18.6,1.1.1.19.11,1.1.1.20.13,1.1.1.21.6,1.1.1.22.27,1.1.1.2.
23,1.1.1.3.19,1.1.1.4.15,1.1.1.5.16,1.1.1.6.10,1.1.1.9.9,1.1.1.10.15,1.1.1.15.4,
1.1.1.16.21,1.1.1.19.11,1.1.1.20.13,1.1.1.21.6,1.1.1.22.27,1.1.1.2.23,1.1.1.9.9,
1.1.1.10.15,1.1.1.15.4,1.1.1.16.21,1.1.1.2.23,1.1.1.9.9,1.1.1.10.15,1.1.1.15.4,1
.1.1.16.21,1.1.1.2.23,1.1.1.9.9,1.1.1.10.15
0.126,1.20,2.55,0.16,0.16,0.67,0.67,0.6,0.6,0.13,0.13,0.17,0.17,0.19,0.19,0.21,0
.21,0.74,0.74,0.23,0.23,0.8,0.8,1.1.1.2.22,2.50.2.7.17,1.1.1.3.18,1.1.1.4.14,1.1
.1.5.15,1.1.1.6.9,1.1.1.7.15,1.1.1.8.17,1.1.1.9.8,1.1.1.10.14,1.1.1.11.21,1.1.1.
12.7,1.1.1.13.4,1.1.1.14.21,1.1.1.15.3,1.1.1.16.20,1.1.1.17.23,1.1.1.18.5,1.1.1.
19.10,1.1.1.20.12,1.1.1.21.5,1.1.1.22.26,1.1.1.2.22,1.1.1.3.18,1.1.1.4.14,1.1.1.
5.15,1.1.1.6.9,1.1.1.7.15,1.1.1.8.17,1.1.1.9.8,1.1.1.10.14,1.1.1.13.4,1.1.1.14.2
1,1.1.1.15.3,1.1.1.16.20,1.1.1.17.23,1.1.1.18.5,1.1.1.19.10,1.1.1.20.12,1.1.1.21
.5,1.1.1.22.26,1.1.1.2.22,1.1.1.3.18,1.1.1.4.14,1.1.1.5.15,1.1.1.6.9,1.1.1.7.15,
1.1.1.8.17,1.1.1.9.8,1.1.1.10.14,1.1.1.13.4,1.1.1.14.21,1.1.1.15.3,1.1.1.16.20,1
.1.1.17.23,1.1.1.18.5,1.1.1.19.10,1.1.1.20.12,1.1.1.21.5,1.1.1.22.26,1.1.1.2.22,
1.1.1.3.18,1.1.1.4.14,1.1.1.5.15,1.1.1.6.9,1.1.1.9.8,1.1.1.10.14,1.1.1.15.3,1.1.
1.16.20,1.1.1.19.10,1.1.1.20.12,1.1.1.21.5,1.1.1.22.26,1.1.1.2.22,1.1.1.9.8,1.1.
1.10.14,1.1.1.15.3,1.1.1.16.20,1.1.1.2.22,1.1.1.9.8,1.1.1.10.14,1.1.1.15.3,1.1.1
.16.20,1.1.1.2.22,1.1.1.9.8,1.1.1.10.14
*/

const char *
timestamp(time_t t)
{
	static char s[64];
	struct tm *tm = gmtime(&t);
	snprintf(s, sizeof(s), "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min, tm->tm_sec);
	return (s);
}

int main()
{
	struct query *q = get_query();
	const char *game_id = get_query_param(q, "game_id");
	FILE *f, *fm;
	char s[8192], p1[256], p2[256], q1[256], q2[256], id[256], r[256];
	int w, h;

	memset(id, 0, sizeof(p1));
	memset(q1, 0, sizeof(p1));
	memset(q2, 0, sizeof(p1));
//	printf("HTTP/1.0 200 OK\n");
	printf("Content-Type: text/plain\n");
	printf("Connection: close\n");
	printf("\n");

/*
f = fopen("441", "r");
while (fgets(s, sizeof(s), f))
printf("%s", s);
return 0;
*/



	if (strchr(game_id, '|')) {
		if (sscanf(game_id, "%255[^|]|%255[^|]|%255[^|]",
		    id, q1, q2) != 3) {
			printf("Error: invalid game_id %s\n", game_id);
			goto done;
		}
	} else
		strlcpy(id, game_id, sizeof(id));
	f = fopen(fn, "r");
	while (fgets(s, sizeof(s), f)) {
		if (!strncmp(s, "[White \"", 8)) {
			memset(p1, 0, sizeof(p1));
			sscanf(s + 8, "%255[^\"]", p1);
		}
		if (!strncmp(s, "[Black \"", 8)) {
			memset(p2, 0, sizeof(p2));
			sscanf(s + 8, "%255[^\"]", p2);
		}
		if (!strncmp(s, "[Result \"", 9)) {
			memset(r, 0, sizeof(r));
			sscanf(s + 9, "%255[^\"]", r);
		}
		if (!strncmp(s, id, strlen(id)) &&
		    (!q1[0] || !strcmp(p1, q1)) &&
		    (!q2[0] || !strcmp(p2, q2))) {
			/* map filename */
			fgets(s, sizeof(s), f);
			s[strlen(s) - 1] = 0;
			int mapid = 0;
			sscanf(s, "maps/map%d.txt", &mapid);
			printf("game_id=%s\nwinner=0\nloser=0\nmap_id=%d\n"
			    "draw=%s\n", id, mapid,
			    strcmp(r, "1/2-1/2") ? "0" : "1");
			printf("timestamp=%s\nplayer_one=%s\nplayer_two=%s\n",
			    timestamp(atol(id)), p1, p2);
/*
game_id=4414272 winner=123465 loser=123485 map_id=741 draw=0
timestamp=2010-09-08 01:42:00 player_one=phreeza player_two=UloPee
playback_string=
*/

			if ((fm = fopen(s, "r")) == NULL) {
				printf("Error: fopen: %s: %s",
				    fm, strerror(errno));
				goto done;
			}
			printf("playback_string=");
			int planets = 0;
			while (fgets(s, sizeof(s), fm)) {
				double x, y;
				int owner, ships, growth;
/*
P 11.6742243424 11.649441005 0 56 3
P 15.5644784882 23.29888201 1 100 5
P 7.7839701966 0.0 2 100 5
*/
				if (s[0] != 'P' || s[1]!= ' ' ||
				    sscanf(s + 2, "%lf %lf %d %d %d", &x, &y,
				    &owner, &ships, &growth) != 5) {
					printf("Error: sscanf: %s: %s", fm, s);
					goto done;
				}
				if (planets++)
					printf(":");
				printf("%f,%f,%d,%d,%d", x, y, owner, ships,
				    growth);
			}
			fclose(fm);

			int turns = 0;
			while (fgets(s, sizeof(s), f)) {
				s[strlen(s) - 1] = 0;
				if (!s[0])
					break;
				printf("%c%s", turns++ ? ':' : '|', s);
			}
			printf("\n");
			break;
		}
	}
done:
	fclose(f);
        fflush(stdout);
	return (0);
}
