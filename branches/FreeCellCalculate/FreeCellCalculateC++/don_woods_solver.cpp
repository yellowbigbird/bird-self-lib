//>From don@clari.net  Tue Aug 12 18:09:42 1997
//Return-Path: <don@clari.net>
//Received: from madrigal.clari.net (madrigal.clari.net [192.54.253.79])
//	  by mrin60.mail.aol.com (8.8.5/8.8.5/AOL-4.0.0)
//	  with ESMTP id SAA11154 for <Wgreview@aol.com>;
//	  Tue, 12 Aug 1997 18:09:17 -0400 (EDT)
//Received: (from don@localhost)
//	by madrigal.clari.net (8.8.5/8.8.5) id PAA18000
//	for Wgreview@aol.com; Tue, 12 Aug 1997 15:09:11 -0700 (PDT)
//Date: Tue, 12 Aug 1997 15:09:11 -0700 (PDT)
//From: Don Woods <don@clari.net>
//Message-Id: <199708122209.PAA18000@madrigal.clari.net>
//To: Wgreview@aol.com
//Subject: Re: Baker's Game
//Content-Type: X-sun-attachment
//
//----------
//X-Sun-Data-Type: text
//X-Sun-Data-Description: text
//X-Sun-Data-Name: text
//X-Sun-Charset: us-ascii
//X-Sun-Content-Lines: 43
//
//> Yes.    I still have copies of all of our e-letters.   Let me know if you
//> need me to resend copies.
//
//Thanks.  I think I remember the basic content of the letters.  In addition
//to asking about the winning chances, you were also interested in copies of
//the program(s), and talked about a mailing list about solitaires.
//
//Anyway, I completed my modifications to handle Baker's Game.  It runs
//almost as fast as the Seahaven program (about 15 positions per second
//on my Unix box), and gave a definite answer for every position I've
//thrown at it.  (The FreeCell program generally gets bogged down if it
//can't solve a position.  The Seahaven and Baker's programs always give
//a definite solvable-or-impossible answer.)
//
//While debugging the Baker's Game program I ran across one bug that I
//realised would have a small effect on the Seahaven program as well.  In
//the Seahaven program, it would cause the program to fail to solve
//positions in which the only winning line included a situation where
//(a) a king was moved from a free cell to an empty column, and (b) the
//matching queen was also in a free cell, and (c) another formerly-empty
//column was already headed by a king of a lower suit (in bridge ranking).
//In such situations it was failing to move the queen (and jack, if
//present, etc.) onto the newly moved king.
//
//This bug had a very small effect on the Seahaven statistics; I ran a
//fresh set of ten million games and found it won 89.337% instead of the
//previously reported 89.307%.  The corresponding bug in Baker's Game
//would have had a larger effect, since there are more cases where a
//sequence of cards is moved from free cells into an empty column.
//
//Anyway, I tested one million Baker's Game positions, and found 75.08% were
//winnable.  0.93%, or about 1.2% of winnable games, could be won only if at
//some point a partial sequence was moved into an empty column.  (Without
//immediately moving the rest of the sequence, that is.  E.g., you might
//have four cards in sequence in one column, and have to move two of them
//into an empty column, then do some other stuff before moving the rest of
//the sequence.)  The vast majority of winnable games did not require such a
//move.  I played out a couple of games that required partial-sequence moves
//and recorded the situations involved; let me know if you want to see them.
//
//I'll attach the sources for the various programs.
//
//	-- Don.
//----------
//X-Sun-Data-Type: c-file
//X-Sun-Data-Description: c-file
//X-Sun-Data-Name: baker.c
//X-Sun-Charset: us-ascii
//X-Sun-Content-Lines: 1227

/*
 * Program for testing winnability of opening positions in Baker's Game.
 * Can also generate random positions, optionally filtered for winnability.
 * Does not actually support interactive playing of the game.  See below for
 * a brief explanation of the rules.
 *
 * (c) Copyright 1993, 1997, Donald R. Woods.  Shareware: Right is granted to
 *	freely redistribute this source provided this notice is retained.
 */

/*****************************************************************************
Brief summary of the game:

Deal four columns of six cards each, and four of seven cards each, spreading
each column so the cards are all visible.  These columns form the "tableau".
There is a "holding area" that can hold a maximum of four cards.  The only
cards that can be moved are the bottommost card in each column and the cards
in the holding area.  As Aces become available (i.e., can be moved) they are
moved to start four "foundation" piles, which are then built up in suit to
Kings.  The object is to move all the cards to the foundations.

A move consists of moving one card.  A card can move onto a tableau column
if the card being moved is the same suit and one rank lower than the card it
is being played onto (i.e., the tableau builds down in suit).  Any card can
be moved to an empty holding area spot, or to an empty column in the tableau.
(Note that this differs from Seahaven Towers.)


General approach: Depth-first search seems to be fast enough, so we won't try
anything fancier.  We first try solving without considering certain types of
move that are unlikely to help; if that fails, we retry with all moves being
considered.

The search space is reduced by canonicalising each position so as to reduce
the number of different moves that need be considered.  Canonical form is
obtained by (a) making all possible moves to the foundations and (b) making
all possible moves from the holding area to non-empty columns in the tableau.
(We also sort the cards in the holding area, and require that any tableau
columns that have ever been empty are sorted based on the cards at the top of
each column, with the empty columns last.  We do NOT sort the rest of the
tableau.)

Given this canonical form, the only moves that need be considered are:
 - maximal sequence from tableau to tableau, assuming sufficient openings
   exist in the holding area and/or empty columns as intermediate spots
 - maximal sequence from tableau to empty column, again assuming sufficient
   openings; if sequence is an entire column, or could move to a non-empty
   column, do not bother trying this move
 - if no space exists, move a maximal sequence into free cells; don't bother
   trying this if they could also be moved onto a non-empty tableau column
   (conversion to canonical form would immediately duplicate another move)
 - if space exists, move a sequence that is an entire column into free cells
   provided this increases the max length of sequence that can be moved
 - move one card from holding area to empty tableau column, unless another
   free cell contains a card that this one could move onto (move it down
   instead)
 ? move part of a sequence (at least TWO cards) into a space (try longer
   partial sequences first)
This last type of move is considered only if the position is found to be
unsolveable without it.

Certain moves imply that later moves are not allowed:  Moving part of a
sequence off of column X means that it cannot be moved anywhere else unless
more cards are first moved away from column X (else why bother to have moved
it the first time?).  Moving a card from a free cell to an empty column means
it cannot be moved anywhere (even to a foundation!) unless another card has
at some time been placed on it (else why not leave it in the free cell and
leave the column empty until the later move?).

A position is encoded in terms of the starting position.  For each non-empty
column, the column is headed either any single card, or by 1-5 of the cards
originally in that column.  The single card, or bottom of the 1-5 cards, may
then have 0-11 additional cards on it.  (We'll never build an ace into the
tableau.)  Thus for each column we give:
	1 bit:  0 if empty or headed by an original card, else 1
	7 bits: 0 if empty, 2-7 if headed by 2-7 orig cards, else top card
	4 bits: number of additional cards in sequence
	4 bit:  0 if no move restrictions, 1 if mustn't move until more cards
		are placed here, 8-15 if mustn't move to foundation or col 0-7
		until col 0-7 has moved, 7 if a restricted col came from here
(Move restrictions of value 7-15 arise only if allowing the optional partial
sequence moves.)

If the original cards include a sequence at the bottom of a column, or if only
a single original card remains at the top, the column representation is
canonicalised in favor of claiming fewer original cards still in place.  (This
ensures that positions match even if we remove some cards that later return.)

Knowing which cards are in the tableau -- along with any "prohibited moves" --
uniquely identifies the position, so the foundation and holding area are not
considered when testing two positions for equality, but they are encoded as
part of the position for ease of access.

In addition to storing the original position (necessary in order to interpret
the above encoding), we also create a structure indexed by card that says
where each card started.  This makes it easy to see whether the card is
available for play.  (If the card's starting column does not still include
as many original cards as the given card's starting position, the card has
been reached.  If so, it is guaranteed to be available to move to a foundation,
and is also available to be built upon unless it is in the holding area.)

Input format is 52 cards represented either as rank then suit or suit then
rank, where suit is one of CDHS and rank is one of A23456789TJQK.  Lower-case
is okay too.  Other characters such as spaces and line breaks are ignored and
can be used for readability.  E.g.:

7h 9s Kc 5d 8h 3h 6d 9d
7d As 3c Js 8c Kd 2d Ac
2s Qc Jh 8d Th Ts 9h 5h
Qs 6c 4s 6h Ad 8s 2h 4d
Ah 7s 3d Jd 9c 3s Qd Kh
7c 4c Jc Qh 2c Tc 5s 4h
5c Td Ks 6s

*****************************************************************************/

#include <stdio.h>
#include <string.h>

#define true  (1==1)
#define false (1==0)

typedef unsigned char	uchar;
typedef uchar		Card;	/* 00ssrrrr: ss=suit(0-3), rr=rank(1-13) */
typedef unsigned short	Column; /* see encoding above */

#define Rank(card)	((card) & 0x0F)
#define Suit(card)	((card) >> 4)

#define NumOrig(col)	((col) >> 8)
#define SeqLen(col)	(((col) >> 4) & 0xF)
#define BeenEmpty(col)	(((col) & 0x8000) != 0)
#define NewTop(col)	(((col) >> 8) - 0x80)
#define Restrict(col)	((col) & 0x000F)
#define CantMove(col)	((Restrict(col) != 0) && (Restrict(col) != 7))

/* move restrictions */
#define MUST_MOVE_ONTO	1	/* must add more here before moving away */
#define MUST_MOVE_FROM	8	/* must move from X before moving from here */
#define MOVE_AWAITED	7	/* move from here enables move(s) elsewhere */

typedef struct pos_struct {
	uchar	foundations[4];
	Card	hold[4];
	Column	tableau[8];
	short	spaces;		  /* number of spaces */
	short	numHold;	  /* number of open free cells */
	struct pos_struct *via;	  /* position from which this was reached */
	struct pos_struct *chain; /* for chaining in hash table */
	unsigned long hash;
} Position;

#define MaxMove(pos)	(((pos)->numHold+1) << (pos)->spaces)

typedef struct {
	Card	tableau[8][7];
	uchar	started[0x40];	/* where each card started */
} Original;

#define CLUBS	 (0 << 4)
#define DIAMONDS (1 << 4)
#define HEARTS   (2 << 4)
#define SPADES	 (3 << 4)

static int verbosity = 0;	/* how much to print out while running */
static int show = false;	/* whether to show solution if found */
static int showorig = false;	/* whether to show original layout */
static int allowPartial = false;/* whether to allow splitting sequences */
static int couldPartial;	/* whether bypassed splitting a sequence */

/* statistics gathering */
static long generated = 0;	/* # times AddToTree called */
static long distinct = 0;	/* # different positions reached */
static long hashes = 0;		/* # hash table slots used */
static long partial = 0;	/* # sequence-splitting moves tried */

/* Random number package.  Roll our own since so many systems seem to have
 * pretty bad ones.  Include code for both MSDOS and UNIX time functions to
 * initialise it.
 */
#ifdef __MSDOS__
#include "time.h"
#define GET_TIME(var) {time(&var);}
#else
#include "sys/time.h"
#define GET_TIME(var) { \
	struct timeval now; \
	gettimeofday(&now, 0); \
	var = now.tv_sec + now.tv_usec; \
}
#endif

#define RAND_BUFSIZE	98
#define RAND_HASHWORD	27
#define RAND_BITSUSED	29
#define RAND_MASK	((1 << RAND_BITSUSED) - 1)

static long rand_buf[RAND_BUFSIZE], *rand_bufLoc;
static long Rand();
static char rand_flips[1<<6];
static short rand_flipper;

#define RAND_FLIPMASK (sizeof(rand_flips) - 1)

static void
RandFlipInit(loc) int loc;
{
	static char f[] =
	   " 11 111 1  1    11 1 111 1  11      11 11111   11 1 1 11  1 11  ";

	rand_flipper = loc;
	for (loc=0; loc<=RAND_FLIPMASK; loc++)
		rand_flips[loc] = (f[loc]==' '? 0 : -1);
}

static void
RandInit(seed) long seed;
{
	int n;

	if (seed == 0) GET_TIME(seed);
	memset(rand_buf, 0, sizeof(rand_buf));
	for (n=0; n<RAND_BITSUSED; n++)
	{	rand_buf[n] = RAND_MASK >> n;
		rand_buf[n+RAND_BITSUSED+3] = 1 << n;
	}
	for (n=RAND_BITSUSED*2+3; n<RAND_BUFSIZE; n++)
	{	rand_buf[n] = seed & RAND_MASK;
		seed = seed * 3 + 01234567;
	}
	rand_bufLoc = rand_buf;
	RandFlipInit(0);
	for (n=(seed & 0377); n<50000; n++) Rand();
}

static long
Rand()
{
	long f;

	if (!rand_bufLoc) RandInit(0);
	rand_bufLoc = (rand_bufLoc == rand_buf ?
		rand_bufLoc+RAND_BUFSIZE-1 : rand_bufLoc-1);
	*rand_bufLoc ^= *(rand_bufLoc >= rand_buf+RAND_HASHWORD ?
		rand_bufLoc-RAND_HASHWORD
		: rand_bufLoc+(RAND_BUFSIZE-RAND_HASHWORD));
	f = rand_flips[rand_flipper++ & RAND_FLIPMASK];
	return(*rand_bufLoc ^ (f & RAND_MASK));
}

static void
ShuffleChar(arr, size) char *arr; int size;
{
	int swap;
	char temp;

	while (size > 1) {
		swap = Rand() % (size--);
		temp = arr[swap];
		arr[swap] = arr[size];
		arr[size] = temp;
	}
}

/* End of random number package
 *******************************/

static Original orig;

/* Read original position from stdin into orig.  First cards read are
 * the top cards of each column, etc.
 */
static void
ReadOriginal(filename) char *filename;
{
	int count = 0;
	Card cd = 0;
	FILE *f = stdin;

	if (filename != NULL && (f = fopen(filename, "r")) == NULL) {
		printf("baker: can't open %s\n", filename);
		exit(1);
	}
	memset(&orig, -1, sizeof(orig));
	while (count < 104) {
		int c = getc(f);
		if (c == EOF) {
			printf("Insufficient input.\n");
			exit(1);
		}
		if (c >= '2' && c <= '9') cd += c - '0';
		else if ((c |= 0x20) == 'a') cd += 1;
		else if (c == 't') cd += 10;
		else if (c == 'j') cd += 11;
		else if (c == 'q') cd += 12;
		else if (c == 'k') cd += 13;
		else if (c == 'c') cd += CLUBS;
		else if (c == 'd') cd += DIAMONDS;
		else if (c == 'h') cd += HEARTS;
		else if (c == 's') cd += SPADES;
		else continue;
		if (count++ % 2 != 0) {
			int pos = (count>>1) - 1;
			orig.tableau[pos%8][pos/8] = cd;
			if (orig.started[cd] != 0xFF) {
				printf("Card %02x appears twice!\n", cd);
				exit(1);
			}
			orig.started[cd] = pos;
			cd = 0;
		}
	}
	if (filename != NULL) fclose(f);
}

/* Build original as simple sorted deck.  Don't fill in tableau/hold yet.
 */
static void
BuildOriginal()
{
	int i;

	memset(&orig, -1, sizeof(orig));
	for (i=0; i<52; i++) {
		Card cd = (i>>2)+1 + ((i&3)<<4);
		orig.started[cd] = i;
	}
}

/* Shuffle the starting positions in orig, then fill in tableau/hold.
 */
static void
Shuffle()
{
	int i;
	static uchar arrange[52];
	
	if (arrange[0] == arrange[1]) for (i=0; i<52; i++) arrange[i] = i;
	ShuffleChar(arrange, 52);
	for (i=0; i<0x40; i++)
		if (orig.started[i] != 0xFF)
			orig.started[i] = arrange[orig.started[i]];
	for (i=0; i<52; i++) {
		Card cd = (i>>2)+1 + ((i&3)<<4);
		int where = orig.started[cd];
		orig.tableau[where%8][where/8] = cd;
	}
}

/* Allocate a new Position structure.  Allocates many at a time to reduce
 * malloc overhead.  Saves linked list of blocks of positions to enable
 * freeing the storage, which can be necessary if we're examining hundreds
 * of positions in a single run of the program.
 */
#define POS_BLOCK 1000
typedef struct pos_block {
	Position block[POS_BLOCK];
	struct pos_block *link;
} Block;

static int pos_avail = 0;
static Block *block = NULL;

static Position *
NewPosition()
{
	if (pos_avail-- == 0) {
		Block *more = (Block *) malloc(sizeof(Block));
		if (more == NULL) {
			printf("\nERROR: Out of memory!\n");
			exit(2);
		}
		more->link = block;
		block = more;
		pos_avail = POS_BLOCK - 1;
	}
	return (block->block + pos_avail);
}

/* Find the bottom (i.e., immediately playable) card in a column.
 * Return 0 for empty columns.
 */
static Card
Bottom(pos, col) Position *pos; int col;
{
	Column c = pos->tableau[col];
	Card seqtop;

	if (c == 0) return(0);
	if (BeenEmpty(c)) seqtop = NewTop(c);
	else seqtop = orig.tableau[col][NumOrig(c)-1];
	return(seqtop - SeqLen(c));
}

/* Show a position in internal format.  Can be called from debugger.
 */
ShowPosition(pos) Position *pos;
{
	int i, tot = 0, check;

	printf("F:");
	for (i=0; i<4; i++) {
		printf(" %d", pos->foundations[i]);
		tot += pos->foundations[i];
	}
	printf("; H:");
	check = 0;
	for (i=0; i<4; i++) {
		printf(" %02x", pos->hold[i]);
		if (pos->hold[i]) tot++; else check++;
	}
	if (check != pos->numHold) printf("(%d?)", pos->numHold);
	check = 0;
	printf("; T:");
	for (i=0; i<8; i++) {
		Column col = pos->tableau[i];
		printf(" %04x", col);
		if (col) {
			if (! BeenEmpty(col)) tot += NumOrig(col) - 1;
			tot += SeqLen(col) + 1;
		}
		else check++;
	}
	if (check != pos->spaces) printf("(%d?)", pos->spaces);
	printf("\n");
}

/* Show a card; rank in uppercase, suit in lowercase, trailing space.
 */
void
ShowCard(card) Card card;
{
	static char *rank = "?A23456789TJQK", *suit = "cdhs";
	
	if (card == 0) printf(".. ");
	else printf("%c%c ", rank[Rank(card)], suit[Suit(card)]);
}

/* Show a position in more readable format.  Gives bottom sequence for each
 * column (e.g., "Q-8s"), then holding area and foundation tops.  Format:
X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys | Hs Hs Hs Hs | Fc Fd Fh Fs
 */
void
ReadablePosition(pos) Position *pos;
{
	int i;
	static char *rank = "?A23456789TJQK", *suit = "cdhs";

	for (i=0; i<8; i++) {
		Column col = pos->tableau[i];
		Card bot = Bottom(pos, i);
		Card top = bot + SeqLen(col);
		if (col == 0) printf("  .. ");
		else {
			if (top == bot) printf("  ");
			else printf("%c-", rank[Rank(top)]);
			ShowCard(bot);
		}
	}
	printf("| ");
	for (i=0; i<4; i++) ShowCard(pos->hold[i]);
	printf("|");
	for (i=0; i<4; i++) {
		uchar moved = pos->foundations[i];
		if (moved == 0) printf(" ..");
		else printf(" %c%c", rank[moved], suit[i]);
	}
	printf("\n");
}
	
/* Show the winning path, in reverse order (because it's convenient).
 */
void
ShowPath(pos) Position *pos;
{
	printf("Winning path (in reverse order):\n");
	while (pos != NULL) {
		ReadablePosition(pos);
		pos = pos->via;
	}
	printf("\n");
}
	
/* Show the original position as a rectangular layout.
 */
void
ShowOriginal()
{
	int row, col;

	printf("\n");
	for (row=0; row<7; row++) {
		for (col=0; col<(row==6? 4 : 8); col++)
			ShowCard(orig.tableau[col][row]);
		printf("\n");
	}
	printf("\n");
}

#define HASH_SIZE 19001
static Position **tree;

/* See if this position is already in the search tree.  If so, return false.
 * Else add it and return true.  Position is assumed to be in canonical form,
 * so the tableau uniquely determines the hold area and foundations.
 */
static int
AddToTree(pos) Position *pos;
{
	unsigned long hash = 0;
	int i;
	Position **probe;

	generated++;
	for (i=0; i<8; i++) hash = hash*5 + pos->tableau[i];
	pos->hash = hash;
	probe = &tree[hash % HASH_SIZE];
	if (*probe == NULL) hashes++;
	else while (*probe != NULL) {
		if ((*probe)->hash == hash && memcmp((*probe)->tableau,
				pos->tableau, 8*sizeof(Column)) == 0)
			return(false);
		probe = &((*probe)->chain);
	}
	*probe = pos;
	pos->chain = NULL;
	if ((distinct++ & 0x7FFF) == 0x7FFF && verbosity == 1) {
		printf("%d distinct positions out of %d...\n",
		       distinct, generated);
		fflush(stdout);
	}
	return(true);
}

