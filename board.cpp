/*
 * Copyright (c) 2010 Daniel Hartmeier <daniel@benzedrine.cx>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include <algorithm>

#include "board.h"

#ifdef DEBUG
#define debug(format, ...)	do { fprintf(stderr, format, ## __VA_ARGS__); \
				} while (0)
#else
#define debug(format, ...)	
#endif

Board::Board(const char *fn) : fn(fn), valid(false)
{
	FILE *f;
	char s[8192];
	int id = 0;

	if (!(f = fopen(fn, "r"))) {
		debug("Board::Board: fopen: '%s': %s\n", fn, strerror(errno));
		goto done;
	}
	while (fgets(s, sizeof(s), f)) {
		if (s[0] == 'P' && s[1] == ' ') {
			double x, y;
			int owner, ships, growth;

			if (sscanf(s + 2, "%lf %lf %d %d %d",
			    &x, &y, &owner, &ships, &growth) != 5) {
				debug("Board::Board: %s: sscanf: %s", fn, s);
				goto done;
			}
			planets.insert(planets.end(),
			    Planet(id++, owner, ships, growth, x, y));
		} else {
			debug("Board::Board: %s: invalid line: %s", fn, s);
			goto done;
		}
	}
	if (planets.size() >= 2)
		valid = true;
	turns = 0;
done:
	if (f)
		fclose(f);
}

int
Board::distance(int src, int dst) const
{
	if (src < 0 || src >= planets.size() ||
	    dst < 0 || dst >= planets.size())
		return 0;
	const Planet &psrc = planets[src], &pdst = planets[dst];
	double dx = psrc.getX() - pdst.getX();
	double dy = psrc.getY() - pdst.getY();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

bool
Board::issueOrder(int player, int src, int dst, int ships)
{
	if (player < 1) {
		debug("Board::issueOrder: invalid player %d\n", player);
		return true;
	}
	if (src < 0 || src >= planets.size()) {
		debug("Board::issueOrder: invalid src %d\n", src);
		return true;
	}
	if (dst < 0 || dst >= planets.size() || src == dst) {
		debug("Board::issueOrder: invalid dst %d\n", dst);
		return true;
	}
	Planet &p = planets[src];
	if (player != p.getOwner()) {
		debug("Board::issueOrder: player %d not owner of planet %d "
		    "(owner is %d)\n", player, src, p.getOwner());
		return true;
	}
	if (ships < 1 || ships > p.getShips()) {
		debug("Board::issueOrder: invalid number of ships %d\n", ships);
		return true;
	}
	int len = distance(src, dst);
	if (len < 1) {
		debug("Board::issueOrder: len %d < 1\n", len);
		return true;
	}
	p.setShips(p.getShips() - ships);
	for (vector<Fleet>::iterator i = fleets.begin(); i != fleets.end();
	    ++i) {
		if (i->getOwner() == player &&
		    i->getSrc() == src && i->getDst() == dst &&
		    i->getLen() == len && i->getRemain() == len) {
			i->setShips(i->getShips() + ships);
			return false;
		}
	}
	fleets.insert(fleets.end(), Fleet(player, ships, src, dst, len, len));
	return false;
}

inline int max(int a, int b) { return a > b ? a : b; }

void
Board::nextTurn()
{
	// growth happens before battle
	for (vector<Planet>::iterator p = planets.begin(); p != planets.end();
	    ++p)
		if (p->getOwner())
			p->setShips(p->getShips() + p->getGrowth());

	// advance fleets, for fleets arriving at their destination:
	// merge fleets of each player
	map<int, pair<int, int> > m;
	vector<Fleet>::iterator f = fleets.begin();
	while (f != fleets.end()) {
		if (f->decrementRemain() > 0) {
			++f;
			continue;
		}
		if (f->getOwner() == 1)
			m[f->getDst()].first += f->getShips();
		else
			m[f->getDst()].second += f->getShips();
		f = fleets.erase(f);
	}
	// process arriving (merged) fleets
	for (map<int, pair<int, int> >::iterator i = m.begin(); i != m.end();
	    ++i) {
		Planet &p = planets[i->first];
/*
<jmcarthur> (1) Combine fleets. (2) Compare the top two (A and B). (3) If one is
+strictly greater than the other (say, A), winner is A with A-B ships. Otherwise, the
+previous owner of the planet retains ownership with 0 ships.
<ratatata> a1k0n: I don't think I've sent a wrong command, the code that gets evaluated
+at that point is pretty much the same 10 turns before.. I will review anyway
<jmcarthur> dhartmei: ^^
*/
		vector<int> n(3);
		n[0] = 0;
		n[1] = i->second.first;
		n[2] = i->second.second;
		n[p.getOwner()] += p.getShips();
		if (n[1] > n[2] && n[1] > n[0]) {
			p.setOwner(1);
			p.setShips(n[1] - max(n[2], n[0]));
		} else if (n[2] > n[1] && n[2] > n[0]) {
			p.setOwner(2);
			p.setShips(n[2] - max(n[1], n[0]));
		} else {
			sort(n.begin(), n.end());
			p.setShips(n[2] - n[1]);
		}
	}
	turns++;
}

