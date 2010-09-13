#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "cgi.h"

const char *fn = "results.pgn";
static gzFile gz = NULL;

static void
dprintf(const char *fmt, ...)
{
	static char s[65536];
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsnprintf(s, sizeof(s), fmt, ap);
	va_end(ap);
	if (gz != NULL)
		r = gzputs(gz, s);
	else
		fprintf(stdout, "%s", s);
}

int main()
{
	struct query *q = get_query();
	const char *game_id = get_query_param(q, "game_id");
	const char *e;
	FILE *f, *fm, *fh;
	char s[8192], p1[256], p2[256], q1[256], q2[256], id[256], r[256];
	int w, h;

	memset(id, 0, sizeof(p1));
	memset(q1, 0, sizeof(p1));
	memset(q2, 0, sizeof(p1));
	printf("Connection: close\n");
	printf("Content-Type: text/html\n");
	if ((e = getenv("HTTP_ACCEPT_ENCODING")) != NULL &&
	    strstr(e, "gzip") != NULL) {
		printf("Content-Encoding: gzip\n\n");
		fflush(stdout);
		gz = gzdopen(fileno(stdout), "wb9");
	} else
		printf("\n");

	if (game_id == NULL || !game_id[0]) {
		dprintf("Error: missing parameter game_id");
		fflush(stdout);
		return 0;
	}
	if (strchr(game_id, '|')) {
		if (sscanf(game_id, "%255[^|]|%255[^|]|%255[^|]",
		    id, q1, q2) != 3) {
			dprintf("Error: invalid game_id %s\n", game_id);
			fflush(stdout);
			return 0;
		}
	} else
		strlcpy(id, game_id, sizeof(id));

	if ((fh = fopen("canvas.html", "r")) == NULL) {
		dprintf("Error: fopen: canvas.html: %s\n", strerror(errno));
		fflush(stdout);
		return 0;
	}
	while (fgets(s, sizeof(s), fh) != NULL) {
		if (!strncmp(s, "%%SCRIPT%%", 8))
			break;
		dprintf("%s", s);
	}

	if ((f = fopen(fn, "r")) == NULL) {
		dprintf("Error: fopen: %s: %s", fn, strerror(errno));
		return 0;
	}
	int turns = 0;
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

			if ((fm = fopen(s, "r")) == NULL) {
				dprintf("Error: fopen: %s: %s",
				    fm, strerror(errno));
				goto done;
			}
			dprintf("var data = \"");
			dprintf("map_id=%s\\n", s);
			dprintf("player_one=%s\\nplayer_two=%s\\n", p1, p2);
			dprintf("playback_string=");
			int planets = 0;
			while (fgets(s, sizeof(s), fm)) {
				double x, y;
				int owner, ships, growth;
				if (s[0] != 'P' || s[1]!= ' ' ||
				    sscanf(s + 2, "%lf %lf %d %d %d", &x, &y,
				    &owner, &ships, &growth) != 5) {
					dprintf("Error: sscanf: %s: %s", fm, s);
					goto done;
				}
				if (planets++)
					dprintf(":");
				dprintf("%f,%f,%d,%d,%d", x, y, owner, ships,
				    growth);
			}
			fclose(fm);

			while (fgets(s, sizeof(s), f)) {
				s[strlen(s) - 1] = 0;
				if (!s[0])
					break;
				dprintf("%c%s", turns++ ? ':' : '|', s);
			}
			dprintf("\"\n");
			break;
		}
	}
	fclose(f);

	while (fgets(s, sizeof(s), fh) != NULL)
		dprintf("%s", s);
	fclose(fh);
done:
	if (gz != NULL)
		gzclose(gz);
	else
		fflush(stdout);
	return (0);
}