/* If the specified card is still in its original column and not yet uncovered,
 * return -1.  Else return the number of the column where the card lies, or the
 * number of its holding area spot.  Only the first test needs to be quick,
 * since if the card HAS been uncovered we're about to make some progress.  We
 * can also assume we're never asked about a card in the foundation piles;
 * indeed, we're only asked about cards that, if they've been uncovered, are
 * currently playable.
 */
static int
Available(pos, card) Position *pos; Card card;
{
	uchar whence;
	int i;

	if (Rank(card) > 13) return(-1);
	whence = orig.started[card];
	if (!BeenEmpty(pos->tableau[whence%8])
		&& NumOrig(pos->tableau[whence%8]) > (whence/8)+1)
		return(-1);
	for (i=0; i<4; i++) if (pos->hold[i] == card) return(8+i);
	for (i=0; i<8; i++) if (Bottom(pos, i) == card) return(i);
	printf("\nERROR: card %02x not covered but not found\n", card);
	ShowPosition(pos);
	ShowOriginal();
	exit(2);
}

/* Sort the holding area into descending order (hence spaces come last).
 * Use homogeneous sorting network.
 */
static void
SortHoldingArea(pos) Position *pos;
{
	Card *h = pos->hold;
	Card temp;

#define SWAP(i,j) if (h[i]<h[j]) {temp=h[i]; h[i]=h[j]; h[j]=temp;}

	SWAP(0,1)
	SWAP(2,3)
	SWAP(0,2)
	SWAP(1,3)
	SWAP(1,2)
}

/* Move a formerly-empty column within the tableau, adjusting move
 * restrictions on other columns if necessary.
 */
static void
MoveColumn(pos, col, to, from) Position *pos; Column col; int to, from;
{
	int i;

	pos->tableau[to] = col;
	if (Restrict(col) == MOVE_AWAITED) {
		for (i=0; i<8; i++) {
			if (Restrict(pos->tableau[i]) == MUST_MOVE_FROM + from)
				pos->tableau[i] += to - from;
		}
	}
}

/* Make sure all formerly-empty columns come before all currently empty, and
 * that the formerly-empty ones are sorted by top card.
 *
 * Note: This routine is pretty inefficient, since it not only uses a bubble
 * sort but also looks at the never-empty columns every pass.  Check whether
 * its gets called much and if so think about speeding it up.  (It may well
 * get used more in Baker's Game than it was in Seahaven.)
 */
static void
SortEmpties(pos) Position *pos;
{
	int changed, i, previ;
	Column prevcol;

	do {
		changed = false;
		prevcol = 0xFFFF;
		for (i=0; i<8; i++) {
			Column col = pos->tableau[i];
			if (col==0 || BeenEmpty(col)) {
				if (col > prevcol) {
					MoveColumn(pos, prevcol, i, previ);
					MoveColumn(pos, col, previ, i);
					changed = true;
				}
				else
					prevcol = col;
				previ = i;
			}
		}
	} while (changed);
}

/* Given a column for which MOVE_AWAITED, update other cols to note move
 * from here.
 */
static void
MovedFrom(pos, i) Position *pos; int i;
{
	int k;

	for (k=0; k<8; k++)
		if (k == i || Restrict(pos->tableau[k]) == MUST_MOVE_FROM+i)
			pos->tableau[k] &= 0xFFF0;
}

/* Given a column (which is assumed to be non-empty), remove the currently
 * playable sequence.  Then make sure that (a) the new sequence at the end is
 * maximal, i.e. combine it if possible with the card immediately above, and
 * (b) if the column contains only a single sequence, it's converted to a
 * filled space even though the sequence started there.  Returns the index
 * where the column ended up (same as input unless SortEmpties was called).
 */
static int
Uncover(pos, i) Position *pos; int i;
{
	Column col = pos->tableau[i];

	if (Restrict(col)) {
		int k;
		if (Restrict(col) != MOVE_AWAITED) {
			printf("\nERROR: Violated move prohibition.\n");
			ShowOriginal();
			exit(2);
		}
		MovedFrom(pos, i);
	}
      	if (BeenEmpty(col)) {
		col = 0;
		pos->spaces++;
	}
	else
		col = (col & 0xFF00) - 0x0100;
	while (col != 0) {
		int num = NumOrig(col);
		Card seqtop = orig.tableau[i][num-1];
		if (num == 1) {
			col = 0x8000 + (seqtop << 8) + (col & 0x00FF);
			break;
		}
		if (seqtop != orig.tableau[i][num-2] - 1) break;
		col -= (1<<8) - (1<<4);	/* 1 less orig, 1 more in seq */
	}
	pos->tableau[i] = col;
	if (col == 0 || BeenEmpty(col)) SortEmpties(pos);
	if (pos->tableau[i] == col) return(i);
	col &= 0xFFF0;
	for (i=0; i<8; i++) if ((pos->tableau[i] & 0xFFF0) == col) return(i);
	printf("\nERROR: column got lost while sorting empties\n");
	ShowOriginal();
	exit(2);
}

/* Return true if given card can be played to a foundation in given position.
 */
static int
PlaysToFoundation(pos, card) Position *pos; Card card;
{
	if (Rank(card) == pos->foundations[Suit(card)] + 1) return(true);
	return(false);  /* note: works (returns false) if card==0 */
}

/* If specified column plays to a foundation, do so and return true.
 */
static int
TableauToFoundation(pos, i) Position *pos; int i;
{
	Column col = pos->tableau[i];
	Card bot = Bottom(pos, i);

	if (! PlaysToFoundation(pos, bot)) return(false);
	if (CantMove(col)) pos->tableau[i] &= 0xFFF0;
	/* go ahead and do it even though disallowed; otherwise we might
	   overlook a short winning path and spend forever trying partial
	   moves before backing up */
	pos->foundations[Suit(bot)] += 1 + SeqLen(col);
	(void) Uncover(pos, i);
	return(true);
}

/* If specified holding area card plays to foundation, do so and return true.
 * Does NOT sort the holding area.
 */
static int
HoldToFoundation(pos, i) Position *pos; int i;
{
	Card held = pos->hold[i];

	if (! PlaysToFoundation(pos, held)) return(false);
	pos->hold[i] = 0;
	pos->numHold++;
	pos->foundations[Suit(held)] += 1;
	return(true);
}

/* Add a sequence of cards (given as top card and number of additional cards)
 * to the holding area.  Assumes there is room.  Sorts the holding area.
 */
static void
AddToHold(pos, seqtop, slen) Position *pos; Card seqtop; int slen;
{
	pos->numHold -= slen+1;
	while (slen-- >= 0) pos->hold[2-slen] = seqtop--;
	SortHoldingArea(pos);
}

/* Perform various automatic operations to put a position into canonical form,
 * such that obviously equivalent positions are combined.  These include:
 *	- moving cards to the foundations
 *	- moving cards from holding area onto non-empty columns
 *	- moving an entire column onto another column unless restricted
 *	- sorting the cards (and empty slots) in the holding area
 *	- sorting empty and formerly empty columns (done in Uncover)
 * The latter two are assumed not to be necessary unless the other operations
 * may have led to things being unsorted.
 */
static void
Canonical(pos) Position *pos;
{
	int sortHold = false;
	int changed, i, canMove;

	do {	/* look for moves to foundation */
		changed = false;
		for (i=0; i<8; i++)
			if (TableauToFoundation(pos, i)) changed = true;
		for (i=3; i>=0; i--)	/* start at 3 since might not be sorted
					   after first iteration */
			if (HoldToFoundation(pos, i)) changed = sortHold = true;
			/* reverse order moves seqs in one pass */
	} while (changed);

	/* look for moves from holding area to tableau */
	/* single forward pass moves all possible */
	for (i=0; i<4; i++) {
		Card held = pos->hold[i];
		int whither;
		if (held == 0) continue;
		whither = Available(pos, held+1);
		if (whither >= 0 && whither <= 7) {
			Column col = pos->tableau[whither];
			col += (1<<4);
			if (Restrict(col) == MUST_MOVE_ONTO) col &= 0xFFF0;
			pos->tableau[whither] = col;
			pos->hold[i] = 0;
			pos->numHold++;
			sortHold = true;
		}
	}

	/* look for whole columns that can combine with other columns */
	canMove = MaxMove(pos);
	for (i=0; i<8; i++) {
		Column col = pos->tableau[i];
		Card top;
		int whither;
		if (! BeenEmpty(col) || SeqLen(col)+1 > canMove) continue;
		top = NewTop(col);
		whither = Available(pos, top+1);
		if (whither >= 0 && whither <= 7 &&
		    Restrict(col) != MUST_MOVE_FROM + whither &&
		    Restrict(col) != MUST_MOVE_ONTO) {
			Column onto = pos->tableau[whither];
			onto += (SeqLen(col)+1) << 4;
			if (Restrict(onto) == MUST_MOVE_ONTO) onto &= 0xFFF0;
			pos->tableau[whither] = onto;
			(void) Uncover(pos, i);
			i = -1;	/* start over since empties got sorted */
			canMove = MaxMove(pos);	/* can move more now, too */
		}
	}

	if (sortHold) SortHoldingArea(pos);
}

/* Check whether a newly available card in a particular column requires
 * that we call Canonical.  If the card moves to a foundation, or if
 * another entire column should move onto it, call the full Canonical
 * routine.  If cards just move down from the hold area, no other action
 * is needed.
 */
void
MiniCanon(pos, i) Position *pos; int i;
{
	Card want;
	Column col = pos->tableau[i];
	int j, moved = 0;

	if (col == 0 || TableauToFoundation(pos, i)) {Canonical(pos); return;}

	want = Bottom(pos, i) - 1;

	/* look for moves from holding area to tableau */
	/* single forward pass moves all possible */
	for (j=0; j<4 && pos->hold[j]!=0; j++) {
		if (pos->hold[j] == want) {
			pos->hold[j] = 0;
			moved++;
			want--;
		}
	}
	if (moved != 0) {
		col += (moved << 4);
		if (Restrict(col) == MUST_MOVE_ONTO) col &= 0xFFF0;
		pos->tableau[i] = col;
		pos->numHold += moved;
		SortHoldingArea(pos);
	}

	/* see if another column can move here */
	for (j=0; j<8; j++) if ((pos->tableau[j] >> 8) == want+0x80) {
		if (SeqLen(pos->tableau[j])+1 <= MaxMove(pos)) {
			Canonical(pos);
			return;
		}
	}

	/* see if this entire column can move onto another */
	if (col && BeenEmpty(col) && Restrict(col) < MUST_MOVE_FROM) {
		int onto = Available(pos, NewTop(col)+1);
		if (onto >= 0 && onto <= 7) {
			Canonical(pos);
			return;
		}
	}
}

/*
 * Move generator; given a column index and index of first empty column
 * (if any), decide the unique reasonable move (if any) from the given
 * column and apply supplied recursive proc.  Do not try partial sequence
 * moves at this time.
 */
