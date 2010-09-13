#include <errno.h>
#include <stdio.h>
#include <string.h>

const char *infn = "results.pgn";
const char *outfn = "results-new.pgn";
const char *bckfn = "results-old.pgn";

int main(int argc, char *argv[])
{
	FILE *in, *out;
	char s[8192];
	int state = 0;
	unsigned t, t0;
	unsigned countin = 0, countout = 0;

	t0 = time(NULL) - 6*60*60;
	printf("copying >= %u\n", t0);
	if ((out = fopen(outfn, "w")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", outfn, strerror(errno));
		return (1);
	}
	if ((in = fopen(infn, "r")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", infn, strerror(errno));
		fclose(out);
		return (1);
	}
	while (fgets(s, sizeof(s), in) != NULL) {
		countin++;
		if (state == 0 && !strncmp(s, "[Result ", 8)) {
			state = 1;
			continue;
		}
		if (state == 1) {
			t = atol(s);
			if (t >= t0)
				state = 2;
			else
				state = 0;
			continue;
		}
		if (state == 2 && !strcmp(s, "\n")) {
			state = 3;
			continue;
		}
		if (state == 3) {
			fprintf(out, "%s", s);
			countout++;
			continue;
		}
	}
	fclose(in);
	fclose(out);
	if (rename(infn, bckfn)) {
		fprintf(stderr, "rename: %s: %s: %s\n", infn, bckfn, strerror(errno));
		return (1);
	}
	if (rename(outfn, infn)) {
		fprintf(stderr, "rename: %s: %s: %s\n", outfn, infn, strerror(errno));
		return (1);
	}
	printf("%u of %u lines copied\n", countout, countin);
	return (0);
}
