#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <zlib.h>

extern "C" {
#include "cgi.h"
}

using namespace std;

const char *fn = "ratings.txt";
static gzFile gz = NULL;

struct Player {
	char		 name[256];
	unsigned	 rank, games, score, draws;
	int		 elo, plus, minus, oppo;
};

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
	const char *e;
	vector<Player> players;
	FILE *f;
	bool skip = true;

	unsigned rank;
	char s[8192];

	printf("Connection: close\n");
	printf("Content-Type: text/html\n");
	if ((e = getenv("HTTP_ACCEPT_ENCODING")) != NULL &&
	    strstr(e, "gzip") != NULL) {
		printf("Content-Encoding: gzip\n\n");
		fflush(stdout);
		gz = gzdopen(fileno(stdout), "wb9");
	} else
		printf("\n");

	dprintf("<html><body>\n");
	dprintf("<h2>Current ratings</h2>\n");

	f = fopen(fn, "r");
	while (fgets(s, sizeof(s), f)) {
		if (skip) {
			if (!strncmp(s, "Rank Name ", 10))
				skip = false;
			continue;
		}
		Player p;
		if (sscanf(s, "%u %255s %d %d %d %u %u%% %d %u%%",
		    &p.rank, p.name, &p.elo, &p.plus, &p.minus,
		    &p.games, &p.score, &p.oppo, &p.draws) != 9)
			continue;
		players.insert(players.end(), p);
	}
	fclose(f);

	dprintf("<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n");
	dprintf("<tr><th>Rank</th><th align=\"left\">Name</th><th>Elo</th><th>+</th><th>-</th><th>games</th><th>score</th><th>oppo.</th><th>draws</th></tr>\n");
	for (unsigned i = 0; i < players.size(); ++i) {
		Player p = players[i];
		const char *bg = i % 2 ? "#FFFFFF" : "#DDDDFF";
		dprintf("<tr bgcolor=\"%s\"><td align=\"right\">%u</td><td><a href=\"getplayer?player=%s\">%s</a></td><td align=\"right\"><b>%d</b></td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%u</td><td align=\"right\">%u%%</td><td align=\"right\">%d</td><td align=\"right\">%u%%</td></tr>\n",
		    bg, p.rank, p.name, p.name, p.elo, p.plus, p.minus,
		    p.games, p.score, p.oppo, p.draws);
	}
	dprintf("</table>\n");

	dprintf("</body><html>\n");

	if (gz != NULL)
		gzclose(gz);
	else
		fflush(stdout);
	return (0);
}