static Position *
DFScol(via, pos, i, empty, proc)
     Position *via, *pos; int i, empty; Position *(*proc)();
{
	Column tcol, col = via->tableau[i];
	int target, slen = SeqLen(col)+1;
	int canMove = MaxMove(via);
	Card seqtop;
	       
	if (col == 0 || slen > canMove || CantMove(col)) return(pos);
	seqtop = Bottom(via, i) + slen - 1;
	target = Available(via, seqtop+1);
	if (target >= 0 && target <= 7) {
		/* move seq to tableau */
		*pos = *via;
		pos->via = via;
		tcol = pos->tableau[target] + (slen << 4);
		if (Restrict(tcol) == MUST_MOVE_ONTO) tcol &= 0xFFF0;
		pos->tableau[target] = tcol;
		MiniCanon(pos, Uncover(pos, i));
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	else if (empty >= 0 && slen <= (canMove>>1) && ! BeenEmpty(col)) {
		/* move entire seq (if less than whole col) to empty column */
		*pos = *via;
		pos->via = via;
		pos->tableau[empty] = 0x8000 + (seqtop<<8) + ((slen-1)<<4);
		pos->spaces--;
		SortEmpties(pos);
		MiniCanon(pos, Uncover(pos, i));
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	else if (empty < 0 && slen <= via->numHold) {
		/* move seq to hold */
		*pos = *via;
		pos->via = via;
		AddToHold(pos, seqtop, slen-1);
		MiniCanon(pos, Uncover(pos, i));
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	else if (BeenEmpty(col) && slen <= via->numHold &&
		 (via->numHold - slen + 1) << (via->spaces + 1) >= canMove) {
		/* move entire column to hold if increases MaxMove */
		*pos = *via;
		pos->via = via;
		AddToHold(pos, seqtop, slen-1);
		(void) Uncover(pos, i);
		Canonical(pos);
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	return(pos);
}

/*
 * Move generator; given an empty column, try moving down each free cell
 * unless another free cell contains the next higher card.
 */
static Position *
DFSempty(via, pos, empty, from, thru, proc)
     Position *via, *pos; int empty, from, thru; Position *(*proc)();
{
	int i;
	Card prev = 0;

	for (i=from; i<=thru; i++) {
		Card c = via->hold[i];
		if (c == 0) return(pos);
		if (c != prev - 1) {
			/* just move card down and let MiniCanon do the rest */
			*pos = *via;
			pos->via = via;
			pos->tableau[empty] = 0x8000 + (c<<8) + MUST_MOVE_ONTO;
			pos->hold[i] = 0;
			pos->numHold++;
			pos->spaces--;
			SortHoldingArea(pos);
			MiniCanon(pos, empty);
			SortEmpties(pos);
			if (AddToTree(pos)) {
				pos = (*proc)(pos, NewPosition());
				if (pos == NULL) return(NULL);
			}
		}
		prev = c;
	}
	return(pos);
}

/*
 * Move generator; try moving only part of a sequence to an empty column.
 * Called even if allowPartial is false, so we can set couldPartial if such
 * move was available.  Not called unless there's an empty column.
 */
static Position *
DFSpart(via, pos, i, empty, proc)
     Position *via, *pos; int i, empty; Position *(*proc)();
{
	Column col = via->tableau[i];
	int slen = SeqLen(col);	/* don't add 1 as we did in DFScol */
	int canMove = MaxMove(via);
	Card seqtop;

	if (col == 0 || slen < 1 || slen >= canMove || CantMove(col))
		return(pos);
	if (!allowPartial) {couldPartial = true; return(pos);}
	seqtop = Bottom(via, i) + slen;
	while (slen > (canMove>>1)) --seqtop, --slen;
	while (--slen >= 0) {
		/* move slen+1 cards into empty column */
		*pos = *via;
		pos->via = via;
		MovedFrom(pos, i);
		pos->tableau[empty] = 0x8000 + (--seqtop << 8)
			+ (slen << 4) + (MUST_MOVE_FROM + i);
		pos->tableau[i] += MOVE_AWAITED - ((slen+1) << 4);
		pos->spaces--;
		SortEmpties(pos);
		if (AddToTree(pos)) {
			partial++;
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	return(pos);
}

/* Depth-first search.  Generate all canonical positions reachable from "via".
 * Use "pos" as the place to construct new positions; ask for a new pos struct
 * as needed.  For positions that haven't already been reached, add them and
 * call DFS recursively.  If a winning position is found, return NULL.  Else
 * return a pointer to a not-yet-used Position struct.
 */
static Position *
DFS(via, pos) Position *via, *pos;
{
	int i, target, empty = -1;

	if (verbosity >= 2) {
		Position *p = via->via;
		while (p != NULL) {printf(" "); p = p->via;}
		ShowPosition(via);
		fflush(stdout);
	}

	/* check for victory */
	if (via->foundations[0] == 13 && via->foundations[1] == 13
		&& via->foundations[2] == 13 && via->foundations[3] == 13) {
		if (show) ShowPath(via);
		return(NULL);
	}

	if (via->spaces)
		for (i=0; i<8 && empty<0; i++)
			if (via->tableau[i] == 0) empty = i;

	/* look at moves from tableau columns */
	for (i=0; i<8; i++) {
		pos = DFScol(via, pos, i, empty, DFS);
		if (pos == NULL) return(NULL);
	}

	/* if there's an empty space, consider moving down from free cells */
	if (empty >= 0) {
		pos = DFSempty(via, pos, empty, 0, 3, DFS);
		if (pos == NULL) return(NULL);
	}

	/* if all else fails, try splitting a sequence (if allowed) */
	if (empty >= 0) {
		for (i=0; i<8; i++) {
			pos = DFSpart(via, pos, i, empty, DFS);
			if (pos == NULL) return(NULL);
		}
	}

	return(pos);
}

/* Initialise a Position structure to represent the original position:
 * Each column contains 6-7 original cards plus no extras.  Actually set
 * each column to 7-8 cards, then use Uncover to remove a card (and maximise
 * sequences where possible).  Note: Encoding for column doesn't break if
 * number of original cards exceeds 3 bits.
 */
static void
InitialPosition(pos) Position *pos;
{
	int i;
	
	memset(pos, 0, sizeof(*pos));
	pos->numHold = 4;
	for (i=0; i<4; i++) pos->tableau[i] = (8<<8); /* 8 original cards */
	for (i=4; i<8; i++) pos->tableau[i] = (7<<8); /* 7 original cards */
	for (i=0; i<8; i++) (void) Uncover(pos, i);
}

static char *initSeq;

/* Make initial moves specified by numeric command line switches: 0-7 is
 * column to move from (to other column if possible, else to space unless
 * source column is becoming empty, else to free cells); if source column
 * is empty next digit is index into sorted free cells to bring down.
 * Use same functions as regular search to make the moves, but tell them
 * to call DFSinit instead of DFS to process.
 */
static Position *
DFSinit(via, pos) Position *via, *pos;
{
	int i, empty = -1, n = *initSeq++ - '0';

	if (n < 0) return DFS(via, pos);

	if (verbosity >= 2) {
		Position *p = via->via;
		if (p != NULL) {printf("*"); p = p->via;}
		while (p != NULL) {printf(" "); p = p->via;}
		ShowPosition(via);
		fflush(stdout);
	}

	for (i=0; i<8; i++) if (via->tableau[i] == 0) {empty = i; break;}
	if (via->tableau[n] == 0) {
		n = *initSeq++ - '0';
		return DFSempty(via, pos, empty, n, n, DFSinit);
	}
	else
		return DFScol(via, pos, n, empty, DFSinit);
}

void
ReportStats()
{
	if (verbosity == 0) return;
	printf("\nGenerated %d positions (%d distinct).\n",
	       generated, distinct);
	if (allowPartial) printf("Tried %d partial-sequence moves.\n", partial);
	printf("Used %d of %d hash table slots.\n\n", hashes, HASH_SIZE);
}

void
Cleanup()
{
	/* clean up to prepare for next position */
	generated = distinct = hashes = 0;
	memset(tree, 0, HASH_SIZE * sizeof(Position*));
	while (block->link != NULL) {
		Block *link = block->link;
		free(block);
		block = link;
	}
	pos_avail = POS_BLOCK;
}

main(argc, argv) int argc; char *argv[];
{
	Position *pos0;
	int i, ap, reps = 1, make_random = false;
	char *filename = NULL, init[100];

	RandInit(79);
	initSeq = init;
	for (i=1; i<argc; i++) {
		char *arg = argv[i];
		if (*arg == '-') {
			while (*++arg) switch (*arg) {
			case 'i':
				showorig = true;
				break;
			case 'r':
				make_random = true;
				break;
			case 's':
				show = true;
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7':
				*initSeq++ = *arg;
				break;
			case 'v':
				if (verbosity++ < 2) break;
				/* FALL THROUGH */
			default:
				printf("\
Usage: baker [-irsvv###...] [file]\n\n\
-i:  show initial layout\n\
-r:  if no 'file' arg, generate random winnable position\n\
     else convert filename to int, generate and test that many positions\n\
-s:  show solution if found (else just report whether one exists)\n\
-v:  verbose; give some statistics when done\n\
-vv: very verbose; dump entire search tree as it is traversed\n\
-#:  column numbers (0-7) representing proposed initial move sequence\n");
				exit(1);
			}
		}
		else if (filename == NULL) filename = arg;
	}
	*initSeq = '\0';

	if (make_random) {
		BuildOriginal();
		if (filename != NULL) reps = atoi(filename);
		if (reps <= 0) reps = 1;
	}
	else
		ReadOriginal(filename);
	
	tree = (Position **) calloc(HASH_SIZE, sizeof(Position*));
	if (tree == NULL) {
		printf("\nERROR: Out of memory!\n");
		exit(2);
	}

	for (i=0; i<reps; i++) {
		if (make_random) Shuffle();
		if (showorig) ShowOriginal();

		for (ap=0; ap<2; ap++) {
			pos0 = NewPosition();
			InitialPosition(pos0);
			Canonical(pos0);
			(void) AddToTree(pos0);
			initSeq = init;
			allowPartial = (ap != 0);
			couldPartial = false;
			if (DFSinit(pos0, NewPosition()) == NULL) {
				if (!showorig && make_random && filename==NULL)
					ShowOriginal();
				printf("%sinnable.\n", allowPartial?
					"Tricky but w" : "W");
				ap++;
			}
			else if (couldPartial) {
				printf("Difficult, will try harder.\n");
			}
			else {
				printf("Impossible.\n");
				if (make_random && filename==NULL) i--;
				ap++;
			}
			ReportStats();
			Cleanup();
		}
	}

	exit(0);
}
----------
X-Sun-Data-Type: c-file
X-Sun-Data-Description: c-file
X-Sun-Data-Name: free1.c
X-Sun-Charset: us-ascii
X-Sun-Content-Lines: 1071

/*
 * Program for testing winnability of opening positions in Free Cell.
 * Can also generate random positions, optionally filtered for winnability.
 * Does not actually support interactive playing of the game.  See below for
 * a brief explanation of the rules.
 *
 * (c) Copyright 1994, Donald R. Woods.  Shareware: Right is granted to
 *	freely redistribute this source provided this notice is retained.
 */

/*****************************************************************************
Brief summary of the game:

Deal four columns of six cards each, and four of seven cards each, spreading
each column so the cards are all visible.  These columns form the "tableau".
There is a "holding area" that can hold a maximum of four cards.  The only
cards that can be moved are the bottommost card in each column and the cards
in the holding area.  As Aces become available (i.e., can be moved) they are
moved to start four "foundation" piles, which are then built up in suit to
Kings.  The object is to move all the cards to the foundations.

A move consists of moving one card.  A card can move onto a tableau column
if the card being moved is the opposite color and one rank lower than the
card it is being played onto (i.e., the tableau builds down, alternating
color).  Any card can be moved to an empty holding area spot, or to an empty
column in the tableau.

There is a variant using ten column of five cards each (with the other two
cards starting in the holding area), where the tableau builds down in suit
instead of alternating color, and only kings can be moved into empty tableau
columns.  This is sometimes called Seahaven Towers.  It is a somewhat easier
game to solve since more moves are "forced" (in the sense of being provably
good).


General approach: Depth-first search seems to be fast enough, so we won't try
anything fancier.

Positions are hashed and cached so the search doesn't expand them more than
once.  A position is cached in canonical form, obtained by sorting the cards
in the holding area and -- for the purposes of comparison of positions --
sorting the tableau columns based on the cards at the head of each column.
(Actually sorting the columns fails due to the need to maintain a matching
data structure that maps each card to its position.)

Part of the data structure representing a position gives just ranks and
colors.  A separate part notes, for each card (rank+suit, not just color),
where the card is located, and whether it is eligible to be interchanged
with the like-color card (because the resulting position was reached
elsewhere in the search tree), or whether it MUST be interchanged (because
at some point the only accessible card of the pair was moved to a
foundation pile).  A newly-reached position can be ignored if the first
part of the data structure matches a previously-reached position and the
second part has no cards that can be interchanged where the previous
position didn't.

The only "forced moves" entail moving a card to the foundation piles if
(a) it is eligible to be moved there and (b) both of the cards that could
be played on this one (i.e., next lower rank and opposite color) are
either already on the foundations or are eligible to be played there.
Forced moves are made one card at a time to simplify undoing them, but no
other moves are considered if a forced move is available.

Input format is 52 cards represented either as rank then suit or suit then
rank, where suit is one of CDHS and rank is one of A23456789TJQK.  Lower-case
is okay too.  Other characters such as spaces and line breaks are ignored and
can be used for readability.  E.g.:

7h 9s Kc 5d 8h 3h 6d 9d
7d As 3c Js 8c Kd 2d Ac
2s Qc Jh 8d Th Ts 9h 5h
Qs 6c 4s 6h Ad 8s 2h 4d
Ah 7s 3d Jd 9c 3s Qd Kh
7c 4c Jc Qh 2c Tc 5s 4h
5c Td Ks 6s

*****************************************************************************/

#include <stdio.h>
#include <string.h>

#define true  (1==1)
#define false (1==0)

typedef unsigned char	uchar;
typedef uchar		Pair;	/* 000crrrr: c=color(0=black), r=rank(1-13) */
typedef uchar		Card;

#define Rank(card)	((card) & 0x0F)
#define Suit(card)	((card) >> 4)
#define Colormate(card)	((card) ^ 0x20)
#define Either(card)	((card) & 0x1F)

typedef uchar		Loc;	/* cccnnnnn: c=column, n=index in column; or
				   00011111: in holding area; or
				   11111111: on foundation */
#define HOLDING		0x1F
#define FOUNDATION	0xFF

#define Col(loc)	((loc) >> 5)
#define Index(loc)	((loc) & 0x1F)

typedef uchar		Swap;	/* 0 = cannot be swapped (includes cases where
				   it could've been but the other card has
				   since been removed), 1 = can be swapped,
				   2 =has been swapped (matters only when
				   reconstructing path to a position) */
#define NO_SWAP		0
#define CAN_SWAP	1
#define DID_SWAP	2

typedef struct column_struct {
	uchar	count;		/* number of cards in column */
	Pair	cards[19];
} Column;

typedef struct pos_struct {
	uchar	foundations[4];
	Pair	hold[4];
	uchar	colSeq[8];	/* sorted order for tableau columns */
	Column	tableau[8];
	Loc	location[52];
	Swap	swappable[26];
	Card	moved;		/* most recently moved card */
	int	mustTry;	/* must move to/from this col if possible */
	struct pos_struct *via;	  /* position from which this was reached */
	struct pos_struct *swapvia; /* ditto but with most recently moved
				       card swapped with its mate */
	struct pos_struct *chain; /* for chaining in hash table */
	long	hash;
} Position;

#define SortedCol(pos, index)	&(pos)->tableau[(pos)->colSeq[index]]

int cardToIndex[0x3F];		/* maps Card -> index in info[] (0-51) */

#define CLUBS	 (0 << 4)
#define DIAMONDS (1 << 4)
#define SPADES	 (2 << 4)
#define HEARTS   (3 << 4)

static int verbosity = 0;	/* how much to print out while running */
static int show = false;	/* whether to show solution if found */
static int showorig = false;	/* whether to show original layout */
static int maxdepth = 100000;	/* max search depth before giving up a line */

#define TRACTABLE 200000	/* max distinct positions before giving up */

/* statistics gathering */
static long generated = 0;	/* # times AddToTree called */
static long distinct = 0;	/* # different positions reached */
static long hashes = 0;		/* # hash table slots used */
static long rejected = 0;	/* # rejected due to repeated column */
static long swaps = 0;		/* # positions combining lines via swaps */
static long maxout = 0;		/* # trees pruned for hitting max depth,
				   or max depth reached if no limit imposed */
static long maxfree = 0;	/* max of (open holds+1) * (2^spaces) */
static long brokeSeq = 0;	/* # trees pruned due to breaking sequences */
static long windepth;		/* depth of winning line */

/* Random number package.  Roll our own since so many systems seem to have
 * pretty bad ones.  Include code for both MSDOS and UNIX time functions to
 * initialise it.
 */
#ifdef __MSDOS__
#include "time.h"
#define GET_TIME(var) {time(&var);}
#else
#include "sys/time.h"
#define GET_TIME(var) { \
	struct timeval now; \
	gettimeofday(&now, 0); \
	var = now.tv_sec + now.tv_usec; \
}
#endif

#define RAND_BUFSIZE	98
#define RAND_HASHWORD	27
#define RAND_BITSUSED	29
#define RAND_MASK	((1 << RAND_BITSUSED) - 1)

static long rand_buf[RAND_BUFSIZE], *rand_bufLoc;
static long Rand();
static char rand_flips[1<<6];
static short rand_flipper;

#define RAND_FLIPMASK (sizeof(rand_flips) - 1)

static RandFlipInit(loc) int loc;
{
	static char f[] =
	   " 11 111 1  1    11 1 111 1  11      11 11111   11 1 1 11  1 11  ";

	rand_flipper = loc;
	for (loc=0; loc<=RAND_FLIPMASK; loc++)
		rand_flips[loc] = (f[loc]==' '? 0 : -1);
}

static RandInit(seed) long seed;
{
	int n;

	if (seed == 0) GET_TIME(seed);
	memset(rand_buf, 0, sizeof(rand_buf));
	for (n=0; n<RAND_BITSUSED; n++)
	{	rand_buf[n] = RAND_MASK >> n;
		rand_buf[n+RAND_BITSUSED+3] = 1 << n;
	}
	for (n=RAND_BITSUSED*2+3; n<RAND_BUFSIZE; n++)
	{	rand_buf[n] = seed & RAND_MASK;
		seed = seed * 3 + 01234567;
	}
	rand_bufLoc = rand_buf;
	RandFlipInit(0);
	for (n=(seed & 0377); n<50000; n++) Rand();
}

static long Rand()
{
	long f;

	if (!rand_bufLoc) RandInit(0);
	rand_bufLoc = (rand_bufLoc == rand_buf ?
		rand_bufLoc+RAND_BUFSIZE-1 : rand_bufLoc-1);
	*rand_bufLoc ^= *(rand_bufLoc >= rand_buf+RAND_HASHWORD ?
		rand_bufLoc-RAND_HASHWORD
		: rand_bufLoc+(RAND_BUFSIZE-RAND_HASHWORD));
	f = rand_flips[rand_flipper++ & RAND_FLIPMASK];
	return(*rand_bufLoc ^ (f & RAND_MASK));
}

/* End of random number package
 *******************************/

static Position orig;	/* special in that only tableau[] is set, and it
			   stores Cards instead of Pairs */

/* Read original position from stdin into orig.
 */
static void ReadOriginal(filename) char *filename;
{
	int count = 0;
	Card cd = 0;
	FILE *f = stdin;

	if (filename != NULL && (f = fopen(filename, "r")) == NULL) {
		printf("free_cell: can't open %s\n", filename);
		exit(1);
	}
	memset(&orig, 0, sizeof(orig));
	while (count < 104) {
		int c = getc(f);
		if (c == EOF) {
			printf("Insufficient input.\n");
			exit(1);
		}
		if (c >= '2' && c <= '9') cd += c - '0';
		else if ((c |= 0x20) == 'a') cd += 1;
		else if (c == 't') cd += 10;
		else if (c == 'j') cd += 11;
		else if (c == 'q') cd += 12;
		else if (c == 'k') cd += 13;
		else if (c == 'c') cd += CLUBS;
		else if (c == 'd') cd += DIAMONDS;
		else if (c == 'h') cd += HEARTS;
		else if (c == 's') cd += SPADES;
		else continue;
		if (count++ % 2 != 0) {
			Column *col = &orig.tableau[((count-1)>>1)%8];
			col->cards[col->count++] = (Pair) cd;
			cd = 0;
		}
	}
	if (filename != NULL) fclose(f);
}

/* Build original as simple sorted deck.
 */
static void BuildOriginal()
{
	int i;

	memset(&orig, 0, sizeof(orig));
	for (i=0; i<52; i++) {
		Column *col = &orig.tableau[i%8];
		Card cd = (i>>2)+1 + ((i&3)<<4);
		col->cards[col->count++] = (Pair) cd;
	}
}

/* Shuffle the starting positions in orig.
 */
static Shuffle()
{
	int i = 52;
	
	while (i > 1) {
		int swap = Rand() % (i--);
		Pair *c1 = &orig.tableau[i%8].cards[i/8];
		Pair *c2 = &orig.tableau[swap%8].cards[swap/8];
		Pair temp = *c1;
		*c1 = *c2;
		*c2 = temp;
	}
}

/* Initialise a Position record based on orig.
 */
static InitialPosition(pos) Position *pos;
{
	int i;

	for (i=0; i<52; i++) {
		cardToIndex[((i/13)<<4)+(i%13)+1] = i;
		pos->location[i] = 0xFF;
	}
	for (i=0; i<52; i++) {
		int col = (i % 8), index = (i / 8);
		Card c = (Card) orig.tableau[col].cards[index];
		int x = cardToIndex[c];
		if (pos->location[x] != 0xFF) {
			printf("Card %02x appears twice!\n", c);
			exit(1);
		}
		pos->tableau[col].cards[index] = Either(c);
		pos->tableau[col].count++;
		pos->location[x] = (col<<5) + index;
	}
	memset(pos->swappable, NO_SWAP, sizeof(pos->swappable));
	for (i=0; i<8; i++) pos->colSeq[i] = i;
}

/* Allocate a new Position structure.  Allocates many at a time to reduce
 * malloc overhead.  Saves linked list of blocks of positions to enable
 * freeing the storage, which can be necessary if we're examining hundreds
 * of positions in a single run of the program.
 */
#define POS_BLOCK 2000
typedef struct pos_block {
	Position block[POS_BLOCK];
	struct pos_block *link;
} Block;

static int pos_avail = 0;
static Block *block = NULL;

static Position *NewPosition()
{
	if (pos_avail-- == 0) {
		Block *more = (Block *) calloc(1, sizeof(Block));
		if (more == NULL) {
			printf("\nERROR: Out of memory!\n");
			exit(2);
		}
		more->link = block;
		block = more;
		pos_avail = POS_BLOCK - 1;
	}
	return (block->block + pos_avail);
}

/* Show a card; rank in uppercase, suit in lowercase, trailing space.
 */
ShowCard(card) Card card;
{
	static char *rank = "?A23456789TJQK", *suit = "cdsh";
	
	if (card == 0) printf(".. ");
	else printf("%c%c ", rank[Rank(card)], suit[Suit(card)]);
}

/* Show the topmost card on a foundation pile.
 */
ShowFoundation(pos, s) Position *pos; int s;
{
	uchar r = pos->foundations[s];
	if (r == 0) printf("   "); else ShowCard((s<<4)+r);
}

/* Show a position as a rectangular layout.
 */
void ReadablePosition(pos) Position *pos;
{
	int suit, rank, h = 0;
	int row=0, col;

	for (suit=3; suit>=0; suit--) for (rank=13; rank>0; rank--) {
		Card c = (suit<<4) + rank;
		Loc where = pos->location[cardToIndex[c]];
		if (where == HOLDING)
			pos->hold[h++] = c;
		else if (where != FOUNDATION)
			pos->tableau[Col(where)].cards[Index(where)] = c;
	}
	for (col=0; col<4; col++) {
		ShowCard(pos->hold[col]);
		pos->hold[col] &= 0x1F;
	}
	for (col=0; col<4; col++) ShowFoundation(pos, col);
	printf("\n");
	while (true) {
		int none = true;
		for (col=0; col<8; col++) {
			int cd = pos->tableau[col].cards[row];
			pos->tableau[col].cards[row] &= 0x1F;
			if (cd) {ShowCard(cd); none = false;}
			else printf("   ");
		}
		printf("\n");
		if (none) break;
		row++;
	}
	fflush(stdout);
}

int foobar = 0;

#define HASH_SIZE 19001
static Position **tree;

/* See if this position is already in the search tree.  If so, return false.
 * Else add it and return true.
 */
static int AddToTree(pos) Position *pos;
{
	long hash = 0;
	int i;
	Position **probe;

	generated++;
	for (i=0; i<4; i++) hash = hash*3 + pos->hold[i];
	for (i=0; i<8; i++) {
		Column *col = SortedCol(pos, i);
		int ct = col->count;
		hash *= 5;
		if (ct) hash += col->cards[ct-1];
	}
	pos->hash = hash;
	pos->swapvia = NULL;
	probe = &tree[hash % HASH_SIZE];
	if (*probe == NULL) hashes++;
	else while (*probe != NULL) {
		if ((*probe)->hash == hash && memcmp((*probe)->hold, pos->hold,
		  4*sizeof(Card)) == 0) {
			for (i=0; i<8; i++) {
				Column *c1 = SortedCol(pos, i);
				Column *c2 = SortedCol(*probe, i);
				if (c1->count != c2->count) break;
				if (c1->count == 0) {i=8; break;}
				if (memcmp(c1->cards, c2->cards,
				      c1->count*sizeof(Card)) != 0) break;
			}
			if (i == 8) for (i=0; i<26; i++) {
				if (pos->swappable[i] > (*probe)->swappable[i])
					break;
			}
			if (i == 26) {
				int latest = cardToIndex[pos->moved],
				    mate = cardToIndex[Colormate(pos->moved)],
				    either = cardToIndex[Either(pos->moved)];
				if (pos->swappable[either] == NO_SWAP &&
				    pos->location[latest] !=
				       (*probe)->location[latest] &&
				    pos->location[mate] ==
				       (*probe)->location[latest]) {
					pos->swappable[either] = CAN_SWAP;
					pos->swapvia = *probe;
				}
				else
					return(false);
			}
		}
		probe = &((*probe)->chain);
	}
	if (pos->swapvia) swaps++;
	*probe = pos;
	pos->chain = NULL;
	if ((distinct++ & 0x7FFF) == 0x7FFF && verbosity == 1) {
		printf("%d distinct positions...\n", distinct);
		fflush(stdout);
	}
	return(true);
}

/* Return true if tableau has an empty column.
 */
static int HasSpace(pos) Position *pos;
{
	return(pos->tableau[pos->colSeq[7]].count == 0);
}

/* Return card at bottom of column (resolving Pair to single Card).
 */
static Card Bottom(pos, i) Position *pos; int i;
{
	Column *col = &pos->tableau[i];
	Card cd = col->cards[col->count - 1];
	if (pos->location[cardToIndex[cd]] != (i<<5) + col->count - 1)
		cd ^= 0x20;
	return(cd);
}

/* Return true if card is available to be played.
 */
static int Available(pos, card) Position *pos; Card card;
{
	Loc where;

	if (card == 0) return(false);
	where = pos->location[cardToIndex[card]];
	if (where == HOLDING) return(true);
	if (where == FOUNDATION) return(false);
	if (pos->tableau[Col(where)].count == Index(where)+1) return(true);
	return(false);
}

/* Sort the holding area into descending order (hence spaces come last).
 * Use homogeneous sorting network.
 */
static void SortHoldingArea(pos) Position *pos;
{
	Card *h = pos->hold;
	Card temp;

#define SWAP(i,j) if (h[i]<h[j]) {temp=h[i]; h[i]=h[j]; h[j]=temp;}

	SWAP(0,1)
	SWAP(2,3)
	SWAP(0,2)
	SWAP(1,3)
	SWAP(1,2)
}

/* Sort the tableau columns based on the top cards of the columns.
 * Only called when the top card of a column actually changes.  Note
 * that the columns will be mostly sorted already.
 */
static void SortColumns(pos) Position *pos;
{
	int thru = 7;

	while (thru > 0) {
		int i, need = 0;
		for (i=1; i<=thru; i++) {
			if (pos->tableau[pos->colSeq[i]].cards[0] > 
			    pos->tableau[pos->colSeq[i-1]].cards[0]) {
				uchar temp = pos->colSeq[i];
				pos->colSeq[i] = pos->colSeq[i-1];
				pos->colSeq[i-1] = temp;
				need = i-1;
			}
		}
		thru = need;
	}
}

/* Move the specified card from wherever it is to new location.  If this
 * requires swapping a swappable pair, update struct accordingly.  Assumes
 * destination of move is legal for card being moved.
 */
static Loc MoveCard(pos, card, whither) Position *pos; Card card; Loc whither;
{
	Loc *where = &pos->location[cardToIndex[card]];

	if (!Available(pos, card)) {
		Loc temp, *where2 = &pos->location[cardToIndex[Colormate(card)]];
		Swap *swap = &pos->swappable[cardToIndex[Either(card)]];
		if (*swap != CAN_SWAP
		    || !Available(pos, Colormate(card))) {
			printf("Bug! Move of unavailable card.\n");
			exit(2);
		}
		*swap = DID_SWAP;
		temp = *where;
		*where = *where2;
		*where2 = temp;
	}

	pos->mustTry = (-1);
	if (*where == HOLDING) {
		int i;
		for (i=0; i<4; i++) if (pos->hold[i] == Either(card)) {
			pos->hold[i] = 0;
			break;
		}
		SortHoldingArea(pos);
	}
	else {
		Column *col = &pos->tableau[Col(*where)];
		col->count--;
		col->cards[col->count] = 0;
		if (col->count == 0) SortColumns(pos);
		else if (whither != FOUNDATION &&
		    col->cards[col->count-1] == ((Either(card)+1) ^ 0x10))
			pos->mustTry = Col(*where);
	}

	*where = whither;
	if (whither == FOUNDATION)
		pos->foundations[Suit(card)]++;
	else if (whither == HOLDING) {
		pos->hold[3] = Either(card);
		SortHoldingArea(pos);
	}
	else {
		Column *col = &pos->tableau[Col(whither)];
		col->cards[col->count++] = Either(card);
		if (col->count == 1) SortColumns(pos);
		if (Index(whither) != col->count-1) {
			printf("Bug! Moved to wrong index in column.\n");
			exit(2);
		}
	}

	pos->moved = card;
}

static depth = 0;

static ShowMove(pos, tag, swap) Position *pos; char *tag; int swap;
{
	Card moved = pos->moved;
	Loc whither = pos->location[cardToIndex[moved]];

	printf("%4d: ", depth);
	ShowCard(swap? Colormate(moved) : moved);
	printf("-> ");
	if (whither == HOLDING) printf("free cell");
	else if (whither == FOUNDATION) printf("foundation");
	else printf("tableau %d", Col(whither)+1);
	if (tag) printf(" (%s)", tag);
	printf("\n");
	if (verbosity > 2) fflush(stdout);
}

/* Show the winning path, in reverse order (because it's convenient).
 */
static int FollowPath(pos, tally) Position *pos; int tally;
{
	Position *p = pos;
	int steps = 0;

	while (p->moved) {
		int swap = false, index = cardToIndex[Either(p->moved)];
		if (pos->swappable[index] == DID_SWAP &&
		    p->swappable[index] == CAN_SWAP) {
			if (p->swapvia) {
				if (!tally) {
					printf("  Bypassing seq that swaps ");
					ShowCard(p->moved);
					printf(" with ");
					ShowCard(Colormate(p->moved));
					printf("\n");
				}
				p = p->swapvia;
				continue;
			}
			else swap = true;
		}
		if (tally) steps++;
		else {ReadablePosition(p); ShowMove(p, NULL, swap); depth--;}
		p = p->via;
	}
	if (tally) windepth = steps;
	return(steps);
}

static void ShowPath(pos) Position *pos;
{
	Position *p = pos;
	printf("Winning path (in reverse order):\n");
	depth = FollowPath(pos, true);
	(void) FollowPath(pos, false);
	printf("\n");
}

/* Make (and if verbose, print) a move, then recursively call DFS if the
 * resulting position is new.
 */
static Position *DFS(); /* forward decl */
static Position *TryMove(via, pos, card, whither)
     Position *via, *pos; Card card; Loc whither;
{
	Loc whence;
	int dup = false;

	depth++;
	*pos = *via;
	pos->via = via;
	MoveCard(pos, card, whither);
	if ((whither & 0x1F) != 0x1F) {
		/* moved to col; check for repetition */
		int i = Col(whither);
		Column *col = &pos->tableau[i];
		Position *prev;
		for (prev=via; prev; prev=prev->via) {
			Column *oldcol = &prev->tableau[i];
			if (oldcol->count != 0 &&
			    oldcol->cards[0] != col->cards[0]) break;
			if (oldcol->count == col->count &&
			    memcmp(oldcol->cards, col->cards,
				   col->count*sizeof(Card)) == 0) {
				int j;
				for (j=0; j<col->count; j++) {
					int x = cardToIndex[col->cards[j]];
					int xx = x;
					if (pos->location[x] != (i<<5)+j)
						xx += 26;
					if (prev->location[xx] != pos->location[xx]
					    || prev->swappable[x] != pos->swappable[x]) break;
				}
				if (j >= col->count) dup = true;
				break;
			}
		}
	}
	if (dup) {
		if (verbosity >= 2) ShowMove(pos, "rejected", false);
		rejected++;
	}
	else if (AddToTree(pos)) {
		if (verbosity >= 2)
			ShowMove(pos, (pos->swapvia? "merged" : NULL), false);
		pos = DFS(pos, NewPosition());
		if (pos == NULL) return(NULL);
	}
	else if (verbosity >= 2) ShowMove(pos, "old", false);
	depth--;
	return(pos);
}

/* Depth-first search.  Generate all positions reachable from "via".  (If
 * there is a forced move, just do the forced move.)  Use "pos" as the
 * place to construct new positions; ask for a new pos struct as needed.
 * For positions that haven't already been reached, add them and call DFS
 * recursively via TryMove.  If a winning position is found, return NULL.
 * Else return a pointer to a not-yet-used Position struct.
 */
static Position *DFS(via, pos) Position *via, *pos;
{
	int i, j, intab = 0, goodcols = 0; Card moved; Loc onto;

	/* check for victory */
	if (via->foundations[0] == 13 && via->foundations[1] == 13
		&& via->foundations[2] == 13 && via->foundations[3] == 13) {
		if (show) ShowPath(via);
		else if (verbosity > 0) (void) FollowPath(via, true);
		return(NULL);
	}

	if (maxdepth > 99999) {
		if (depth > maxout) maxout = depth;
	}
	else if (depth >= maxdepth) {
		maxout++;
		return(pos);
	}

	if (distinct >= TRACTABLE) return(pos);	/* give up */

	for (i=3; i>=0 && via->hold[i]==0; i--) {}
	for (j=7; j>=0 && via->tableau[via->colSeq[j]].count==0; j--) {}
	i = (3 - i + 1) << (7 - j);
	if (i > maxfree) {
		maxfree = i;
		/* if (i == 6) ReadablePosition(via); */
	}

	/* check for forced moves to foundation */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		int opp1 = (i^1), opp2 = (opp1^2);	/* other color suits */
		if (r > 13 || via->foundations[opp1]+2 < r ||
		    via->foundations[opp2]+2 < r) continue;
		/* move is forced if card is available; note that if card
		   is swappable with its colormate, then if either card is
		   available they both are, since next lower cards of other
		   color are by definition already moved to foundations */
		moved = (i<<4) + r;
		if (Available(via, moved))
			return TryMove(via, pos, moved, (Loc) FOUNDATION);
		/* card not available, but note good column to dig in */
		goodcols |= (1 << Col(via->location[cardToIndex[moved]]));
	}

	/* if previous move broke a sequence (and didn't move to foundation),
	   then mustTry will be >= 0, saying we must move to/from that column
	   if it is possible to do so */
	if (via->mustTry >= 0) {
		Column *col = &via->tableau[via->mustTry];
		int tried = false;
		moved = Bottom(via, via->mustTry);
		/* if not a King, try moving to another non-empty column */
		if (Rank(moved) < 13) {
			Card onto = (moved+1) ^ 0x10;
			for (i=0; i<2; i++) {
				Loc loc = via->location[cardToIndex[onto]];
				if (Available(via, onto) && loc != HOLDING) {
					tried = true;
					pos = TryMove(via, pos, moved, loc+1);
					if (pos == NULL) return(NULL);
				}
				onto = Colormate(onto);
			}
		}
		/* if can't move within tableau, try moving to holding area
		   or, if that's full, to empty column */
		onto = FOUNDATION;
		if (via->hold[3] == 0) onto = HOLDING;
		else if (HasSpace(via)) onto = (via->colSeq[7]<<5) + 0;
		if (!tried && onto != FOUNDATION &&
		    (col->count>1 || !HasSpace(via))) {
			tried = true;
			pos = TryMove(via, pos, moved, onto);
			if (pos == NULL) return(NULL);
		}
		/* if neither of the above, try move to foundation (if can move
		   elsewhere, will try moving to foundation from there) */
		if (!tried && via->foundations[Suit(moved)]+1 == Rank(moved)) {
			tried = true;
			pos = TryMove(via, pos, moved, (Loc) FOUNDATION);
			if (pos == NULL) return(NULL);
		}
		/* move colormate of just-moved card onto revealed card */
		if (Available(via, Colormate(via->moved))) {
			tried = true;
			pos = TryMove(via, pos, Colormate(via->moved),
				      (via->mustTry<<5) + col->count);
			if (pos == NULL) return(NULL);
		}
		/* if this column is a singleton and the only place it can
		   move is into another empty column, abandon the line */
		if (col->count == 1 && via->tableau[via->colSeq[7]].count == 0)
			tried = true;
		if (tried) {
			brokeSeq++;
			return(pos);
		}
	}

	/* try non-forced moves to foundation, but don't move the second
	   of colormates up if both lower cards are still in play */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		if (r > 13) continue;
		if (via->foundations[i^2] >= r && via->foundations[i^1]+2 < r
		    && via->foundations[i^3]+2 < r) continue;
		moved = (i<<4) + r;
		/* if card & colormate both avail, moving original suffices;
		   other must then be able to move to where original was */
		if (Available(via, moved)) {
			pos = TryMove(via, pos, moved, (Loc) FOUNDATION);
			if (pos == NULL) return(NULL);
		}
		else if (via->swappable[cardToIndex[Either(moved)]] &&
			 Available(via, Colormate(moved))) {
			pos = TryMove(via, pos, moved, (Loc) FOUNDATION);
			if (pos == NULL) return(NULL);
		}
	}

	/* next preference is moves onto the bottoms of non-empty columns;
	   cards which can do this never need to move to empty cols or
	   holding cells (though in some odd cases they may end up moving
	   from the other col to a holding cell) */
	for (i=0; i<8; i++) {
		Column *col = &via->tableau[i];
		if (col->count == 0) continue;
		/* bottom card guaranteed to be > Ace else force-move above */
		moved = (col->cards[col->count - 1] - 1) ^ 0x10;
		onto = (i<<5) + col->count;
		for (j=0; j<2; j++) {
			if (Available(via, moved)) {
				Loc where;
				pos = TryMove(via, pos, moved, onto);
				if (pos == NULL) return(NULL);
				where = via->location[cardToIndex[moved]];
				if (where != HOLDING)
					intab |= 1 << (Col(where));
			}
			moved ^= 0x20;
		}
	}

	/* next try moving from columns with 2+ cards to hold; if no hold,
	   move to empty columns; (if both, can get to empty cols via hold) */
	onto = FOUNDATION;
	if (via->hold[3] == 0)
		onto = HOLDING;
	else if (via->tableau[via->colSeq[7]].count == 0)
		onto = (via->colSeq[7]<<5) + 0;
	if (onto != FOUNDATION) {
		for (i=0; i<8; i++)
		  if ((goodcols & (1<<i)) != 0 && (intab & (1<<i)) == 0) {
			if (via->tableau[i].count < 2) continue;
			pos = TryMove(via, pos, Bottom(via, i), onto);
			if (pos == NULL) return(NULL);
		}
		for (i=0; i<8; i++)
		  if ((goodcols & (1<<i)) == 0 && (intab & (1<<i)) == 0) {
			if (via->tableau[i].count < 2) continue;
			pos = TryMove(via, pos, Bottom(via, i), onto);
			if (pos == NULL) return(NULL);
		}
	}

	/* try moving from holding area to empty column; vary which one is
	   tried first, in case the key move we're looking for is hold[3] */
	if (via->tableau[via->colSeq[7]].count == 0) {
		for (i=0; i<4; i++) {
			Card below;
			moved = via->hold[(i+(depth>>2))&3];
			if (moved == 0) continue;
			if (via->location[cardToIndex[moved]] != HOLDING)
				moved ^= 0x20;
			/* don't move a card down if both cards that could
			   use it are already on foundations */
			below = (moved-1) ^ 0x10;
			if (via->location[cardToIndex[below]] != FOUNDATION
			    || via->location[cardToIndex[below^0x20]]
			    != FOUNDATION) {
				pos = TryMove(via, pos, moved,
					      (Loc) ((via->colSeq[7]<<5)+0));
				if (pos == NULL) return(NULL);
			}
		}
	}

	/* try moving from singleton columns to holding area, but only if
	   there isn't another empty column already */
	if (via->hold[3] == 0 && !HasSpace(via)) {
		for (i=0; i<8; i++) if ((intab & (1<<i)) == 0) {
			if (via->tableau[i].count != 1) continue;
			pos = TryMove(via, pos, Bottom(via, i), (Loc) HOLDING);
			if (pos == NULL) return(NULL);
		}
	}

	/* last, try non-forced moves to foundation, where we're moving
	   up the second of colormates and both lower cards are in play */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		if (r > 13) continue;
		if (!(via->foundations[i^2] >= r && via->foundations[i^1]+2 < r
		    && via->foundations[i^3]+2 < r)) continue;
		moved = (i<<4) + r;
		if (Available(via, moved)) {
			pos = TryMove(via, pos, moved, (Loc) FOUNDATION);
			if (pos == NULL) return(NULL);
		}
	}

	return(pos);
}

main(argc, argv) int argc; char *argv[];
{
	Position *pos0;
	int i, reps = 1, make_random = false;
	char *filename = NULL;

	for (i=1; i<argc; i++) {
		char *arg = argv[i];
		if (*arg == '-') {
			while (*++arg) switch (*arg) {
			case 'i':
				showorig = true;
				break;
			case 'm':
				if (maxdepth > 99999) maxdepth = 300;
				else maxdepth += 200;
				break;
			case 'r':
				make_random = true;
				break;
			case 's':
				show = true;
				break;
			case 'v':
				if (verbosity++ < 3) break;
				/* FALL THROUGH */
			default:
				printf("\
Usage: free_cell [-imrsvv] [file]\n\n\
-i:  show initial layout\n\
-m:  set max search depth of 300 (+200 per extra m)\n\
-r:  if no 'file' arg, generate random winnable position\n\
     else convert filename to int, generate and test that many positions\n\
-s:  show solution if found (else just report whether one exists)\n\
-v:  verbose; give some statistics when done\n\
-vv: very verbose; dump entire search tree as it is traversed\n\
-vvv: same as -vv but does fflush after each step\n");
				exit(1);
			}
		}
		else if (filename == NULL) filename = arg;
	}

	if (make_random) {
		BuildOriginal();
		if (filename != NULL) reps = atoi(filename);
		if (reps <= 0) reps = 1;
	}
	else
		ReadOriginal(filename);
	
	tree = (Position **) calloc(HASH_SIZE, sizeof(Position*));
	if (tree == NULL) {
		printf("\nERROR: Out of memory!\n");
		exit(2);
	}

	for (i=0; i<reps; i++) {
		if (make_random) Shuffle();
		pos0 = NewPosition();
		InitialPosition(pos0);
		pos0->mustTry = (-1);
		if (showorig) ReadablePosition(pos0);
		SortColumns(pos0);
		(void) AddToTree(pos0);

		if (DFS(pos0, NewPosition()) == NULL) {
			if (make_random && filename==NULL && !showorig)
				ReadablePosition(pos0);
			printf("Winnable.\n");
		}
		else {
			printf(distinct>=TRACTABLE? "Intractable.\n"
				: "Impossible.\n");
			if (make_random && filename==NULL) i--;
		}

		if (verbosity > 0) {
			printf("\n");
			if (windepth)
				printf("Winning line of %d steps.\n", windepth);
			printf("\
Generated %d positions (%d distinct).\n\
Rejected %d due to repeated columns.\n\
Pruned %d lines due to broken sequences.\n\
Combined %d lines via pair-swapping.\n",
			       generated, distinct, rejected, brokeSeq, swaps);
			if (!windepth)
				printf("Maximum freedom was %d.\n", maxfree);
			if (maxdepth < 100000) printf("\
Abandoned %d lines for exceeding %d moves.\n", maxout, maxdepth);
			else printf("\
Maximum search depth was %d.\n", maxout);
			printf("Used %d of %d hash table slots.\n",
			       hashes, HASH_SIZE);
		}

		if (i < reps) {
			/* clean up to prepare for next position */
			generated = distinct = rejected = swaps = windepth =
			  maxout = maxfree = brokeSeq = hashes = depth = 0;
			memset(tree, 0, HASH_SIZE * sizeof(Position*));
			while (block->link != NULL) {
				Block *link = block->link;
				free(block);
				block = link;
			}
			pos_avail = POS_BLOCK;
			memset(block, 0, sizeof(*block));
			fflush(stdout);
		}
	}

	exit(0);
}

----------
X-Sun-Data-Type: c-file
X-Sun-Data-Description: c-file
X-Sun-Data-Name: free2.c
X-Sun-Charset: us-ascii
X-Sun-Content-Lines: 1746

/*
 * Program for testing winnability of opening positions in Free Cell.
 * Can also generate random positions, optionally filtered for winnability.
 * Does not actually support interactive playing of the game.  See below for
 * a brief explanation of the rules.
 *
 * (c) Copyright 1994, Donald R. Woods.  Shareware: Right is granted to
 *	freely redistribute this source provided this notice is retained.
 */

/*****************************************************************************
Brief summary of the game:

Deal four columns of six cards each, and four of seven cards each, spreading
each column so the cards are all visible.  These columns form the "tableau".
There is a "holding area" that can hold a maximum of four cards.  The only
cards that can be moved are the bottommost card in each column and the cards
in the holding area.  As Aces become available (i.e., can be moved) they are
moved to start four "foundation" piles, which are then built up in suit to
Kings.  The object is to move all the cards to the foundations.

A move consists of moving one card.  A card can move onto a tableau column
if the card being moved is the opposite color and one rank lower than the
card it is being played onto (i.e., the tableau builds down, alternating
color).  Any card can be moved to an empty holding area spot, or to an empty
column in the tableau.

There is a variant using ten column of five cards each (with the other two
cards starting in the holding area), where the tableau builds down in suit
instead of alternating color, and only kings can be moved into empty tableau
columns.  This is sometimes called Seahaven Towers.  It is a somewhat easier
game to solve since more moves are "forced" (in the sense of being provably
good).

General approach: Depth-first search, using "atomic" moves that for the most
part move only entire sequences (alternating color, descending rank), moving
part of a sequence only to move an interior card onto a foundation.  (Partial
sequences can also be moved as part of an atomic move that altogether moves
the entire sequence.)  There may be situations where moving a partial sequence
is necessary; a few such cases are included.  It is possible that a winning
move may be overlooked in some rare case.

Positions are hashed and cached so the search doesn't expand them more than
once.  A position is cached in canonical form, obtained by sorting the cards
in the holding area and -- for the purposes of comparison of positions --
sorting the tableau columns based on the cards at the head of each column.
(Actually sorting the columns fails due to the need to maintain a matching
data structure that maps each card to its position.)  Cards are also played
down from free cells to non-empty tableau columns.

The search rejects a position if it has been examined previously (with the
same or looser restrictions on later moves), or if a move into the tableau
causes the destination column to look exactly as it has earlier in the
current search path, unless in the interim the column has become empty and
has had a different card placed at the top.  Each position is also examined
for opportunities to swap cards with their "colormates"; subsequent positions
are then taken to include all positions that could be reached by having
swapped zero or more of such pairs.  Whether the swap is actually performed
isn't determined until one of the pair moves to a foundation.

The types of move, in the order in which they are considered, are as follows.
"*" denotes a forced move, i.e. if such a move is available it is made and no
others are considered.  "$" denotes moves that are tried first for "good"
columns, then for other columns, where "good" means the column either contains
a card that would be a forced move to a foundation if reached, or the column
has at most 2 cards.  "?T" refers to "dontMoveTo" flags, which denote columns
that cannot be moved onto until they have been moved from.  "?F" likewise
denotes columns that cannot be moved from until moved onto.

* declare victory if all foundations are built to Kings
* move to foundation if foundations of other color are built to within 2 of
  the rank of the card being considered (or higher), and either the other
  foundation of the same color is built to within 3 (or higher) or both
  foundations of the other color are built to within 1
* move an entire column (a single sequence) onto another, if ?T/?F permit
* if moving a free cell to a space would trigger the preceding rule, do it
  (if that free cell is part of a sequence, bring down the top of the sequence)
* check for swappable color-equivalent pairs (see below)
$ move a complete sequence from one column to another non-empty column
$ move a complete sequence into a single space
$ if no space exists, move a complete sequence into free cells (if it all fits)
$ move part of a sequence to another non-empty column
- move from free cell into a space, unless another free cell contains a card
  that this one could move onto (move it down instead); set ?F for that column
- move as much as will fit of a sequence into a single space, provided (a) the
  entire sequence can't be moved (such moves are tried earlier), (b) there is
  another space, and (c) there is a free cell card that, if moved into the
  other space, might eventually accept some or all of the remaining sequence
  (this move sets ?T for the source column)
- move a complete sequence into a combination of spaces and free cells (only
  for sequences that could not be moved in their entirety earlier)
- move a card to a foundation (i.e., foundation moves that aren't "forced")
- move part of a sequence into spaces and/or free cells to expose a card that
  can play to a foundation, and move that card to the foundation

The situations that are detected as color-equivalent are:
- a card and its colormate are both in the free cells
- a free cell card's colormate is in the bottom sequence of a column, not at
  the top of the sequence, and there's enough freedom to do the swap
- a free cell's colormate is at the top of a single-sequence column, and
  there's enough freedom (note that, in some but not all cases, you can do
  the swap with less freedom if you start by moving the free cell to a space)
- the card at the bottom of a column can be swapped with a card in the bottom
  sequence of another column (again, a special case arises when both are at
  the tops of columns)

Input format is 52 cards represented either as rank then suit or suit then
rank, where suit is one of CDHS and rank is one of A23456789TJQK.  Lower-case
is okay too.  Other characters such as spaces and line breaks are ignored and
can be used for readability.  E.g.:

7h 9s Kc 5d 8h 3h 6d 9d
7d As 3c Js 8c Kd 2d Ac
2s Qc Jh 8d Th Ts 9h 5h
Qs 6c 4s 6h Ad 8s 2h 4d
Ah 7s 3d Jd 9c 3s Qd Kh
7c 4c Jc Qh 2c Tc 5s 4h
5c Td Ks 6s

*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#define true  (1==1)
#define false (1==0)

#ifndef DEBUG
typedef unsigned char	uchar;
#else
typedef int uchar;
#endif

typedef uchar		Card;	/* 00ssrrrr: s=suit, r=rank(1-13) */

#define Rank(card)	((card) & 0x0F)
#define Suit(card)	((card) >> 4)
#define Either(card)	((card) & 0x1F)	/* reduce a card to color+rank */

#define CardBelow(card)	(((card)-1) ^ 0x10)	/* a card that goes on this */
#define CardAbove(card) (((card)+1) ^ 0x10)	/* a card this goes on */

typedef uchar		Loc;	/* cccnnnnn: c=column, n=index in column; or
				   00011111: in holding area; or
				   01111111: splitting among spaces/holding; or
				   11111111: on foundation; or
				   11011111: on foundation after pair-swap */
#define HOLDING		0x1F
#define MULTIPLE	0x7F
#define FOUNDATION	0xFF
#define SWAP_FOUND	0xDF

#define Col(loc)	((loc) >> 5)
#define Index(loc)	((loc) & 0x1F)

#define SpaceLoc(pos)	(((pos)->colSeq[7] << 5) + 0)

/* modified position data for storing in hash table */
typedef struct tree_entry {
	long	hash;
	uchar	cards[60];	/* sorted-tableau lengths & cards */
	uchar	other[4];	/* foundations or hold copied from full posn */
	uchar	dontMoveFrom, dontMoveTo;  /* copied from full position, qv */
	long	swappable;	/* bitmap of swappable pairs */
	struct tree_entry *chain; /* for chaining in hash table */
} TreeEntry;

typedef struct {
	Card	moved;
	Loc	whither;
	int	whatType;
} Move;

typedef struct {
	uchar	count;		/* number of cards in column */
	uchar	seq;		/* length of bottommost sequence */
	uchar	afterMate;	/* true if prev column starts with colormate */
	Card	cards[19];
} Column;

typedef struct pos_struct {
	uchar	foundations[4];
	Card	hold[4];
	uchar	colSeq[8];	/* sorted order for tableau columns */
	Column	tableau[8];
	Loc	location[52];
	Move	how;		/* move that got us here */
	uchar	dontMoveTo;	/* bitmap: cols that must shrink b4 growing */
	uchar	dontMoveFrom;	/* bitmap: cols that must grow b4 shrinking */
	uchar	goodcols;	/* columns w/ potential forced found'n moves */
	long	swappable;	/* bitmap of swappable pairs */
	long	swapped;	/* pairs that have been swapped */
	struct pos_struct *via;	  /* position from which this was reached */
	TreeEntry *tree;	/* hash table entry for this position */
	struct pos_struct *deeper; /* where to build next deeper position */
} Position;

static int cardToIndex[0x3F];	/* maps Card -> index in location[] (0-51) */

#define Where(pos, card)	pos->location[cardToIndex[card]]
#define SortedCol(pos, index)	(&(pos)->tableau[(pos)->colSeq[index]])
#define BitSet(val, bit)	(((val) & (1 << (bit))) != 0)
#define BitOff(val, bit)	(((val) & (1 << (bit))) == 0)

#define CLUBS	 (0 << 4)
#define DIAMONDS (1 << 4)
#define SPADES	 (2 << 4)
#define HEARTS   (3 << 4)

static jmp_buf longjmp_env;	/* environment for aborting recursion */

/* parameterisation */
static int verbosity = 0;	/* how much to print out while running */
static int show = false;	/* whether to show solution if found */
static int showorig = false;	/* whether to show original layout */
static int forceCombining = true; /* whether to force full-col-onto-other */
static int partials = true;	/* allow all partial-sequence moves */
static int tractLimit = 30000;	/* max distinct positions before giving up */
static int deadEnds = 0;	/* if >0, show dead ends that are this big */
static int trySwapping = true;	/* if true, merge color-equivalent lines */
static int lateNight = false;	/* if true, only run between 12:00-3:59am */

/* statistics gathering */
static long generated = 0;	/* # times AddToTree called */
static long distinct = 0;	/* # different positions reached */
static long matchedSwap = 0;	/* # rejected for matching via colorswaps */
static long hashes = 0;		/* # hash table slots used */
static long rejected = 0;	/* # rejected due to repeated column */
static long windepth;		/* depth of winning line */

static long moveTypes[20];	/* histogram of types of move tried */
static long moveTypesNew[20];	/* and how often it led to a new position */

/* Indices into moveTypes[] */
#define MV_FFORCE 0	/* forced move to foundation */
#define MV_TFORCE 1	/* forced move full column to tableau */
#define MV_SFORCE 2	/* forced move free cell to space to pick up column */
#define MV_TFULL  3	/* full sequence to non-empty tableau column */
#define MV_SFULL  4	/* full sequence to space */
#define MV_HFULL  5	/* full sequence to holding area */
#define MV_TPART  6	/* partial sequence to another column */
#define MV_THOLD  7	/* holding area to tableau */
#define MV_SPART  8	/* partial sequence to space */
#define MV_MFULL  9	/* full sequence to multiple spaces/free cells */
#define MV_FOPT  10	/* optional move to foundation */
#define MV_FDIG  11	/* partial sequence to uncover foundation move */

static char *moveDescs[] = {
	"forced move to foundation",
	"forced move full column to tableau",
	"forced move free cell to space to pick up column",
	"full sequence to non-empty tableau column",
	"full sequence to space",
	"full sequence to holding area",
	"partial sequence to another column",
	"holding area to tableau",
	"partial sequence to space",
	"full sequence to multiple spaces/free cells",
	"optional move to foundation",
	"partial sequence to uncover foundation move",
	NULL};

/* Random number package.  Roll our own since so many systems seem to have
 * pretty bad ones.  Include code for both MSDOS and UNIX time functions to
 * initialise it.
 */
#ifdef __MSDOS__
#include "time.h"
#define GET_TIME(var) {time(&var);}
#else
#include "sys/time.h"
#define GET_TIME(var) { \
	struct timeval now; \
	gettimeofday(&now, 0); \
	var = now.tv_sec + now.tv_usec; \
}
#endif

#define RAND_BUFSIZE	98
#define RAND_HASHWORD	27
#define RAND_BITSUSED	29
#define RAND_MASK	((1 << RAND_BITSUSED) - 1)

static long rand_buf[RAND_BUFSIZE], *rand_bufLoc;
static long Rand();
static char rand_flips[1<<6];
static short rand_flipper;

#define RAND_FLIPMASK (sizeof(rand_flips) - 1)

static RandFlipInit(loc) int loc;
{
	static char f[] =
	   " 11 111 1  1    11 1 111 1  11      11 11111   11 1 1 11  1 11  ";

	rand_flipper = loc;
	for (loc=0; loc<=RAND_FLIPMASK; loc++)
		rand_flips[loc] = (f[loc]==' '? 0 : -1);
}

static RandInit(seed) long seed;
{
	int n;

	if (seed == 0) GET_TIME(seed);
	memset(rand_buf, 0, sizeof(rand_buf));
	for (n=0; n<RAND_BITSUSED; n++)
	{	rand_buf[n] = RAND_MASK >> n;
		rand_buf[n+RAND_BITSUSED+3] = 1 << n;
	}
	for (n=RAND_BITSUSED*2+3; n<RAND_BUFSIZE; n++)
	{	rand_buf[n] = seed & RAND_MASK;
		seed = seed * 3 + 01234567;
	}
	rand_bufLoc = rand_buf;
	RandFlipInit(0);
	for (n=(seed & 0377); n<50000; n++) Rand();
}

static long Rand()
{
	long f;

	if (!rand_bufLoc) RandInit(0);
	rand_bufLoc = (rand_bufLoc == rand_buf ?
		rand_bufLoc+RAND_BUFSIZE-1 : rand_bufLoc-1);
	*rand_bufLoc ^= *(rand_bufLoc >= rand_buf+RAND_HASHWORD ?
		rand_bufLoc-RAND_HASHWORD
		: rand_bufLoc+(RAND_BUFSIZE-RAND_HASHWORD));
	f = rand_flips[rand_flipper++ & RAND_FLIPMASK];
	return(*rand_bufLoc ^ (f & RAND_MASK));
}

/* End of random number package
 *******************************/

/* Exit if running at bad time.
 */
#include <sys/time.h>
static void CheckTime()
{
	struct timeval now;
	struct tm *t;

	gettimeofday(&now, 0);
	t = localtime(&now.tv_sec);
	if (t->tm_hour >= 4) {
		printf("\n\n *** EXITING: BAD TIME ***\n\n");
		fflush(stdout);
		exit (1);
	}
}

static Position orig;	/* special in that only tableau[] is set */

/* Read original position from stdin into orig.
 */
static void ReadOriginal(filename) char *filename;
{
	int count = 0;
	Card cd = 0;
	FILE *f = stdin;

	if (filename != NULL && (f = fopen(filename, "r")) == NULL) {
		printf("free_cell: can't open %s\n", filename);
		exit(1);
	}
	memset(&orig, 0, sizeof(orig));
	while (count < 104) {
		int c = getc(f);
		if (c == EOF) {
			printf("Insufficient input.\n");
			exit(1);
		}
		if (c >= '2' && c <= '9') cd += c - '0';
		else if ((c |= 0x20) == 'a') cd += 1;
		else if (c == 't') cd += 10;
		else if (c == 'j') cd += 11;
		else if (c == 'q') cd += 12;
		else if (c == 'k') cd += 13;
		else if (c == 'c') cd += CLUBS;
		else if (c == 'd') cd += DIAMONDS;
		else if (c == 'h') cd += HEARTS;
		else if (c == 's') cd += SPADES;
		else continue;
		if (count++ % 2 != 0) {
			Column *col = &orig.tableau[((count-1)>>1)%8];
			col->cards[col->count++] = cd;
			cd = 0;
		}
	}
	if (filename != NULL) fclose(f);
}

/* Build original as simple sorted deck.
 */
static void BuildOriginal()
{
	int i;

	memset(&orig, 0, sizeof(orig));
	for (i=0; i<52; i++) {
		Column *col = &orig.tableau[i%8];
		Card cd = (i>>2)+1 + ((i&3)<<4);
		col->cards[col->count++] = cd;
	}
}

/* Shuffle the starting positions in orig.
 */
static Shuffle()
{
	int i = 52;
	
	while (i > 1) {
		int swap = Rand() % (i--);
		Card *c1 = &orig.tableau[i%8].cards[i/8];
		Card *c2 = &orig.tableau[swap%8].cards[swap/8];
		Card temp = *c1;
		*c1 = *c2;
		*c2 = temp;
	}
}

/* Cache length of sequence at bottom of a column.
 */
static int SeqLength(col) Column *col;
{
	int i = col->count - 1;

	if (i < 0)
		col->seq = 0;
	else {
		Card cd = Either(col->cards[i]);
		while (i-- > 0) {
			Card above = Either(col->cards[i]);
			if (above != CardAbove(cd)) break;
			cd = above;
		}
		col->seq = col->count - 1 - i;
	}
}

/* Initialise a Position record based on orig.
 */
static InitialPosition(pos, permute) Position *pos; int permute;
{
	int i, order[8];

	for (i=0; i<52; i++) {
		cardToIndex[((i/13)<<4)+(i%13)+1] = i;
		pos->location[i] = 0xFF;
	}
	for (i=0; i<8; i++) order[i] = i;
	if (permute) {
		int weight[8];
		memset(weight, 0, sizeof(weight));
		for (i=0; i<52; i++) {
			int col = (i % 8), index = (i / 8);
			Card c = orig.tableau[col].cards[index];
			weight[col] += (1 << (13-Rank(c))) *
					((col<5? 6 : 5) - index);
		}
		for (i=0; i<8; i++) weight[i] = weight[i]*8 + i;
		for (i=0; i<8; i++) {
			int j;
			order[i] = 0;
			for (j=0; j<8; j++) if (weight[j] > weight[i])
				order[i]++;
		}
	}
	for (i=0; i<52; i++) {
		int col = (i % 8), index = (i / 8);
		Card c = orig.tableau[col].cards[index];
		int x = cardToIndex[c];
		if (pos->location[x] != 0xFF) {
			printf("Card %02x appears twice!\n", c);
			exit(1);
		}
		col = order[col];
		pos->tableau[col].cards[index] = c;
		pos->tableau[col].count++;
		pos->location[x] = (col<<5) + index;
	}
	for (i=0; i<8; i++) {
		pos->colSeq[i] = i;
		SeqLength(&pos->tableau[i]);
	}
	pos->swappable = pos->swapped = 0;
}

/* Allocate a new tree_entry structure.  Allocates many at a time to reduce
 * malloc overhead.  Saves linked list of blocks of structs to enable
 * freeing the storage, which can be necessary if we're examining hundreds
 * of opening positions in a single run of the program.
 */
#define TREE_BLOCK 2000
typedef struct tree_block {
	TreeEntry block[TREE_BLOCK];
	struct tree_block *link;
} Block;

static int tree_avail = 0;
static Block *block = NULL;

static TreeEntry *NewTreeEntry()
{
	if (tree_avail-- == 0) {
		Block *more = (Block *) calloc(1, sizeof(Block));
		if (more == NULL) {
			printf("\nERROR: Out of memory!\n");
			exit(2);
		}
		more->link = block;
		block = more;
		tree_avail = TREE_BLOCK - 1;
	}
	return (block->block + tree_avail);
}

/* Show a card; rank in uppercase, suit in lowercase, trailing space.
 */
ShowCard(card) Card card;
{
	static char *rank = "?A23456789TJQK", *suit = "cdsh";
	
	if (card == 0) printf(".. ");
	else printf("%c%c ", rank[Rank(card)], suit[Suit(card)]);
}

/* Show the topmost card on a foundation pile.
 */
ShowFoundation(pos, s) Position *pos; int s;
{
	uchar r = pos->foundations[s];
	if (r == 0) printf("   "); else ShowCard((s<<4)+r);
}

/* Show a position as a rectangular layout.
 */
void ReadablePosition(pos) Position *pos;
{
	int row=0, col;

	for (col=0; col<4; col++) ShowCard(pos->hold[col]);
	for (col=0; col<4; col++) ShowFoundation(pos, col);
	if (pos->dontMoveTo || pos->dontMoveFrom)
		printf(" [!to 0x%02x, !from 0x%02x]",
		       pos->dontMoveTo, pos->dontMoveFrom);
	if (pos->swappable) printf(" SWAP: 0x%08x", pos->swappable);
	printf("\n");
	while (true) {
		int none = true;
		for (col=0; col<8; col++) {
			Card cd = pos->tableau[col].cards[row];
			if (cd) {ShowCard(cd); none = false;}
			else printf("   ");
		}
		printf("\n");
		if (none) break;
		row++;
	}
	fflush(stdout);
}

static void ShowMoveTypes()
{
	int k;

	printf("%12s%12s\n", "total", "distinct");
	for (k=0; moveDescs[k]; k++)
		printf("%12d%12d  %s\n", moveTypes[k], moveTypesNew[k],
		       moveDescs[k]);
}

static int CanSwap(swapMap, card) long swapMap; Card card;
{
	return (BitSet(swapMap, Either(card)));
}

#define HASH_SIZE 19001
static TreeEntry **tree;

/* Return column in sorted order, where columns that are headed by
 * colormates are subsorted based on the cards at the bottom.
 */
static Column *CarefullySortedCol(pos, i) Position *pos; int i;
{
	Column *col = SortedCol(pos, i);
	Column *other;
	if (col->afterMate) {
		other = SortedCol(pos, i-1);
		if (Either(col->cards[col->count-1])
		    < Either(other->cards[other->count-1])) return(other);
		return(col);
	}
	if (i == 7) return(col);
	other = SortedCol(pos, i+1);
	if (other->afterMate) {
		if (Either(other->cards[other->count-1])
		    < Either(col->cards[col->count-1])) return(other);
	}
	return(col);
}

/* Dump position and tree entry it's being compared against.
 */
static void DumpStuff(prev, pos) TreeEntry *prev; Position *pos;
{
	int i;

	printf("\nTree:\t!to/from = %08x/%08x\n\tother[] =", prev->dontMoveTo,
	       prev->dontMoveFrom);
	for (i=0; i<4; i++) printf(" %02x", prev->other[i]);
	printf("\n\tcards[] =");
	for (i=0; i<60; i++) printf(" %02x", prev->cards[i]);
	printf("\nPos:\t!to/from = %08x/%08x\n\tother[] =", pos->dontMoveTo,
	       pos->dontMoveFrom);
	for (i=0; i<4; i++) printf(" %02x", (trySwapping? pos->foundations[i]
					     : pos->hold[i]));
	printf("\n\tcol order =");
	for (i=0; i<8; i++)
		printf(" %d", (trySwapping? CarefullySortedCol(pos, i)
			       : SortedCol(pos, i)) - &pos->tableau[0]);
	printf("\n\ttableau[] =\n");
	for (i=0; i<8; i++) {
		int j;
		Column *col = &pos->tableau[i];
		printf("\t\t%02x:", col->count);
		for (j=0; j<col->count; j++) printf(" %02x", col->cards[j]);
		printf("\n");
	}
	printf("\n");
}

/* Compare current position to one stored in the hash table.
 */
static int BeenThere(prev, pos) TreeEntry *prev; Position *pos;
{
	uchar *c;
	int i, j;

	if (verbosity >= 5) DumpStuff(prev, pos);
	if ((prev->dontMoveTo & (0xFF ^ pos->dontMoveTo)) != 0
	    || (prev->dontMoveFrom & (0xFF ^ pos->dontMoveFrom)) != 0)
		return(false);
	if (trySwapping) {
		int exact = true;
		if (memcmp(prev->other, pos->foundations, 4*sizeof(uchar)) != 0)
			return(false);
		c = prev->cards;
		for (i=0; i<8; i++) {
			Column *col = CarefullySortedCol(pos, i);
			if (*c++ != col->count) return(false);
			for (j=0; j<col->count; c++, j++) {
				if (*c == col->cards[j]) continue;
				if (Either(*c) != Either(col->cards[j])
				    || !CanSwap(prev->swappable, *c))
					return(false);
				exact = false;
			}
		}
		if (!exact) matchedSwap++;
	}
	else {
		if (memcmp(prev->other, pos->hold, 4*sizeof(uchar)) != 0)
			return(false);
		c = prev->cards;
		for (i=0; i<8; i++) {
			Column *col = SortedCol(pos, i);
			if (*c++ != col->count) break;
			if (memcmp(c, col->cards, col->count*sizeof(Card)) != 0)
				return(false);
			c += col->count;
		}
	}
	return(true);
}

/* See if this position is already in the search tree.  If so, return false.
 * Else add it and return true.  In either case, set pos->tree.
 */
static int AddToTree(pos) Position *pos;
{
	long hash = 0, hashMod;
	int i;
	uchar *c;
	TreeEntry *probe;

	if (lateNight && (generated & 0xFFF) == 0) CheckTime();
	if (trySwapping) {
		for (i=0; i<4; i++) hash += (1 << Either(pos->hold[i]));
		hash %= 1381;	/* leaves room to be multiplied by 5^8 */
		for (i=0; i<8; i++) {
			Column *col = SortedCol(pos, i);
			int ct = col->count;
			if (ct == 0 || !col->afterMate) hash *= 5;
			if (ct) hash += Either(col->cards[ct-1]) + 7*ct;
		}
	}
	else {
		for (i=0; i<4; i++) hash = hash*3 + pos->hold[i];
		for (i=0; i<8; i++) {
			Column *col = SortedCol(pos, i);
			int ct = col->count;
			hash *= 5;
			if (ct) hash += col->cards[ct-1] + 7*ct;
		}
	}
	if (verbosity >= 5) printf("hash=%08x\n", hash);
	hashMod = hash % HASH_SIZE;
	probe = tree[hashMod];
	if (probe == NULL) hashes++;
	else while (probe != NULL) {
		if (probe->hash == hash && BeenThere(probe, pos)) {
			pos->tree = probe;
			return(false);
		}
		probe = probe->chain;
	}
	probe = pos->tree = NewTreeEntry();
	probe->chain = tree[hashMod];
	tree[hashMod] = probe;
	probe->hash = hash;
	if (trySwapping)
		memcpy(probe->other, pos->foundations, 4*sizeof(uchar));
	else
		memcpy(probe->other, pos->hold, 4*sizeof(uchar));
	probe->dontMoveFrom = pos->dontMoveFrom;
	probe->dontMoveTo = pos->dontMoveTo;
	probe->swappable = pos->swappable;
	c = probe->cards;
	for (i=0; i<8; i++) {
		Column *col = (trySwapping? CarefullySortedCol(pos, i)
			       : SortedCol(pos, i));
		*c++ = col->count;
		memcpy(c, col->cards, col->count*sizeof(Card));
		c += col->count;
	}
	if ((distinct++ & 0x7FFF) == 0x7FFF && verbosity == 1) {
		printf("%d distinct positions out of %d...\n",
		       distinct, generated);
		fflush(stdout);
	}
	return(true);
}

/* Return true if tableau has an empty column.
 */
static int HasSpace(pos) Position *pos;
{
	return(SortedCol(pos, 7)->count == 0);
}

/* Return true if card is at the bottom of a column.
 */
static int AtBottom(pos, card) Position *pos; Card card;
{
	Loc where;

	if (card == 0) return(false);
	where = Where(pos, card);
	if (Index(where) == 0x1F) return(false);
	if (pos->tableau[Col(where)].count == Index(where)+1) return(true);
	return(false);
}

/* Return true if card is available to be played to a foundation.
 */
static int Available(pos, card) Position *pos; Card card;
{
	return(AtBottom(pos, card) || Where(pos, card) == HOLDING);
}

/* Return true if dontMoveTo/From not violated.
 */
static int OkayToMove(pos, card, whither) Position *pos; Card card; Loc whither;
{
	Loc where = Where(pos, card);

	if (where != HOLDING && BitSet(pos->dontMoveFrom, Col(where)))
		return(false);
	if (Index(whither) != 0x1F && BitSet(pos->dontMoveTo, Col(whither)))
		return(false);
	return(true);
}

/* Return true if given card fits on a card in the holding area. */
static int HoldContainsCardAbove(pos, card) Position *pos; Card card;
{
	Card above = CardAbove(card);
	return(Rank(card) < 13 && (Where(pos, above) == HOLDING
				   || Where(pos, above^0x20) == HOLDING));
}

/* Return how long a sequence can be moved using all free cells/spaces.
 */
static int MaxCanMove(pos) Position *pos;
{
	int i, j;

	for (i=3; i>=0 && pos->hold[i]==0; i--) {}
	for (j=7; j>=0 && SortedCol(pos, j)->count==0; j--) {}
	return ((3 - i + 1) << (7 - j));
}

/* Same as MaxCanMove but assumes N free cells will be moved into 1 space.
 */
static int CouldMove(pos, n) Position *pos; int n;
{
	int i, j;

	for (i=3; i>=0 && pos->hold[i]==0; i--) {}
	for (j=7; j>=0 && SortedCol(pos, j)->count==0; j--) {}
	return ((3 - i + 1 + n) << (7 - j - 1));
}

/* Sort the holding area into descending order (hence spaces come last).
 * Use homogeneous sorting network.
 */
static void SortHoldingArea(pos) Position *pos;
{
	Card *h = pos->hold;
	Card temp;

#define SWAP(i,j) if (h[i]<h[j]) {temp=h[i]; h[i]=h[j]; h[j]=temp;}

	SWAP(0,1)
	SWAP(2,3)
	SWAP(0,2)
	SWAP(1,3)
	SWAP(1,2)
}

/* Sort the tableau columns based on the top cards of the columns.
 * Only called when the top card of a column actually changes.  Note
 * that the columns will be mostly sorted already.  Color-equivalent
 * top cards are sorted adjacent to aid hash function in AddToTree.
 */
static void SortColumns(pos) Position *pos;
{
	int i, thru = 7;

	for (i=0; i<8; i++) {
		/* generate value to sort on */
		Column *col = &pos->tableau[i];
		Card c = col->cards[0];
		col->afterMate = (Either(c) << 1) + (c >> 5);
	}
	while (thru > 0) {
		int i, need = 0;
		for (i=1; i<=thru; i++) {
			if (SortedCol(pos, i)->afterMate > 
			    SortedCol(pos, i-1)->afterMate) {
				uchar temp = pos->colSeq[i];
				pos->colSeq[i] = pos->colSeq[i-1];
				pos->colSeq[i-1] = temp;
				need = i-1;
			}
		}
		thru = need;
	}
	for (i=7; i>0; i--) SortedCol(pos, i)->afterMate =
		(((SortedCol(pos, i)->afterMate)
		  ^ (SortedCol(pos, i-1)->afterMate)) == 1);
	SortedCol(pos, 0)->afterMate = false;
}

/* Nuclear moves used by MoveCard. */

static void RemoveFromHold(pos, card) Position *pos; Card card;
{
	int i;

	for (i=0; pos->hold[i] != card; i++) {}
	pos->hold[i] = 0;
	SortHoldingArea(pos);
}

static void RemoveFromTableau(pos, card) Position *pos; Card card;
{
	Loc whence = Where(pos, card);
	Column *col = &pos->tableau[Col(whence)];
	int i;

	for (i=Index(whence); i<col->count; i++) col->cards[i] = 0;
	col->count = Index(whence);
	SeqLength(col);
	if (col->count == 0) SortColumns(pos);
}

static void MoveHoldToTableau(pos, card, whither)
	Position *pos; Card card; Loc whither;
{
	Column *col = &pos->tableau[Col(whither)];

	RemoveFromHold(pos, card);
	col->cards[Index(whither)] = card;
	if (col->count++ == 0) SortColumns(pos);
	col->seq++;
	Where(pos, card) = whither;
}

static void MoveToFoundation(pos, card) Position *pos; Card card;
{
	Loc *where = &Where(pos, card);
	if (*where == HOLDING) RemoveFromHold(pos, card);
	else RemoveFromTableau(pos, card);
	pos->foundations[Suit(card)]++;
	*where = FOUNDATION;
}

static void MoveTableauToHold(pos, card) Position *pos; Card card;
{
	Loc whence = Where(pos, card);
	Column *col = &pos->tableau[Col(whence)];
	int i, j = 3;

	for (i=Index(whence); i<col->count; i++) {
		pos->hold[j--] = col->cards[i];
		Where(pos, col->cards[i]) = HOLDING;
		col->cards[i] = 0;
	}
	col->count = Index(whence);
	SeqLength(col);
	if (col->count == 0) SortColumns(pos);
	SortHoldingArea(pos);
}

static void MoveTableauToTableau(pos, card, whither)
	Position *pos; Card card; Loc whither;
{
	Loc whence = Where(pos, card);
	Column *col = &pos->tableau[Col(whence)];
	Column *dest = &pos->tableau[Col(whither)];
	Card *spot = &dest->cards[Index(whither)];
	int i;

	for (i=Index(whence); i<col->count; i++) {
		*spot++ = col->cards[i];
		Where(pos, col->cards[i]) = whither++;
		col->cards[i] = 0;
	}
	i = dest->count;
	dest->count = Index(whither);
	dest->seq += dest->count - i;
	col->count = Index(whence);
	SeqLength(col);
	if (col->count == 0 || i == 0) SortColumns(pos);
}

static void SwapSuits(pos, card) Position *pos; Card card;
{
	Loc where = Where(pos, card);
	if (where != HOLDING) {
		pos->tableau[Col(where)].cards[Index(where)] ^= 0x20;
		if (Index(where) == 0) SortColumns(pos);
	}
	else {
		/* assume only one of the pair is in the holding area
		   if we're actually being told to do the swap */
		int i;
		for (i=0; Either(pos->hold[i]) != Either(card); i++) {}
		pos->hold[i] ^= 0x20;
		SortHoldingArea(pos);
	}
}

/* Move the specified card from wherever it is to new location, moving
 * other cards as necessary to accomplish this.  May assume destination
 * of move is legal for card being moved.  If move is a partial sequence
 * into a space, set dontMoveTo.  If move is from holding area to space,
 * set dontMoveFrom.  If moving to/from a column, clear dontMoveFrom/To
 * for that column.
 */
static Loc MoveCard(pos, card, whither) Position *pos; Card card; Loc whither;
{
	Loc where = Where(pos, card);

	pos->how.moved = card;
	pos->how.whither = whither;

	if (Index(where) != 0x1F) pos->dontMoveTo &= (-1)^(1 << Col(where));

	if (whither == FOUNDATION) {
		MoveToFoundation(pos, card);
	}
	else if (whither == HOLDING) {
		MoveTableauToHold(pos, card);
	}
	else if (whither == MULTIPLE) {
		int canMove = MaxCanMove(pos);
		Column *col = &pos->tableau[Col(where)];
		int needToMove = col->count - Index(where);
		while (needToMove > 0) {
			if (HasSpace(pos)) {
				int moving = needToMove;
				if (moving > (canMove>>1)) moving = canMove>>1;
				MoveTableauToTableau(pos,
				   col->cards[col->count - moving],
				   SpaceLoc(pos));
				canMove >>= 1;
				needToMove -= moving;
			}
			else {
				MoveTableauToHold(pos, card);
				break;
			}
		}
		/* moves to MULTIPLE always leave at least one card */
		if (Either(col->cards[Index(where)-1]) == Either(CardAbove(card)))
			pos->dontMoveTo |= (1 << Col(where));
	}
	else if (where == HOLDING) {
		MoveHoldToTableau(pos, card, whither);
		pos->dontMoveFrom |= (1 << Col(whither));
	}
	else {
		MoveTableauToTableau(pos, card, whither);
		if (Index(where) > 0 && Either(CardAbove(card)) ==
		    Either(pos->tableau[Col(where)].cards[Index(where)-1])) {
			if (Index(whither) == 0
			    /* these make it worse????
			       || CanSwap(pos, card)
			       || Where(pos, card^0x20) == FOUNDATION */
			       ) pos->dontMoveTo |= (1 << Col(where));
		}
		pos->dontMoveFrom &= (-1)^(1 << Col(whither));
	}
}

/* Part of canonicalisation (see below).  Make a forced move to a foundation
 * if possible; return true if such a move is made.  If no move made, compute
 * goodcols as bitmask of columns containing potential forced moves.
 */
static int ForceToFoundation(pos) Position *pos;
{
	int i, j;
	Card moved;

	pos->goodcols = 0;
	for (i=0; i<4; i++) {
		int r = pos->foundations[i]+1;	/* next rank to go here */
		j = pos->foundations[i^1];
		if (pos->foundations[i^3] < j) j = pos->foundations[i^3];
		if (r > 13 || j+2 < r ||
		    (j+1 < r && pos->foundations[i^2]+3 < r)) continue;
		/* move is forced if card is available */
		moved = (i<<4) + r;
		/* for forced moves, if the card is swappable, then
		   if either is available they both are, so no swap */
		if (Available(pos, moved)) {
			if (OkayToMove(pos, moved, FOUNDATION)) {
				MoveCard(pos, moved, FOUNDATION);
				return(true);
			}
			else {
				pos->goodcols = 0;
				return(false);
			}
		}
		/* card not available, but note good column to dig in */
		pos->goodcols |= (1 << Col(Where(pos, moved)));
	}
	return(false);
}

/* Reduce a position to canonical form by making forced moves to foundations
 * and then moving cards down from free cells to non-empty tableau columns
 * (if a choice, pick one).  Don't move onto a column marked as dontMoveTo.
 */
static void Canonical(pos) Position *pos;
{
	int i, j;
	Card moved;
	Move how = pos->how;

	while (ForceToFoundation(pos)) {}

	for (i=0; i<4; i++) {
		Card held = pos->hold[i];
		Card onto;
		if (held == 0) return;
		if (Rank(held) == 13) continue;
		onto = CardAbove(held);
		for (j=0; j<2; j++) {
			Loc where = Where(pos, onto);
			if (AtBottom(pos, onto) &&
			    BitOff(pos->dontMoveTo, Col(where))) {
				MoveHoldToTableau(pos, held, where+1);
				pos->dontMoveFrom &= (-1)^(1 << Col(where));
				i = (-1);
				break;
			}
			onto ^= 0x20;
		}
	}

	pos->how = how;
}

static depth = 0;

static ShowMove(pos, tag) Position *pos; char *tag;
{
	Card moved = pos->how.moved;
	Loc whither = pos->how.whither;

	printf("%4d: ", depth);
	if (CanSwap(pos->swappable & pos->swapped, moved)) moved ^= 0x20;
	ShowCard(moved);
	printf("-> ");
	if (whither == HOLDING) printf("free cell(s)");
	else if (whither == FOUNDATION) printf("foundation");
	else if (whither != MULTIPLE) printf("tableau %d", Col(whither)+1);
	else printf("multiple spaces/free cells");
	if (tag) printf(" (%s)", tag);
	printf(" [%08x]\n", pos->tree);
	if (verbosity > 2) fflush(stdout);
}

/* Show the winning path, in reverse order (because it's convenient).
 */
static void ShowPath(pos) Position *pos;
{
	Position *p = pos;

	printf("Winning path (in reverse order):\n");
	while (p->via) {
		int swap = p->swappable & (p->swapped = pos->swapped);
		ShowMove(p, moveDescs[p->how.whatType]);
		depth--;
		p = p->via;
		swap &= (-1) ^ p->swappable;
		while (swap) {
			Card c;
			for (c=1; BitOff(swap, c); c++) {}
			swap ^= (1 << c);
			printf("%4d: swap ", depth); ShowCard(c);
			printf("with "); ShowCard(c ^ 0x20);
			printf("\n");
		}
	}
	printf("\n");
}

static int maxdepth = 0;

#define WINNABLE    2
#define INTRACTABLE 1
#define IMPOSSIBLE  0

/* Make (and if verbose, print) a move, then recursively call DFS if the
 * resulting position is new.
 */
static void DFS(); /* forward decl */
static void TryMove(via, card, whither, whatType)
     Position *via; Card card; Loc whither; int whatType;
{
	int dup = false, atEntry;
	Position *pos, *temp;

	depth++;
	if (via->deeper == NULL) {
		via->deeper = (Position *)malloc(sizeof(Position));
		via->deeper->deeper = NULL;
	}
	pos = via->deeper;

	temp = pos->deeper;
	*pos = *via;
	pos->deeper = temp;

	pos->via = via;
	if (whither == SWAP_FOUND) {
		Loc temp = Where(pos, card);
		Where(pos, card) = Where(pos, card^0x20);
		Where(pos, card^0x20) = temp;
		SwapSuits(pos, card);
		SwapSuits(pos, card^0x20);
		pos->swapped |= (1 << Either(card));
		card ^= 0x20;
		whither = FOUNDATION;
	}
	if (whither == FOUNDATION)
		pos->swappable &= (-1) ^ (1 << Either(card));
	if (whither == FOUNDATION && !Available(pos, card)) {
		/* digging to foundation-playable card, which must then move */
		Loc where = Where(pos, card);
		MoveCard(pos, pos->tableau[Col(where)].cards[Index(where)+1],
			 (AtBottom(pos, card^0x20)? (Where(pos, card^0x20)+1)
			  : MULTIPLE));
		Canonical(pos);
		if (verbosity >= 2) ShowMove(pos, "forcing");
		TryMove(pos, card, whither, whatType);
		depth--;
		return;
	}
	MoveCard(pos, card, whither);
	Canonical(pos);
	if (pos->goodcols == 0) {
		/* check for victory */
		if (pos->foundations[0] == 13 && pos->foundations[1] == 13
		    && pos->foundations[2] == 13 && pos->foundations[3] == 13) {
			windepth = depth;
			if (show) ShowPath(pos);
			longjmp(longjmp_env, WINNABLE);
		}
		else /* a forced move was disallowed, prune this branch */
			return;
	}

	if (Index(whither) != 0x1F) {
		/* moved to col; check for repetition */
		int i = Col(whither);
		Column *col = &pos->tableau[i];
		Position *prev;
		for (prev=via; prev; prev=prev->via) {
			Column *oldcol = &prev->tableau[i];
			Card *c = col->cards, *oldc = oldcol->cards;
			int j;
			if (oldcol->count == 0) continue;
			if (Either(oldc[0]) != Either(c[0])) break;
			if (oldc[0] != c[0] && !CanSwap(prev->swappable, c[0]))
				break;
			if (oldcol->count != col->count) continue;
			for (j=col->count-1; j>0; j--) {
				if (Either(oldc[j]) != Either(c[j])) break;
				if (oldc[j] != c[j] &&
				    !CanSwap(prev->swappable, c[j])) break;
			}
			if (j == 0) {
				dup = true;
				pos->tree = prev->tree;
				break;
			}
		}
	}
	generated++;
	moveTypes[whatType]++;
	if (dup) {
		if (verbosity >= 2) ShowMove(pos, "rejected");
		rejected++;
	}
	else if (AddToTree(pos)) {
		moveTypesNew[whatType]++;
		pos->how.whatType = whatType;
		if (verbosity >= 2) {
			ShowMove(pos, NULL);
			if (verbosity >= 4) {
				if (verbosity >= 5 || maxdepth++ % 100 == 0) { 
					ReadablePosition(pos);
				}
			}
		}
		atEntry = generated;
		DFS(pos);
		if (deadEnds > 0 && generated-atEntry >= deadEnds) {
			if (verbosity < 2) ShowMove(pos, NULL);
			printf("Dead end of %d positions from:\n",
			       generated - atEntry);
			ReadablePosition(via);
			printf("Move tallies (total, not just subtree):\n");
			ShowMoveTypes();
		}
	}
	else if (verbosity >= 2) ShowMove(pos, "old");
	depth--;
}

/* Note a pair that can be swapped.
 */
static void PotentialSwap(pos, card) Position *pos; Card card;
{
	if (verbosity > 1) {
		printf("%4d: can swap ", depth); ShowCard(card);
		printf("& "); ShowCard(card^0x20);
		printf("\n");
		if (verbosity > 2) fflush(stdout);
	}
	pos->swappable |= (1 << Either(card));
}

/* Look for opportunities to swap pairs.
 */
static void LookForSwaps(pos, canMove) Position *pos; int canMove;
{
	int i, buriedBy;
	Card swap;
	Loc other;

	for (i=0; i<4; i++) {
		swap = pos->hold[i];
		if (swap == 0) return;
		if (CanSwap(pos->swappable, swap)) continue;
		other = Where(pos, swap^0x20);
		if (other == FOUNDATION) continue;
		if (other != HOLDING) {
			Column *col = &pos->tableau[Col(other)];
			buriedBy = col->count - Index(other) - 1;
			if (buriedBy < col->seq-1) {
				/* mate is in seq at bottom of column */
				if (buriedBy + 2 > canMove) continue;
			}
			else if (col->seq == col->count) {
				/* mate is at top of single-sequence column */
				if (col->seq+1 > canMove && (!HasSpace(pos)
				    || col->seq-1 > CouldMove(pos, 1)))
					continue;
			}
			else continue;
		}
		PotentialSwap(pos, swap);
	}
	for (i=0; i<8; i++) {
		Column *col = &pos->tableau[i];
		Column *otherCol;
		if (col->count == 0) continue;
		swap = col->cards[col->count - 1];
		if (CanSwap(pos->swappable, swap)) continue;
		other = Where(pos, swap^0x20);
		if (Index(other) == 0x1F) continue;
		otherCol = &pos->tableau[Col(other)];
		buriedBy = otherCol->count - Index(other) - 1;
		if (buriedBy < otherCol->seq-1) {
			/* mate is in seq at bottom of column */
			if (buriedBy + 2 > canMove) continue;
		}
		else if (otherCol->seq == otherCol->count) {
			/* mate is at top of single-sequence column */
			if (col->count != 1) buriedBy += 2;
			if (buriedBy > canMove) continue;
		}
		else continue;
		PotentialSwap(pos, swap);
	}
}

/* Depth-first search.  Generate all positions reachable from "via".  (If
 * there is a forced move, just do the forced move.)  
 * For positions that haven't already been reached, add them and call DFS
 * recursively via TryMove.  Return true if solution found.
 */
static void DFS(via) Position *via;
{
	int i, j, intab = 0, colsIter; Card moved, onto;
	int canMove;	/* how long a sequence can be moved */

	if (distinct >= tractLimit) longjmp(longjmp_env, INTRACTABLE);

	/* check for forced moves to foundation */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		j = via->foundations[i^1];
		if (via->foundations[i^3] < j) j = via->foundations[i^3];
		if (r > 13 || j+2 < r ||
		    (j+1 < r && via->foundations[i^2]+3 < r)) continue;
		/* move is forced if card is available */
		moved = (i<<4) + r;
		if (Available(via, moved)) {
			/* for forced moves, if the card is swappable, then
			   if either is available they both are, so no swap */
			printf("huh?\n");
			if (OkayToMove(via, moved, FOUNDATION))
				TryMove(via, moved, FOUNDATION, MV_FFORCE);
			return;
		}
	}

	canMove = MaxCanMove(via);

	/* if can move an entire column onto another, and not prohibited by
	   dontMove*, do it (forced) */
	if (forceCombining) {
		for (i=0; i<8; i++) if (BitOff(via->dontMoveFrom, i)) {
			Column *col = &via->tableau[i];
			int held = 0;
			if (col->seq < col->count || col->count == 0) continue;
			moved = col->cards[0];
			if (Rank(moved) >= 13) continue;
			onto = CardAbove(moved);
			if (col->seq <= canMove) for (j=0; j<2; j++) {
				Loc where = Where(via, onto);
				if (AtBottom(via, onto) &&
				    BitOff(via->dontMoveTo, Col(where))) {
					TryMove(via, moved, where+1, MV_TFORCE);
					return;
				}
				onto ^= 0x20;
			}
			while (HoldContainsCardAbove(via, moved)) {
				held++;
				moved = CardAbove(moved);
			}
			if (held == 0) continue;
			if (CouldMove(via, held) < col->seq) continue;
			if (Where(via, moved) != HOLDING) moved ^= 0x20;
			TryMove(via, moved, SpaceLoc(via), MV_SFORCE);
			/* canonicalisation & recursive force does the rest */
			return;	/* if that didn't work, back up */
		}
	}

	for (i=0; i<8; i++)
		if (via->tableau[i].count < 3) via->goodcols |= (1 << i);

	if (trySwapping) LookForSwaps(via, canMove);

	/* try each type of move first for goodcols, then others */

	/* try moving maximal sequences onto other non-empty columns */
	for (colsIter=0; colsIter<2; colsIter++) {
	for (i=0; i<8; i++) if (BitSet(via->goodcols, i)) {
		Column *col = &via->tableau[i];
		if (col->seq == 0 || col->seq > canMove) continue;
		moved = col->cards[col->count - col->seq];
		if (Rank(moved) == 13) continue;
		onto = CardAbove(moved);
		for (j=0; j<2; j++) {
			Loc where = Where(via, onto) + 1;
			if (AtBottom(via, onto) && OkayToMove(via, moved, where)) {
				TryMove(via, moved, where, MV_TFULL);
				intab |= (1 << i);
			}
			onto ^= 0x20;
		}
	}
	via->goodcols ^= 0xFF;
	}

	/* try moving otherwise unmovable maximal sequences to spaces */
	for (colsIter=0; colsIter<2; colsIter++) {
	if (HasSpace(via)) for (i=0; i<8; i++)
	  if (BitOff(intab|via->dontMoveFrom, i) && BitSet(via->goodcols, i)) {
		Column *col = &via->tableau[i];
		if (col->seq == col->count || col->seq > (canMove>>1)) continue;
		moved = col->cards[col->count - col->seq];
		if (HoldContainsCardAbove(via, moved)) continue;
		TryMove(via, moved, SpaceLoc(via), MV_SFULL);
	}
	via->goodcols ^= 0xFF;
	}

	/* if no space, try moving otherwise unmovable maximal sequences
	   into the holding area, if they fit completely */
	for (colsIter=0; colsIter<2; colsIter++) {
	if (!HasSpace(via)) for (i=0; i<8; i++)
	  if (BitOff(intab|via->dontMoveFrom, i) && BitSet(via->goodcols, i)) {
		Column *col = &via->tableau[i];
		if (col->seq == 0 || col->seq > canMove-1) continue;
		moved = col->cards[col->count - col->seq];
		TryMove(via, moved, HOLDING, MV_HFULL);
	}
	via->goodcols ^= 0xFF;
	}

	/* try moving part of a sequence to another non-empty column */
	if (partials) for (colsIter=0; colsIter<2; colsIter++) {
	for (i=0; i<8; i++) {
		Column *col = &via->tableau[i];
		if (col->count == 0) continue;
		moved = CardBelow(col->cards[col->count-1]);
		for (j=0; j<2; j++) {
			Loc where = Where(via, moved);
			Column *srcCol = &via->tableau[Col(where)];
			if (Index(where) != 0x1F
			    && srcCol->count - Index(where) < srcCol->seq
			    && srcCol->count - Index(where) <= canMove
			    && OkayToMove(via, moved, (i<<5) + col->count)
			    && BitSet(via->goodcols, Col(where))) {
				intab |= (1 << Col(where));
				TryMove(via, moved, (i<<5) + col->count,
					MV_TPART);
			}
			moved ^= 0x20;
		}
	}
	via->goodcols ^= 0xFF;
	}

	/* try moving from holding area to empty column; vary which one is
	   tried first, in case the key move we're looking for is hold[3];
	   unless desperate (tbd), don't move down a card if another free
	   cell has a card this one could move onto (move that one instead) */
	if (HasSpace(via)) for (i=0; i<4; i++) {
		Card below;
		moved = via->hold[(i+(depth>>2))&3];
		if (moved == 0) continue;
		/* don't move a card down unless there's a card that could
		   eventually move onto it */
		below = CardBelow(moved);
		if (Where(via, below) == FOUNDATION
		    && Where(via, below^0x20) == FOUNDATION) continue;
		/* don't move down if it could move onto other free card */
		if (HoldContainsCardAbove(via, moved)) continue;
		TryMove(via, moved, SpaceLoc(via), MV_THOLD);
	}

	/* try part of a sequence into a space, if there is another space
	   and a free cell card that (if moved into the other space) could
	   accept part of the remaining sequence */
	if (partials && SortedCol(via, 6)->count == 0) for (i=0; i<4; i++) {
		Card below;
		if (via->hold[i] == 0) break;
		below = CardBelow(via->hold[i]);
		for (j=0; j<2; j++) {
			Loc where = Where(via, below);
			if (Index(where) != 0x1F && BitOff(intab, Col(where))) {
				Column *col = &via->tableau[Col(where)];
				int buriedBy = col->count - Index(where) - 1;
				if (buriedBy < (canMove>>1) ||
				    buriedBy >= col->seq) continue;
				TryMove(via,
					col->cards[col->count-(canMove>>1)],
					SpaceLoc(via), MV_SPART);
			}
			below ^= 0x20;
		}
	}

	/* for maximal sequences that couldn't be moved to single columns
	   (empty or otherwise), try moving them to a combination of
	   multiple spaces and/or free cells */
	if (HasSpace(via)) for (i=0; i<8; i++)
	  if (BitOff(intab|via->dontMoveFrom, i)) {
		Column *col = &via->tableau[i];
		if (col->seq <= (canMove>>1) || col->seq > canMove-1) continue;
		if (col->seq == col->count) continue;
		moved = col->cards[col->count - col->seq];
		if (HoldContainsCardAbove(via, moved)) continue;
		TryMove(via, moved, MULTIPLE, MV_MFULL);
	}

	/* try non-forced moves to foundation, first for cards that are
	   actually available to move already */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		if (r > 13) continue;
		moved = (i<<4) + r;
		if (Available(via, moved) && OkayToMove(via, moved, FOUNDATION)) {
			TryMove(via, moved, FOUNDATION, MV_FOPT);
		}
		moved ^= 0x20;
		if (CanSwap(via->swappable, moved) && Available(via, moved)
		    && OkayToMove(via, moved, FOUNDATION)) {
			TryMove(via, moved, SWAP_FOUND, MV_FOPT);
		}
	}

	/* finally, try digging to reach foundation-playable cards that
	   are covered by cards in sequence */
	for (i=0; i<4; i++) {
		int r = via->foundations[i]+1;	/* next rank to go here */
		if (r > 13) continue;
		moved = ((i<<4) + r) ^ 0x20;
		for (j=0; j<(CanSwap(via->swappable, moved)? 2 : 1); j++) {
			Loc where;
			moved ^= 0x20;
			where = Where(via, moved);
			if (where != HOLDING &&
			    OkayToMove(via, moved, MULTIPLE)) {
				Column *col = &via->tableau[Col(where)];
				int buriedBy = col->count - Index(where) - 1;
				if (buriedBy == 0) continue;
				if (buriedBy >= col->seq - 1) continue;
				onto = moved ^ 0x20;
				if (AtBottom(via, onto)?
				    (!partials && buriedBy <= canMove) :
				    (buriedBy <= canMove-1))
					TryMove(via, moved,
						(j? SWAP_FOUND : FOUNDATION),
						MV_FDIG);
			}
		}
	}
}