/* WIN=p1 wins, LOSS=p2 wins */
RESULT
Board::rate() const
{
	unsigned p[2] = { 0, 0 }, s[2] = { 0, 0 };
	for (vector<Planet>::const_iterator i = planets.begin();
	    i != planets.end(); ++i) {
		if (i->getOwner()) {
			p[i->getOwner() - 1]++;
			s[i->getOwner() - 1] += i->getShips();
		}
	}
	for (vector<Fleet>::const_iterator i = fleets.begin();
	    i != fleets.end(); ++i) {
		s[i->getOwner() - 1] += i->getShips();
	}
	debug("%u turns: %d fleets, player 1 has %d planets and %d ships, player 2 has %d planets and %d ships\n", turns, (int)fleets.size(), p[0], s[0], p[1], s[1]);
	// all planets are owned by one player -> win
	if (p[0] == planets.size())
		return WIN;
	if (p[1] == planets.size())
		return LOSS;
	// a player doesn't have any ships anymore -> loss
	if (!s[0] && !s[1])
		return DRAW;
	if (!s[1])
		return WIN;
	if (!s[0])
		return LOSS;
	if (turns > 299) {
		if (s[0] > s[1])
			return WIN;
		if (s[1] > s[0])
			return LOSS;
		return DRAW;
	}
	return UNDECIDED;
}

string
Board::describe() const
{
	string s;
	char t[256];

	for (vector<Planet>::const_iterator p = planets.begin();
	    p != planets.end(); ++p) {
		if (p != planets.begin())
			s += ",";
		snprintf(t, sizeof(t), "%d.%d", p->getOwner(), p->getShips());
		s += t;
	}
	for (vector<Fleet>::const_iterator f = fleets.begin();
	    f != fleets.end(); ++f) {
		snprintf(t, sizeof(t), ",%d.%d.%d.%d.%d.%d",
		    f->getOwner(), f->getShips(), f->getSrc(), f->getDst(),
		    f->getLen(), f->getRemain());
		s += t;
	}
	return s;
}

void
Board::print(int player, char *s, unsigned len) const
{
	string t;

	s[0] = 0;
	for (vector<Planet>::const_iterator p = planets.begin();
	    p != planets.end(); ++p) {
		snprintf(s + strlen(s), len - strlen(s), "P %f %f %d %d %d\n",
		    p->getX(), p->getY(), p->getOwner() == player ? 1 :
		    (p->getOwner() ? 2 : 0),
		    p->getShips(), p->getGrowth());
	}
	for (vector<Fleet>::const_iterator f = fleets.begin();
	    f != fleets.end(); ++f)
		snprintf(s + strlen(s), len - strlen(s), "F %d %d %d %d %d %d\n",
		    f->getOwner() == player ? 1 : 2, f->getShips(),
		    f->getSrc(), f->getDst(), f->getLen(), f->getRemain());
	if (strlen(s) + 64 > len)
		printf("Board::print() warning: s len exceeded\n");
}