static int Search(permuted) int permuted;
{
	int success;
	Position *pos1;
	static Position *pos0 = NULL;

	if (pos0 == NULL) pos0 = (Position *)malloc(sizeof(Position));
	pos1 = pos0->deeper;
	memset(pos0, 0, sizeof(Position));
	pos0->deeper = pos1;

	InitialPosition(pos0, permuted);
	if (showorig) ReadablePosition(pos0);
	SortColumns(pos0);
	Canonical(pos0);
	generated++;
	(void) AddToTree(pos0);

	success = setjmp(longjmp_env);
	if (success == IMPOSSIBLE /* i.e., 0 */) DFS(pos0);

	if (success == WINNABLE)
		printf("Winnable.\n");
	else {
		if (!showorig) ReadablePosition(pos0);
		printf(success==INTRACTABLE? "Intractable.\n"
		       : partials? "Impossible.\n"
		       : "Impossible without partials.\n");
	}

	if (verbosity > 0) {
		printf("\n");
		if (success == WINNABLE)
			printf("Winning line of %d steps.\n", windepth);
		printf("\
Generated %d positions (%d distinct).\n\
Rejected %d via color-swapping.\n\
Rejected %d due to repeated columns.\n",
		       generated, distinct, matchedSwap, rejected);
		printf("Used %d of %d hash table slots.\n",
		       hashes, HASH_SIZE);
		printf("Move-type tallies:\n");
		ShowMoveTypes();
	}

	/* clean up to prepare for next position */
	generated = distinct = matchedSwap = rejected = 0;
	hashes = depth = windepth = 0;
	memset(tree, 0, HASH_SIZE * sizeof(Position*));
	while (block->link != NULL) {
		Block *link = block->link;
		free(block);
		block = link;
	}
	tree_avail = TREE_BLOCK;
	memset(block, 0, sizeof(*block));
	fflush(stdout);

	if (success == IMPOSSIBLE && !partials) {
		partials = true;
		success = Search(permuted);
		partials = false;
	}

	return(success);
}


main(argc, argv) int argc; char *argv[];
{
	Position *pos0;
	int i, reps = 1, make_random = false, longSearch = false;
	char *filename = NULL;

	for (i=1; i<argc; i++) {
		char *arg = argv[i];
		if (*arg == '-') {
			while (*++arg) switch (*arg) {
			case 'c':
				trySwapping = false;
				break;
			case 'd':
				deadEnds = 100000;
				break;
			case 'f':
				forceCombining = false;
				break;
			case 'i':
				showorig = true;
				break;
			case 'l':
				longSearch = true;
				break;
			case 'p':
				partials = false;
				break;
			case 'r':
				make_random = true;
				break;
			case 's':
				show = true;
				break;
			case 't':
				lateNight = true;
				CheckTime();
				break;
			case 'v':
				if (verbosity++ < 5) break;
				/* FALL THROUGH */
			default:
				printf("\
Usage: free_cell [-irsvv] [file]\n\n\
-i:  show initial layout\n\
-l:  just do a long search immediately\n\
-f:  do NOT force combining partial sequences when lower is entire column\n\
-c:  do NOT merge color-equivalent positions\n\
-p:  do NOT allow partial-sequence moves except to reach foundation moves\n\
-d:  show positions leading to dead ends of >= 100000 positions\n\
-r:  if no 'file' arg, generate random winnable position\n\
     else convert filename to int, generate and test that many positions\n\
-s:  show solution if found (else just report whether one exists)\n\
-t:  only run between 12:00am and 3:59am\n\
-v:  verbose; give some statistics when done\n\
-vv: very verbose; dump entire search tree as it is traversed\n\
-vvv: same as -vv but does fflush after each step\n\
-vvvv: and dump the position under programmed circumstances\n\
-vvvvv: and dump details of all tree/pos comparisons\n");
				exit(1);
			}
		}
		else if (filename == NULL) filename = arg;
	}

	if (make_random) {
		BuildOriginal();
		if (filename != NULL) reps = atoi(filename);
		if (reps <= 0) reps = 1;
	}
	else
		ReadOriginal(filename);
	
	tree = (TreeEntry **) calloc(HASH_SIZE, sizeof(TreeEntry*));
	if (tree == NULL) {
		printf("\nERROR: Out of memory!\n");
		exit(2);
	}

	for (i=0; i<reps; i++) {
		if (make_random) Shuffle();
		for (tractLimit=2500; tractLimit<=1000000; tractLimit*=20) {
			if (longSearch) tractLimit = 1000000;
			if (tractLimit > 3000) {
				printf("\nTrying longer search (%d)...\n",
				       tractLimit);
				fflush(stdout);
			}
			if (Search(false) != INTRACTABLE) break;
			if (!longSearch) {
				printf("\nTrying permuting columns...\n");
				fflush(stdout);
				if (Search(true) != INTRACTABLE) break;
			}
		}
		if (tractLimit > 1000000) printf("\nGiving up.\n");
	}

	exit(0);
}
----------
X-Sun-Data-Type: c-file
X-Sun-Data-Description: c-file
X-Sun-Data-Name: sea_test.c
X-Sun-Charset: us-ascii
X-Sun-Content-Lines: 1023

/*
 * Program for testing winnability of opening positions in Seahaven Towers.
 * Can also generate random positions, optionally filtered for winnability.
 * Does not actually support interactive playing of the game.  See below for
 * a brief explanation of the rules.
 *
 * (c) Copyright 1993, Donald R. Woods.  Shareware: Right is granted to
 *	freely redistribute this source provided this notice is retained.
 */

/*****************************************************************************
Brief summary of the game:

Deal ten columns of five cards each, spreading each column so the cards are
all visible.  These columns form the "tableau".  The other two cards go into
a "holding area" which can hold a maximum of four cards.  The only cards that
can be moved are the bottommost card in each column and the cards in the
holding area.  As Aces become available (i.e., can be moved) they are moved
to start four "foundation" piles, which are then built up in suit to Kings.
The object is to move all the cards to the foundations.

A move consists of moving one card.  A card can move onto a tableau column
if the card being moved is the same suit and one rank lower than the card it
is being played onto (i.e., the tableau builds down in suit).  Any card can
be moved to an empty holding area spot.  ONLY A KING can be moved into an
empty tableau column.

A commonly seen variant is to play with eight columns, which start with six
cards each in half the columns and five each in the rest.  Moves to the
tableau build down but in alternating color (i.e. not in suit); moves to the
foundations are still by suit.  Any card can be moved to an empty column.

This program solves positions only for the 10-column version.


General approach: Depth-first search seems to be fast enough, so we won't try
anything fancier.

The search space is reduced by canonicalising each position so as to reduce
the number of different moves that need be considered.  Canonical form is
obtained by (a) making all possible moves to the foundations and (b) making
all possible non-King moves from the holding area to the tableau.  (We also
sort the cards in the holding area, and require that any tableau columns
that are either empty or contain only a king-headed sequence, are sorted by
suit, with the empty columns last.  We do NOT sort the rest of the tableau.)

Given this canonical form, the only moves that need be considered are:
 - maximal sequence from tableau to tableau, assuming N-1 openings
   exist in the holding area
 - maximal king-headed sequence from tableau to empty column, assuming
   N-1 openings exist in the holding area; if sequence is an entire
   column, do not bother trying this move
 - maximal sequence from tableau to holding area, assuming N openings
   exist; if sequence could also be moved within tableau (including
   king-headed seq to empty column), do not bother trying this move
 - king from holding area to empty tableau column

A move which will create a sequence longer than 5, is not made if it would
bury a smaller card in the same suit.

If it looks like it will help, we can consider having canonicalisation
include moving tableau-to-tableau to build down on king-topped columns once
all kings are at the tops of columns.  We could also include tab-to-tab moves
that extend sequences that are already longer than 5.  We could also detect
other "dead end" features, such as a sequence of length 5 that buries both a
lower card of the same suit AND the next higher card, e.g. 9-A-8-7-6-5-4.

A position is encoded in terms of the starting position.  For each non-empty
column, the column is headed either by one of the 4 kings or by 1-5 of the
cards originally in that column.  The king, or bottom of the 1-5 cards, may
then have 0-11 additional cards on it.  (We'll never build an ace into the
tableau.)  Thus for each column we give:
	1 bit:  1 if headed by king, 0 if empty or headed by an original card
	3 bits: 0-3 if king, 0 if empty, 1-5 if headed by 1-5 original cards
	4 bits: number of additional cards in sequence
If the original cards include a king at the top, or an embedded sequence,
the column representation is canonicalised in favor of claiming fewer original
cards still in place.  (This ensures we always recognise king-topped columns,
and that sequences are always maximal.)

Knowing which cards are in the tableau uniquely identifies the position, so
the foundation and holding area are not considered when testing two positions
for equality, but they are encoded as part of the position for ease of access.

In addition to storing the original position (necessary in order to interpret
the above encoding), we also create a structure indexed by card that says
where each card started.  This makes it easy to see whether the card is
available for play.  (If the card's starting column does not still include
as many original cards as the given card's starting position, the card has
been reached.  If so, it is guaranteed to be available to move to a foundation,
and is also available to be built upon unless it is in the holding area.)

Input format is 52 cards represented either as rank then suit or suit then
rank, where suit is one of CDHS and rank is one of A23456789TJQK.  Lower-case
is okay too.  First two cards are the initial hold area, next ten are the tops
(most buried) of each column, etc.  Other characters such as spaces and line
breaks are ignored and can be used for readability.  E.g.:

         .. 7h 9s ..
Kc 5d 8h 3h 6d 9d 7d As 3c Js
8c Kd 2d Ac 2s Qc Jh 8d Th Ts
9h 5h Qs 6c 4s 6h Ad 8s 2h 4d
Ah 7s 3d Jd 9c 3s Qd Kh 7c 4c
Jc Qh 2c Tc 5s 4h 5c Td Ks 6s

*****************************************************************************/

#include <stdio.h>
#include <string.h>

#define true  (1==1)
#define false (1==0)

typedef unsigned char	uchar;
typedef uchar		Card;	/* 00ssrrrr: ss=suit(0-3), rr=rank(1-13) */
typedef uchar		Column; /* 10ssnnnn for kings, 0ooonnnn for 0-5 orig */

#define Rank(card)	((card) & 0x0F)
#define Suit(card)	((card) >> 4)

#define KingTopped(col)	(((col) & 0x80) != 0)
#define NumOrig(col)	((col) >> 4)
#define SeqLen(col)	((col) & 0x0F)
#define KingSuit(col)	((col) & 0x30)

typedef struct pos_struct {
	uchar	foundations[4];
	Card	hold[4];
	Column	tableau[10];
	struct pos_struct *via;	  /* position from which this was reached */
	struct pos_struct *chain; /* for chaining in hash table */
	long	hash;
} Position;

typedef struct {
	Card	tableau[10][5];
	Card	hold[2];
	uchar	started[0x40];	/* where each card started (0-49=tableau) */
} Original;

#define CLUBS	 (0 << 4)
#define DIAMONDS (1 << 4)
#define HEARTS   (2 << 4)
#define SPADES	 (3 << 4)

static int verbosity = 0;	/* how much to print out while running */
static int show = false;	/* whether to show solution if found */
static int showorig = false;	/* whether to show original layout */

/* statistics gathering */
static long generated = 0;	/* # times AddToTree called */
static long distinct = 0;	/* # different positions reached */
static long hashes = 0;		/* # hash table slots used */
static long buried = 0;		/* # positions rejected due to buried cards */

/* Random number package.  Roll our own since so many systems seem to have
 * pretty bad ones.  Include code for both MSDOS and UNIX time functions to
 * initialise it.
 */
#ifdef __MSDOS__
#include "time.h"
#define GET_TIME(var) {time(&var);}
#else
#include "sys/time.h"
#define GET_TIME(var) { \
	struct timeval now; \
	gettimeofday(&now, 0); \
	var = now.tv_sec + now.tv_usec; \
}
#endif

#define RAND_BUFSIZE	98
#define RAND_HASHWORD	27
#define RAND_BITSUSED	29
#define RAND_MASK	((1 << RAND_BITSUSED) - 1)

static long rand_buf[RAND_BUFSIZE], *rand_bufLoc;
static long Rand();
static char rand_flips[1<<6];
static short rand_flipper;

#define RAND_FLIPMASK (sizeof(rand_flips) - 1)

static RandFlipInit(loc) int loc;
{
	static char f[] =
	   " 11 111 1  1    11 1 111 1  11      11 11111   11 1 1 11  1 11  ";

	rand_flipper = loc;
	for (loc=0; loc<=RAND_FLIPMASK; loc++)
		rand_flips[loc] = (f[loc]==' '? 0 : -1);
}

static RandInit(seed) long seed;
{
	int n;

	if (seed == 0) GET_TIME(seed);
	memset(rand_buf, 0, sizeof(rand_buf));
	for (n=0; n<RAND_BITSUSED; n++)
	{	rand_buf[n] = RAND_MASK >> n;
		rand_buf[n+RAND_BITSUSED+3] = 1 << n;
	}
	for (n=RAND_BITSUSED*2+3; n<RAND_BUFSIZE; n++)
	{	rand_buf[n] = seed & RAND_MASK;
		seed = seed * 3 + 01234567;
	}
	rand_bufLoc = rand_buf;
	RandFlipInit(0);
	for (n=(seed & 0377); n<50000; n++) Rand();
}

static long Rand()
{
	long f;

	if (!rand_bufLoc) RandInit(0);
	rand_bufLoc = (rand_bufLoc == rand_buf ?
		rand_bufLoc+RAND_BUFSIZE-1 : rand_bufLoc-1);
	*rand_bufLoc ^= *(rand_bufLoc >= rand_buf+RAND_HASHWORD ?
		rand_bufLoc-RAND_HASHWORD
		: rand_bufLoc+(RAND_BUFSIZE-RAND_HASHWORD));
	f = rand_flips[rand_flipper++ & RAND_FLIPMASK];
	return(*rand_bufLoc ^ (f & RAND_MASK));
}

static void ShuffleChar(arr, size) char *arr; int size;
{
	int swap;
	char temp;

	while (size > 1) {
		swap = Rand() % (size--);
		temp = arr[swap];
		arr[swap] = arr[size];
		arr[size] = temp;
	}
}

/* End of random number package
 *******************************/

static Original orig;

/* Read original position from stdin into orig.  First two cards read
 * are the initial holding area, then the top card of each column, etc.
 */
static void ReadOriginal(filename) char *filename;
{
	int count = 0;
	Card cd = 0;
	FILE *f = stdin;

	if (filename != NULL && (f = fopen(filename, "r")) == NULL) {
		printf("sea_test: can't open %s\n", filename);
		exit(1);
	}
	memset(&orig, -1, sizeof(orig));
	while (count < 104) {
		int c = getc(f);
		if (c == EOF) {
			printf("Insufficient input.\n");
			exit(1);
		}
		if (c >= '2' && c <= '9') cd += c - '0';
		else if ((c |= 0x20) == 'a') cd += 1;
		else if (c == 't') cd += 10;
		else if (c == 'j') cd += 11;
		else if (c == 'q') cd += 12;
		else if (c == 'k') cd += 13;
		else if (c == 'c') cd += CLUBS;
		else if (c == 'd') cd += DIAMONDS;
		else if (c == 'h') cd += HEARTS;
		else if (c == 's') cd += SPADES;
		else continue;
		if (count++ % 2 != 0) {
			int pos = (count>>1) - 3;
			if (pos < 0) orig.hold[pos+2] = cd;
			else orig.tableau[pos%10][pos/10] = cd;
			if (orig.started[cd] != 0xFF) {
				printf("Card %02x appears twice!\n", cd);
				exit(1);
			}
			orig.started[cd] = (pos+52) % 52;
			cd = 0;
		}
	}
	if (orig.hold[0] < orig.hold[1]) {
		cd = orig.hold[1];
		orig.hold[1] = orig.hold[0];
		orig.hold[0] = cd;
	}
	if (filename != NULL) fclose(f);
}

/* Build original as simple sorted deck.  Don't fill in tableau/hold yet.
 */
static void BuildOriginal()
{
	int i;

	memset(&orig, -1, sizeof(orig));
	for (i=0; i<52; i++) {
		Card cd = (i>>2)+1 + ((i&3)<<4);
		orig.started[cd] = i;
	}
}

/* Shuffle the starting positions in orig, then fill in tableau/hold.
 */
static Shuffle()
{
	int i;
	static uchar arrange[52];
	
	if (arrange[0] == arrange[1]) for (i=0; i<52; i++) arrange[i] = i;
	ShuffleChar(arrange, 52);
	for (i=0; i<0x40; i++)
		if (orig.started[i] != 0xFF)
			orig.started[i] = arrange[orig.started[i]];
	for (i=0; i<52; i++) {
		Card cd = (i>>2)+1 + ((i&3)<<4);
		int where = orig.started[cd];
		if (where < 50) orig.tableau[where%10][where/10] = cd;
		else orig.hold[where-50] = cd;
	}
	if (orig.hold[0] < orig.hold[1]) {
		Card cd = orig.hold[1];
		orig.hold[1] = orig.hold[0];
		orig.hold[0] = cd;
	}
}

/* Allocate a new Position structure.  Allocates many at a time to reduce
 * malloc overhead.  Saves linked list of blocks of positions to enable
 * freeing the storage, which can be necessary if we're examining hundreds
 * of positions in a single run of the program.
 */
#define POS_BLOCK 1000
typedef struct pos_block {
	Position block[POS_BLOCK];
	struct pos_block *link;
} Block;

static int pos_avail = 0;
static Block *block = NULL;

static Position *NewPosition()
{
	if (pos_avail-- == 0) {
		Block *more = (Block *) malloc(sizeof(Block));
		if (more == NULL) {
			printf("\nERROR: Out of memory!\n");
			exit(2);
		}
		more->link = block;
		block = more;
		pos_avail = POS_BLOCK - 1;
	}
	return (block->block + pos_avail);
}

/* Find the bottom (i.e., immediately playable) card in a column.
 * Return 0 for empty columns.
 */
static Card Bottom(pos, col) Position *pos; int col;
{
	Column c = pos->tableau[col];
	Card seqtop;

	if (c == 0) return(0);
	if (KingTopped(c)) seqtop = KingSuit(c) + 13;
	else seqtop = orig.tableau[col][NumOrig(c)-1];
	return(seqtop - SeqLen(c));
}

/* Show a position in internal format.  Can be called from debugger.
 */
ShowPosition(pos) Position *pos;
{
	int i;

	printf("F:");
	for (i=0; i<4; i++) printf(" %d", pos->foundations[i]);
	printf("; H:");
	for (i=0; i<4; i++) printf(" %02x", pos->hold[i]);
	printf("; T:");
	for (i=0; i<10; i++) printf(" %02x", pos->tableau[i]);
	printf("\n");
}

/* Show a card; rank in uppercase, suit in lowercase, trailing space.
 */
ShowCard(card) Card card;
{
	static char *rank = "?A23456789TJQK", *suit = "cdhs";
	
	if (card == 0) printf(".. ");
	else printf("%c%c ", rank[Rank(card)], suit[Suit(card)]);
}

/* Show a position in more readable format.  Gives bottom sequence for each
 * column (e.g., "Q-8s"), then holding area and foundation tops.  Format:
X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys X-Ys | Hs Hs Hs Hs | Fc Fd Fh Fs
 */
ReadablePosition(pos) Position *pos;
{
	int i;
	static char *rank = "?A23456789TJQK", *suit = "cdhs";

	for (i=0; i<10; i++) {
		Column col = pos->tableau[i];
		Card bot = Bottom(pos, i);
		Card top = bot + SeqLen(col);
		if (col == 0) printf("  .. ");
		else {
			if (top == bot) printf("  ");
			else printf("%c-", rank[Rank(top)]);
			ShowCard(bot);
		}
	}
	printf("| ");
	for (i=0; i<4; i++) ShowCard(pos->hold[i]);
	printf("|");
	for (i=0; i<4; i++) {
		uchar moved = pos->foundations[i];
		if (moved == 0) printf(" ..");
		else printf(" %c%c", rank[moved], suit[i]);
	}
	printf("\n");
}
	
/* Show the winning path, in reverse order (because it's convenient).
 */
void ShowPath(pos) Position *pos;
{
	printf("Winning path (in reverse order):\n");
	while (pos != NULL) {
		ReadablePosition(pos);
		pos = pos->via;
	}
	printf("\n");
}
	
/* Show the original position as a rectangular layout.
 */
void ShowOriginal()
{
	int row, col;

	printf("\n\t .. ");
	ShowCard(orig.hold[0]);
	ShowCard(orig.hold[1]);
	printf("..\n");
	for (row=0; row<5; row++) {
		for (col=0; col<10; col++)
			ShowCard(orig.tableau[col][row]);
		printf("\n");
	}
	printf("\n");
}

#define HASH_SIZE 19001
static Position **tree;

/* See if this position is already in the search tree.  If so, return false.
 * Else add it and return true.  Position is assumed to be in canonical form,
 * so the tableau uniquely determines the hold area and foundations.
 */
static int AddToTree(pos) Position *pos;
{
	long hash = 0;
	int i;
	Position **probe;

	generated++;
	for (i=0; i<10; i++) hash = hash*5 + pos->tableau[i];
	pos->hash = hash;
	probe = &tree[hash % HASH_SIZE];
	if (*probe == NULL) hashes++;
	else while (*probe != NULL) {
		if ((*probe)->hash == hash && memcmp((*probe)->tableau,
				pos->tableau, 10*sizeof(Column)) == 0)
			return(false);
		probe = &((*probe)->chain);
	}
	*probe = pos;
	pos->chain = NULL;
	distinct++;
	return(true);
}

/* If the specified card is still in its original column and not yet uncovered,
 * return -1.  Else return the number of the column where the card lies, or the
 * number of its holding area spot.  Only the first test needs to be quick,
 * since if the card HAS been uncovered we're about to make some progress.  We
 * can also assume we're never asked about a card in the foundation piles;
 * indeed, we're only asked about cards that, if they've been uncovered, are
 * currently playable.
 */
static int Available(pos, card) Position *pos; Card card;
{
	uchar whence = orig.started[card];
	int i;

	if (whence < 50 && !KingTopped(pos->tableau[whence%10])
		&& NumOrig(pos->tableau[whence%10]) > (whence/10)+1)
		return(-1);
	for (i=0; i<4; i++) if (pos->hold[i] == card) return(10+i);
	for (i=0; i<10; i++) if (Bottom(pos, i) == card) return(i);
	printf("\nERROR: card %02x not covered but not found\n", card);
	exit(2);
}

/* Sort the holding area into descending order (hence spaces come last).
 * Use homogeneous sorting network.
 */
static void SortHoldingArea(pos) Position *pos;
{
	Card *h = pos->hold;
	Card temp;

#define SWAP(i,j) if (h[i]<h[j]) {temp=h[i]; h[i]=h[j]; h[j]=temp;}

	SWAP(0,1)
	SWAP(2,3)
	SWAP(0,2)
	SWAP(1,3)
	SWAP(1,2)
}

/* Make sure all columns consisting of just king-topped sequences come before
 * all empty columns, and that the king-topped ones are sorted by suit.  Note
 * that this wouldn't work if a king-topped column were represented in terms
 * of an "original" king, but such columns were dealt with by Uncover.
 *
 * Note: This routine is pretty inefficient, since it not only uses a bubble
 * sort but also looks at the non-king columns every pass.  Check whether it
 * gets called much and if so think about speeding it up.
 */
static void SortKingsAndEmpties(pos) Position *pos;
{
	int changed, i, previ;
	Column prevcol;

	do {
		changed = false;
		prevcol = 0xFF;
		for (i=0; i<10; i++) {
			Column col = pos->tableau[i];
			if (col==0 || KingTopped(col)) {
				if (col > prevcol) {
					pos->tableau[i] = prevcol;
					pos->tableau[previ] = col;
					changed = true;
				}
				else
					prevcol = col;
				previ = i;
			}
		}
	} while (changed);
}

/* Given a column (which is assumed to be non-empty), remove the currently
 * playable sequence.  Then make sure that (a) the new sequence at the end is
 * maximal, i.e. combine it if possible with the card immediately above, and
 * (b) if the column contains only a king, it's converted to a king-sequence
 * column even though the king started there.  Returns the index where the
 * column ended up (same as the input unless SortKings was called).
 */
static int Uncover(pos, i) Position *pos; int i;
{
	Column col = pos->tableau[i];

	if (KingTopped(col)) col = 0;
	else col = (col & 0xF0) - 0x10;
	while (col != 0) {
		int num = NumOrig(col);
		Card seqtop = orig.tableau[i][num-1];
		if (num == 1) {
			if (Rank(seqtop) == 13)
				col = 0x80 + (Suit(seqtop) << 4) + SeqLen(col);
			break;
		}
		if (seqtop != orig.tableau[i][num-2] - 1) break;
		col -= (1<<4) - 1;	/* 1 less orig, 1 more in seq */
	}
	pos->tableau[i] = col;
	if (col == 0 || KingTopped(col)) SortKingsAndEmpties(pos);
	if (pos->tableau[i] == col) return(i);
	for (i=0; i<10; i++) if (pos->tableau[i] == col) return(i);
	printf("\nERROR: column got lost while sorting kings\n");
	exit(2);
}

/* Return true if given card can be played to a foundation in given position.
 */
static int PlaysToFoundation(pos, card) Position *pos; Card card;
{
	if (Rank(card) == pos->foundations[Suit(card)] + 1) return(true);
	return(false);  /* note: works (returns false) if card==0 */
}

/* If specified column plays to a foundation, do so and return true.
 */
static int TableauToFoundation(pos, i) Position *pos; int i;
{
	Column col = pos->tableau[i];
	Card bot = Bottom(pos, i);

	if (! PlaysToFoundation(pos, bot)) return(false);
	pos->foundations[Suit(bot)] += 1 + SeqLen(col);
	(void) Uncover(pos, i);
	return(true);
}

/* If specified holding area card plays to foundation, do so and return true.
 */
static int HoldToFoundation(pos, i) Position *pos; int i;
{
	Card held = pos->hold[i];

	if (! PlaysToFoundation(pos, held)) return(false);
	pos->hold[i] = 0;
	pos->foundations[Suit(held)] += 1;
	return(true);
}

/* Add a sequence of cards (given as top card and number of additional cards)
 * to the holding area.  Assumes there is room.  Sorts the holding area.
 */
static void AddToHold(pos, seqtop, slen) Position *pos; Card seqtop; int slen;
{
	while (slen-- >= 0) pos->hold[2-slen] = seqtop--;
	SortHoldingArea(pos);
}

/* Perform various automatic operations to put a position into canonical form,
 * such that obviously equivalent positions are combined.  These include:
 *	- moving cards to the foundations
 *	- moving non-Kings down from the holding area to the tableau
 *	- sorting the cards (and empty slots) in the holding area
 *	- sorting king-topped and empty columns (done in Uncover)
 * The latter two are assumed not to be necessary unless the other operations
 * may have led to things being unsorted.
 */
static void Canonical(pos) Position *pos;
{
	int sortHold = false;
	int changed, i;

	do {	/* look for moves to foundation */
		changed = false;
		for (i=0; i<10; i++)
			if (TableauToFoundation(pos, i)) changed = true;
		for (i=3; i>=0; i--) /* reverse order moves seqs in one pass */
			if (HoldToFoundation(pos, i)) changed = sortHold = true;
	} while (changed);

	/* look for moves from holding area to tableau */
	/* single forward pass moves all possible */
	for (i=0; i<4; i++) if (pos->hold[i]!=0) {
		Card held = pos->hold[i];
		int whither = (Rank(held)==13? -1 : Available(pos, held+1));
		if (whither >= 0 && whither <= 9) {
			pos->tableau[whither] += 1;
			pos->hold[i] = 0;
			sortHold = true;
		}
	}

	if (sortHold) SortHoldingArea(pos);
}

/* Check whether a newly available card in a particular column requires
 * that we call Canonical.  If the card moves to a foundation, call the
 * full Canonical routine.  If cards just move down from the hold area,
 * no other action is needed.
 */
void MiniCanon(pos, i) Position *pos; int i;
{
	Card want;
	int j, sortHold = false;

	if (TableauToFoundation(pos, i)) {Canonical(pos); return;}

	/* look for moves from holding area to tableau */
	/* single forward pass moves all possible */
	want = Bottom(pos, i) - 1;
	for (j=0; j<4 && pos->hold[j]!=0; j++) {
		if (pos->hold[j] == want) {
			pos->tableau[i] += 1;
			pos->hold[j] = 0;
			want -= 1;
			sortHold = true;
		}
	}
	if (sortHold) SortHoldingArea(pos);
}

/*
 * Move generator; given a column index and index of first empty column
 * (if any), decide the unique reasonable move (if any) from the given
 * column and apply supplied recursive proc.
 */
static Position *DFScol(via, pos, i, empty, proc)
     Position *via, *pos; int i, empty; Position *(*proc)();
{
	Column col = via->tableau[i];
	int target, slen = SeqLen(col);
	Card seqtop;
	       
	if (col == 0 || KingTopped(col)) return(pos);
		/* do king-topped in try2 since they tend to be wasted moves */
		/* i.e., king-to-hold is often followed by the reverse move */
	if (slen > 4 || (slen > 0 && via->hold[4-slen] != 0)) return(pos);
	seqtop = orig.tableau[i][NumOrig(col)-1];
	if (Rank(seqtop) == 13) {
		if (empty >= 0) {
			/* move lower king-seq to space */
			*pos = *via;
			pos->via = via;
			pos->tableau[empty] = 0x80 +
				(Suit(seqtop)<<4) + slen;
			SortKingsAndEmpties(pos);
			MiniCanon(pos, Uncover(pos, i));
			if (AddToTree(pos)) {
				pos = (*proc)(pos, NewPosition());
				if (pos == NULL) return(NULL);
			}
		}
		else if (slen < 4 && via->hold[3-slen] == 0) {
			/* move lower king-seq to hold */
			*pos = *via;
			pos->via = via;
			AddToHold(pos, seqtop, slen);
			MiniCanon(pos, Uncover(pos, i));
			if (AddToTree(pos)) {
				pos = (*proc)(pos, NewPosition());
				if (pos == NULL) return(NULL);
			}
		}
	}
	else if ((target = Available(via, seqtop+1)) >= 0 && target <= 9) {
		/* move seq to tableau */
		*pos = *via;
		pos->via = via;
		pos->tableau[target] += 1 + slen;
		MiniCanon(pos, Uncover(pos, i));
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	else if (slen < 4 && via->hold[3-slen] == 0) {
		/* move seq to hold */
		*pos = *via;
		pos->via = via;
		AddToHold(pos, seqtop, slen);
		MiniCanon(pos, Uncover(pos, i));
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	return(pos);
}

/*
 * Move generator; given an empty column, look for kings in the holding
 * area and try moving each of them into the empty column.
 */
static Position *DFSempty(via, pos, empty, from, thru, proc)
     Position *via, *pos; int empty, from, thru; Position *(*proc)();
{
	int i;

	for (i=from; i<=thru; i++) if (Rank(via->hold[i]) == 13) {
		/* just move king down and let MiniCanon do the rest */
		*pos = *via;
		pos->via = via;
		pos->tableau[empty] = 0x80 + (Suit(via->hold[i]) << 4);
		pos->hold[i] = 0;
		SortHoldingArea(pos);
		MiniCanon(pos, empty);
		SortKingsAndEmpties(pos);
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	return(pos);
}

/*
 * Move generator; given a column, if it's king-topped, move all of it into
 * the holding area.  This generator isn't called if there's an empty column.
 */
static Position *DFSking(via, pos, i, proc)
     Position *via, *pos; int i; Position *(*proc)();
{
	Column col = via->tableau[i];
	int slen = SeqLen(col);

	if (KingTopped(col) && slen < 4 && via->hold[3-slen] == 0) {
		*pos = *via;
		pos->via = via;
		AddToHold(pos, KingSuit(col)+13, slen);
		(void) Uncover(pos, i);
		if (AddToTree(pos)) {
			pos = (*proc)(pos, NewPosition());
			if (pos == NULL) return(NULL);
		}
	}
	return(pos);
}

/* Depth-first search.  Generate all canonical positions reachable from "via".
 * Use "pos" as the place to construct new positions; ask for a new pos struct
 * as needed.  For positions that haven't already been reached, add them and
 * call DFS recursively.  If a winning position is found, return NULL.  Else
 * return a pointer to a not-yet-used Position struct.
 */
static Position *DFS(via, pos) Position *via, *pos;
{
	int i, target, empty = -1;

	if (verbosity >= 2) {
		Position *p = via->via;
		while (p != NULL) {printf(" "); p = p->via;}
		ShowPosition(via);
		fflush(stdout);
	}

	/* check for victory */
	if (via->foundations[0] == 13 && via->foundations[1] == 13
		&& via->foundations[2] == 13 && via->foundations[3] == 13) {
		if (show) ShowPath(via);
		return(NULL);
	}

	/* see if any card buried too deeply under higher cards of same suit */
	for (i=0; i<10; i++) {
		Column col = via->tableau[i];
		if (SeqLen(col) > 4 && !KingTopped(col)) {
			Card bot = Bottom(via, i);
			int k = NumOrig(col) - 1;
			while (k-- > 0) {
				Card oc = orig.tableau[i][k];
				if (Suit(oc) == Suit(bot) && oc < bot) {
					buried++;
					return(pos);
				}
			}
		}
	}

	for (i=0; i<10; i++) if (via->tableau[i] == 0) {empty = i; break;}

	for (i=0; i<10; i++) {
		pos = DFScol(via, pos, i, empty, DFS);
		if (pos == NULL) return(NULL);
	}

	/* if there was an empty space, check for kings in holding area */
	if (empty >= 0) {
		pos = DFSempty(via, pos, empty, 0, 3, DFS);
		if (pos == NULL) return(NULL);
	}
	/* if no empty space, consider moving king-topped cols to hold area */
	else {
		for (i=0; i<10; i++) {
			pos = DFSking(via, pos, i, DFS);
			if (pos == NULL) return(NULL);
		}
	}

	return(pos);
}

/* Initialise a Position structure to represent the original position:
 * Each column contains 5 original cards plus no extras.  Actually set
 * each column to 6 cards, then use Uncover to remove a card (and maximise
 * sequences where possible).
 */
static void InitialPosition(pos) Position *pos;
{
	int i;
	
	memset(pos, 0, sizeof(*pos));
	pos->hold[0] = orig.hold[0];
	pos->hold[1] = orig.hold[1];
	for (i=0; i<10; i++) pos->tableau[i] = (6<<4); /* 6 original cards */
	for (i=0; i<10; i++) (void) Uncover(pos, i);
}

static char *initSeq;

static Position *DFSinit(via, pos) Position *via, *pos;
{
	int i, empty = -1, n = *initSeq++ - '0';

	if (n < 0) return DFS(via, pos);

	if (verbosity >= 2) {
		Position *p = via->via;
		if (p != NULL) {printf("*"); p = p->via;}
		while (p != NULL) {printf(" "); p = p->via;}
		ShowPosition(via);
		fflush(stdout);
	}

	for (i=0; i<10; i++) if (via->tableau[i] == 0) {empty = i; break;}
	if (via->tableau[n] == 0) {
		n = *initSeq++ - '0';
		return DFSempty(via, pos, empty, n, n, DFSinit);
	}
	else if (KingTopped(via->tableau[n]))
		return DFSking(via, pos, n, DFSinit);
	else
		return DFScol(via, pos, n, empty, DFSinit);
}

main(argc, argv) int argc; char *argv[];
{
	Position *pos0;
	int i, reps = 1, make_random = false;
	char *filename = NULL, init[100];

	initSeq = init;
	for (i=1; i<argc; i++) {
		char *arg = argv[i];
		if (*arg == '-') {
			while (*++arg) switch (*arg) {
			case 'i':
				showorig = true;
				break;
			case 'r':
				make_random = true;
				break;
			case 's':
				show = true;
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				*initSeq++ = *arg;
				break;
			case 'v':
				if (verbosity++ < 2) break;
				/* FALL THROUGH */
			default:
				printf("\
Usage: sea_test [-irsvv###...] [file]\n\n\
-i:  show initial layout\n\
-r:  if no 'file' arg, generate random winnable position\n\
     else convert filename to int, generate and test that many positions\n\
-s:  show solution if found (else just report whether one exists)\n\
-v:  verbose; give some statistics when done\n\
-vv: very verbose; dump entire search tree as it is traversed\n\
-#:  column numbers (0-9) representing proposed initial move sequence\n");
				exit(1);
			}
		}
		else if (filename == NULL) filename = arg;
	}

	if (make_random) {
		BuildOriginal();
		if (filename != NULL) reps = atoi(filename);
		if (reps <= 0) reps = 1;
	}
	else
		ReadOriginal(filename);
	
	tree = (Position **) calloc(HASH_SIZE, sizeof(Position*));
	if (tree == NULL) {
		printf("\nERROR: Out of memory!\n");
		exit(2);
	}

	for (i=0; i<reps; i++) {
		if (make_random) Shuffle();
		if (showorig) ShowOriginal();
	
		pos0 = NewPosition();
		InitialPosition(pos0);
		Canonical(pos0);
		(void) AddToTree(pos0);

		*initSeq = '\0';
		initSeq = init;
		if (DFSinit(pos0, NewPosition()) == NULL) {
			if (make_random && filename==NULL && !showorig)
				ShowOriginal();
			printf("Winnable.\n");
		}
		else {
			printf("Impossible.\n");
			if (make_random && filename==NULL) i--;
		}

		if (verbosity > 0) printf("\n\
Generated %d positions (%d distinct).\n\
Used %d of %d hash table slots.\n\
Rejected %d sub-trees due to buried cards.\n",
			generated, distinct, hashes, HASH_SIZE, buried);
		
		if (i < reps) {
			/* clean up to prepare for next position */
			generated = distinct = hashes = buried = 0;
			memset(tree, 0, HASH_SIZE * sizeof(Position*));
			while (block->link != NULL) {
				Block *link = block->link;
				free(block);
				block = link;
			}
			pos_avail = POS_BLOCK;
		}
	}

	exit(0);
}
