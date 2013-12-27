// freecell_solver.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <conio.h>
#include <time.h>
#include "jhash.h"
#include "list.h"

#include "freecell_solver.h"
////////////////////////////////////////////////////////////////

#define inline __inline 

#if 0
# undef assert
# define assert(x)
#endif // 

#if 1
# define fprintf
#endif //

#if 1
# define fast_move
#endif //

////////////////////////////////////////////////////////////////

FILE *dbg_info_out = NULL;

int game_done = 0;
static inline void on_done_callback(void)
{
	game_done = 1;
	printf("###### DONE ######\n");
}

//int global_count1 = 0;
//int global_count2 = 0;

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

// get_color: 1 = black 0 = red
//#define get_color(x)    (((x) & 0x10) >> 4)
static inline char get_color(char card)
{
	return (card & 0x10) >> 4;
}

//#define get_type(x)		(((x) & 0xf0) >> 4)
static inline char get_type(char card)
{
	return (card & 0xf0) >> 4;
}

//#define get_digital(x)		((x) & 0x0f)
static inline char get_digital(char card)
{
	return (card & 0xf);
}


////////////////////////////////////////////////////////////////

static void card_to_string(char *buf, char card)
{
	int i, n = 0;

	if (get_digital(card) == 0) {
		// the initial state of final space ...
		buf[n++] = ' ';
		buf[n++] = ' ';
		buf[n++] = 0;
		return;
	}

	switch (get_type(card)) {
	case 1:
		buf[n++] = 'S';
		break;
	case 2:
		buf[n++] = 'H';
		break;
	case 3:
		buf[n++] = 'C';
		break;
	case 4:
		buf[n++] = 'D';
		break;
	default:
		assert(0);
		return;
	}

	i = get_digital(card);
	if (i < 10 && i > 1)
		buf[n++] = '0' + i;
	else {
		switch (i) {
		case 0x01:
//			buf[n++] = 'A';
			buf[n++] = '1';
			break;
		case 0x0a:
			buf[n++] = '1';
			buf[n++] = '0';
			break;
		case 0x0b:
			buf[n++] = 'J';
			break;
		case 0x0c:
			buf[n++] = 'Q';
			break;
		case 0x0d:
			buf[n++] = 'K';
			break;
		default:
			assert(0);
			return;
		}
	}

	buf[n++] = 0;
}

static inline char *card_to_string_with_inner_buffer(char card)
{
	static char buf[10];
	card_to_string(buf, card);
	return buf;
}

static void dump_step(const step *step)
{
	int i, j, n;
	char buf[10];
	const phase *phase; 

	assert(step);
	phase = &step->phase;

	fprintf(dbg_info_out, ">>>>>> dump step: %d >>>>>\n", step->id);

//	fprintf(dbg_info_out, "count: %d %d\n", global_count1, global_count2);

	fprintf(dbg_info_out, "Space: ");
	for (i = 0; i < 4; i++) {
		card_to_string(buf, phase->space[i]);
		fprintf(dbg_info_out, "\t%s", buf);	
	}
	fprintf(dbg_info_out, "\nFinal: ");
	for (i = 0; i < 4; i++) {
		card_to_string(buf, make_card(i + 1, get_digital(phase->final[i])));
		fprintf(dbg_info_out, "\t%s", buf);	
	}
	fprintf(dbg_info_out, "\nCard:\n");

	for (i = 0; i < MAX_CARD_PER_COL; i++) {
		n = 0;
		for (j = 0; j < 8; j++) {
			char c = phase->column[j][i];
			if (c) {
				n++;
			}
			card_to_string(buf, c);
			fprintf(dbg_info_out, "%s\t", buf);
		}
		fprintf(dbg_info_out, "\n");
		if (!n)
			break;
	}

	fprintf(dbg_info_out, "<<<<<< dump step: %d <<<<<<\n", step->id);
}

static inline int search_card_number(const phase *phase, char card)
{
	int i, j;
	int nr = 0;
	for (i = 0; i < 4; i++) 
		if (phase->space[i] == card)
			nr++;

	for (i = 0; i < 8; i++) 
		for (j = 0; j <= phase->pos[i]; j++) 
			if (phase->column[i][j] == card)
				nr++;

	return nr;
}

static int check_phase(const phase *phase)
{
	int i, j, n;
	char f;

	assert(phase);

	for (i = 0; i < 4; i++) {
		f = phase->space[i];
		if (f)
			assert_card(f);

		if (i < 3) {
			assert(f >= phase->space[i + 1]);
		}
	}

	for (i = 1; i <= 4; i++) {
		f = get_digital(phase->final[i - 1]);
		for (j = 1; j <= 0x0d; j++) {
			n = search_card_number(phase, make_card(i, j));
			if (j <= f && n) 
				assert(0);
			if (j > f && n != 1) 
				assert(0);
		}
	}

	for (i = 0; i < 8; i++) {
		n = 0;
		for (j = 0; j < MAX_CARD_PER_COL; j++) {
			if (phase->column[i][j] == 0x00) {
				if (n == 0)
					assert(phase->pos[i] == j - 1);
				n = 1;
			} else if (n == 1) {
				assert(0);
			}
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////

static inline char is_space_full(const phase *phase)
{
	// we sort space card here ... so we just need to test space[3]
	assert(phase);
	return phase->space[3];
}

static inline char is_space_empty(const phase *phase)
{
	assert(phase);
	return phase->space[0];
}

static inline int get_space_count(const phase *phase)
{
	if (phase->space[3])
		return 0;
	else if (phase->space[2])
		return 1;
	else if (phase->space[1])
		return 2;
	else if (phase->space[0])
		return 3;
	else
		return 4;
}

static inline char get_space(const phase *phase, int space_id)
{
	assert(phase && space_id < 4 && space_id >= 0);
	return phase->space[space_id];
}

static inline char set_space(phase *phase, int space_id, char card)
{
	assert(phase && space_id < 4 && space_id >= 0);
	if (card)
		assert_card(card);
	phase->space[space_id] = card;
}

static inline void insert_card_to_space(phase *phase, char card)
{
	char s;
	int i;

	assert(phase);
	assert_card(card);
	assert(!is_space_full(phase));

	for (i = 2; i >= 0; i--) {
		s = phase->space[i];
		if (s < card)
			phase->space[i + 1] = s;
		else {
			phase->space[i + 1] = card;
			return;
		}
	}

	phase->space[0] = card;
}

static inline void remove_card_from_space(phase *phase, int space_id)
{
	int n;

	assert(phase && space_id >= 0 && space_id < 4);
	assert_card(get_space(phase, space_id));
	for (n = space_id; n < 3; n++) {
		phase->space[n] = get_space(phase, n + 1);
	}
	phase->space[3] = 0x00;
}

////////////////////////////////////////////////////////////////

static inline char get_final(const phase *phase, int type)
{
	assert(phase && type >= 1 && type <= 4);
	return get_digital(phase->final[type - 1]);
}

static inline void increase_final(phase *phase, int type)
{
	assert(phase && type >= 1 && type <= 4);
	phase->final[type - 1]++;
}

////////////////////////////////////////////////////////////////

static inline void erase_end_card(phase *phase, int cid)
{
//	int i;
//	char card;

	assert(phase && cid <= 7 && cid >= 0);
	assert(phase->column[cid][0]);

	phase->column[cid][phase->pos[cid]] = 0x00;
	phase->pos[cid]--;
/*
	for (i = MAX_CARD_PER_COL - 1; i >= 0; i--) {
		card = phase->column[cid][i];
		if (card) {
			phase->column[cid][i] = 0x00;
			return;
		}
	}
*/
}

static inline int get_end_card_pos(const phase *phase, int cid)
{
//	int i;
//	char card;
	
	assert(phase && cid <= 7 && cid >= 0);
/*	for (i = MAX_CARD_PER_COL - 1; i >= 0; i--) {
		card = phase->column[cid][i];
		if (card)
			return i;
	}
	return -1;
*/
	return phase->pos[cid];
}

static inline char get_card_by_pos(const phase *phase, int cid, int pos)
{
	assert(phase && cid <= 7 && cid >= 0 && pos >= 0 && pos <= phase->pos[cid]);
	return phase->column[cid][pos];
}

static inline char get_end_card(const phase *phase, int cid)
{
//	int i;
//	char card;

	assert(phase && cid <= 7 && cid >= 0);
/*	for (i = MAX_CARD_PER_COL - 1; i >= 0; i--) {
		card = phase->column[cid][i];
		if (card)
			return card;
	}
*/
	if (phase->pos[cid] >= 0)
		return phase->column[cid][phase->pos[cid]];
	return 0x00;
}

static inline char cut_end_card(phase *phase, int cid)
{
//	int i;
	char card;

	assert(phase && cid <= 7 && cid >= 0);
	assert(phase->column[cid][0]);

	card = phase->column[cid][phase->pos[cid]];
	phase->column[cid][phase->pos[cid]] = 0x00;
	phase->pos[cid]--;
	return card;
/*
	for (i = MAX_CARD_PER_COL - 1; i >= 0; i--) {
		card = phase->column[cid][i];
		if (card) {
			phase->column[cid][i] = 0x00;
			return card;
		}
	}
	return 0x00;
*/
}

static inline void append_card_to_column(phase *phase, int cid, char card)
{
	assert(phase && cid <= 7 && cid >= 0);
	assert_card(card);
	assert(phase->pos[cid] < MAX_CARD_PER_COL - 1);

	phase->pos[cid]++;
	phase->column[cid][phase->pos[cid]] = card;
}

////////////////////////////////////////////////////////////////

static inline int find_card_in_columns(const phase *phase, char card, int *cid, int *pos)
{
	int i, j;

	assert(phase && cid && pos);
	assert_card(card);

	for (i = 0; i < 8; i++) {
		for (j = 0; j <= phase->pos[i]; j++) {
			if (phase->column[i][j] == card) {
				*cid = i;
				*pos = j;
				return 0;
			}
		}
	}

	return -1;
}

static inline int find_card_in_space(phase *phase, char card)
{
	int i;

	assert(phase);
	assert_card(card);

	for (i = 0; i < 4; i++) {
		if (get_space(phase, i) == card)
			return i;
	}

	return -1;
}

////////////////////////////////////////////////////////////////

static inline int compare_phase(const phase *left, const phase *right)
{
	// TODO
	assert(left && right);
	return memcmp(left, right, sizeof(*left));
}

static inline int check_done(step * pstep)
{
	phase *p = &pstep->phase;
	if (get_final(p, 1) == 0x0d 
		&& get_final(p, 2) == 0x0d
		&& get_final(p, 3) == 0x0d
		&& get_final(p, 4) == 0x0d) 
	{
		step *s = pstep;
		while (s) {
			s->state = ST_WIN;
			s = s->up_step;
		}

		on_done_callback();
		return 1;
	}
	return 0;
}

static inline int get_empty_column_count(const phase *phase)
{
	int i, nr = 0;
	for (i = 0; i < 8; i++) {
		if (phase->column[i][0] == 0x00)
			nr++;
	}
	return nr;
}

static inline int get_card_move_ability(phase *phase)
{
	return (get_space_count(phase) + 1) * (get_empty_column_count(phase) + 1);
}

static inline int is_link_able(char top, char under)
{
	assert_card(top);
	assert_card(under);
	return get_color(top) != get_color(under) && get_digital(under) + 1 == get_digital(top);
}

////////////////////////////////////////////////////////////////

static inline void calc_step_hash_value(step *step)
{
	assert(step);
	step->hash = jhash(&step->phase, sizeof(step->phase) / sizeof(u32), 0x12345678);
}

static inline void insert_step_to_hash_bucket(step *sub, game *game)
{
	int n;
	assert(sub && game);
	n = sub->hash % MAX_HASH_BUCKET;
	sub->hash_next = game->hash_bucket[n];
	game->hash_bucket[n] = sub;
}

////////////////////////////////////////////////////////////////

static inline step *clone_step_from_phase(const phase *phase, game *game)
{
	step *pstep;
	
	assert(phase);
	check_phase(phase);

	pstep = (step *)malloc(sizeof(step));
	if (pstep) {
		memcpy (&pstep->phase, phase, sizeof(*phase));

		INIT_LIST_HEAD(&pstep->sub);
		INIT_LIST_HEAD(&pstep->sub_list_head);
//		INIT_LIST_HEAD(&step->list);

		pstep->state = ST_INIT;
		pstep->level = 0;
		pstep->id = game->step_nr;
		pstep->hash_next = NULL;
		pstep->up_step = NULL;

//		list_add_tail(&step->list, &game->all_step);
		game->step_nr++;
	} else {
		assert(0);
	}
	return pstep;
}

static inline void release_step(step *step, game *game)
{
	int n;
	struct step *s, **prev;

	assert(step && game);
	assert(list_empty(&step->sub_list_head));
//	assert(step->state == ST_DIE);

	list_del(&step->sub);

	n = step->hash % MAX_HASH_BUCKET;
	s = game->hash_bucket[n];
	prev = &game->hash_bucket[n];

	while (s) {
		if (s == step) {
			*prev = s->hash_next;
			break;
		}

		prev = &s;
		s = s->hash_next;
	}

//	list_del(&step->list);
//	game->step_nr--;

	free(step);
}

static struct step *check_same_step( step *step, game *game)
{
	struct step *s;
	int n;

	assert(step && game);

	n = step->hash % MAX_HASH_BUCKET;
	s = game->hash_bucket[n];
	while (s) {
		if (s->hash == step->hash && compare_phase(&s->phase, &step->phase) == 0)
			return s;
		s = s->hash_next;
	}

	return NULL;
}

static inline int add_sub_level_step(step *node, step *sub, game *game)
{
	assert(node && sub);

	check_phase(&sub->phase);
	calc_step_hash_value(sub);

	if (check_same_step(sub, game)) {
		release_step(sub, game);
		return -1;
	} else {
		list_add_tail(&sub->sub, &node->sub_list_head);

		sub->level = node->level + 1;
		sub->up_step = node;

		insert_step_to_hash_bucket(sub, game);

		dump_step(sub);
		check_done(sub);

		return 0;
	}
}

////////////////////////////////////////////////////////////////

static void move_card_from_column_to_space(phase *phase, int col_id)
{
	char c;

	c = cut_end_card(phase, col_id);
	assert_card(c);

	fprintf(dbg_info_out, "MOVE: C%d(%s) -> S\n", col_id, card_to_string_with_inner_buffer(c));

	insert_card_to_space(phase, c);
}

static void move_card_from_space_to_column(phase *phase, int space_id, int cid)
{
	char c;
#ifndef fast_move
	int n;
#endif //

	c = get_space(phase, space_id);
	assert_card(c);

	fprintf(dbg_info_out, "MOVE: S(%s) -> C%d\n", card_to_string_with_inner_buffer(c), cid);

#ifndef fast_move
	n = get_end_card_pos(phase, cid);
	assert(n < MAX_CARD_PER_COL - 1);

	if (n >= 0) {
		assert(is_link_able(phase->column[cid][n], c));
	}
#endif //

	append_card_to_column(phase, cid, c);

	remove_card_from_space(phase, space_id);
}

static void move_card_from_space_to_final(phase *phase, int space_id)
{
	char c;
	assert(phase && space_id < 4 && space_id >= 0);
	c = get_space(phase, space_id);
	assert_card(c);

	fprintf(dbg_info_out, "MOVE: S(%s) -> F\n", card_to_string_with_inner_buffer(c));

	assert(get_final(phase, get_type(c)) + 1 == get_digital(c));

	increase_final(phase, get_type(c));
	remove_card_from_space(phase, space_id);
}

static void move_cards_between_column(phase *phase, int from_id, int to_id, int number)
{
	int i, n, m;
	char c;
#ifndef fast_move
	char k;
#endif // 

	assert(phase && from_id >= 0 && from_id <= 8 && to_id >= 0 && to_id <= 8 
		&& from_id != to_id && number > 0);
	assert(number <= get_card_move_ability(phase));


	n = get_end_card_pos(phase, from_id);
	assert(n + 1 >= number);
	m = n + 1 - number;

#ifndef fast_move
	if (number > 1) {
		k = get_card_by_pos(phase, from_id, n);

		for (i = 1, n--; i < number; i++, n--) {
			c = get_card_by_pos(phase, from_id, n);

			assert(is_link_able(c, k));

			k = c;
		}
	}
#endif //
	
	c = get_card_by_pos(phase, from_id, m);

	if (number > 1) {
		fprintf(dbg_info_out, "MOVE: C%d(%s) -%d-> C%d\n", from_id, 
			card_to_string_with_inner_buffer(c), number, to_id);
	} else {
		fprintf(dbg_info_out, "MOVE: C%d(%s) -> C%d\n", from_id, 
			card_to_string_with_inner_buffer(c), to_id);
	}

	n = get_end_card_pos(phase, to_id);
	assert(n + number < MAX_CARD_PER_COL);

#ifndef fast_move
	k = get_card_by_pos(phase, to_id, n);
	assert(is_link_able(k, c));
#endif //

	n++;
	for (i = 0; i < number; i++) {
		phase->column[to_id][n++] = phase->column[from_id][m];
		phase->column[from_id][m++] = 0x00;
	}

	phase->pos[from_id] -= number;
	phase->pos[to_id] += number;
}

static void move_card_from_column_to_final(struct phase *phase, int col_id)
{
	char c;
	int t;

	c = cut_end_card(phase, col_id);
	assert_card(c);

	fprintf(dbg_info_out, "MOVE: C%d(%s) -> F\n", col_id, card_to_string_with_inner_buffer(c));

	t = get_type(c);

#ifndef fast_move
	if (get_final(phase, t) == get_digital(c) - 1) {
#endif //
		increase_final(phase, t);
#ifndef fast_move
	} else {
		assert(0);
	}
#endif //
}

////////////////////////////////////////////////////////////////

static inline int step1_direct_to_final(step *step, game *game)
{
	struct phase *p;
	struct step *s;
	char t, c[2];
	int i, n, sum = 0;

	fprintf(dbg_info_out, "STEP1: direct_to_space begin ...\n");
	assert(step && game);
	s = clone_step_from_phase(&step->phase, game);
	p = &s->phase;

	do {
		n = 0;
		c[0] = __min(get_final(p, 1), get_final(p, 3));	// black
		c[1] = __min(get_final(p, 2), get_final(p, 4));	// red

		for (i = 0; i < 8; i++) {
			t = get_end_card(&s->phase, i);
			if (!t)
				continue;

			if (get_digital(t) == get_final(p, get_type(t)) + 1) {
				if (get_digital(t) <= c[get_color(t)] + 2) {
					move_card_from_column_to_final(p, i);
					n++;
				}
			}
		}

		for (i = 0; i < 4; i++) {
			t = get_space(p, i);
			if (!t)
				continue;

			if (get_digital(t) == get_final(p, get_type(t)) + 1) {
				if (get_digital(t) <= c[get_color(t)] + 2) {
					move_card_from_space_to_final(p, i);
					n++;
				}
			}
		}

		sum += n;
	} while (n);

	if (sum) {
		if (add_sub_level_step(step, s, game))
			sum = 0;
	} else {
		release_step(s, game);
	}

	fprintf(dbg_info_out, "STEP1: direct_to_space end (%d) ...\n", sum);
	return sum > 0 ? 1 : 0;
}

static inline int step2_move_between_column(struct step *step, struct game *game)
{
	int i, j, k, sum = 0;
	struct phase *p;
	int max;

	fprintf(dbg_info_out, "STEP2: move_between_column begin ...\n");

	assert(step && game);
	p = &step->phase;

	max = get_card_move_ability(p);
	for (i = 0; i < 8; i++) {
		char c, n[2];
		c = get_end_card(p, i);
		if (get_digital(c) > 1) {
			if (get_color(c)) {
				n[0] = make_card(2, get_digital(c) - 1);
				n[1] = make_card(4, get_digital(c) - 1);
			} else {
				n[0] = make_card(1, get_digital(c) - 1);
				n[1] = make_card(3, get_digital(c) - 1);
			}

			for (j = 0; j < 2; j++) {
				int cid, pos;
				if (find_card_in_columns(p, n[j], &cid, &pos) == 0) {
					int nr, f;
					k = get_end_card_pos(p, cid);

					nr = k - pos + 1;
					if (nr > max)
						continue;
					
					f = 1;
					for ( ; k > pos; k--) {
						if (!is_link_able(get_card_by_pos(p, cid, k - 1), get_card_by_pos(p, cid, k))) {
							f = 0;
							break;
						}
					}

					if (f) {
						struct step *s = clone_step_from_phase(p, game);
						move_cards_between_column(&s->phase, cid, i, nr);
						if (!add_sub_level_step(step, s, game))
							sum++;
					}
				}
			}
		}
	}

	fprintf(dbg_info_out, "STEP2: move_between_column end (%d) ...\n", sum);
	return sum;
}

static inline int step4_move_between_space_and_column(struct step *step, struct game *game)
{
	int i, j, sum = 0;
	struct phase *p;
	char c, k;
	
	fprintf(dbg_info_out, "STEP4: move_between_space_and_column begin ...\n");
	assert(step && game);
	p = &step->phase;

	for (i = 0; i < 4; i++) {
		c = get_space(p, i);
		if (!c) 
			break;
		for (j = 0; j < 8; j++) {
			k = get_end_card(p, j);
			if ((k && is_link_able(k, c)) || !k) {
				struct step *s = clone_step_from_phase(p, game);
				check_phase(&s->phase);
				move_card_from_space_to_column(&s->phase, i, j);
				check_phase(&s->phase);
				if (!add_sub_level_step(step, s, game))
					sum++;
			}
		}
	}

	if (!is_space_full(p)) {
		for (i = 0; i < 8; i++) {
			c = get_end_card(p, i);
			if (c) {
				struct step *s = clone_step_from_phase(p, game);
				move_card_from_column_to_space(&s->phase, i);
				if (!add_sub_level_step(step, s, game))
					sum++;
			}
		}
	}

	fprintf(dbg_info_out, "STEP4: move_between_space_and_column end (%d) ...\n", sum);
	return sum;
}

static inline int step3_force_move_card_to_final(step *step, struct game *game)
{
	int i, j, sum = 0;
	struct phase *p;
	char c, f;

	fprintf(dbg_info_out, "STEP3: force_move_card_to_final begin ...\n");
	assert(step && game);
	p = &step->phase;

	for (i = 1; i <= 4; i++) {
		f = get_final(p, i);
		j = find_card_in_space(p, make_card(i, get_digital(f) + 1));
		if (j != -1) {
			struct step *s = clone_step_from_phase(p, game);
			move_card_from_space_to_final(&s->phase, j);
			if (!add_sub_level_step(step, s, game))
				sum++;
		}
	}

	for (i = 0; i < 8; i++) {
		c = get_end_card(p, i);
		if (!c)
			continue;
		f = get_final(p, get_type(c));
		if (get_digital(c) == get_digital(f) + 1) {
			struct step *s = clone_step_from_phase(p, game);
			move_card_from_column_to_final(&s->phase, i);
			if (!add_sub_level_step(step, s, game))
				sum++;
		}
	}

	fprintf(dbg_info_out, "STEP3: force_move_card_to_final end (%d) ...\n", sum);
	return sum;
}

//how many sons
static int do_step(struct step *step, struct game *game) 
{
	int sum = 0;
	assert(step && game);

	if (step->state != ST_INIT)
		return 0;

	fprintf(dbg_info_out, "#### do_step begin on step(%d)\n", step->id);
	dump_step(step);

	// 1. direct to finial
	if (!game_done)
		sum += step1_direct_to_final(step, game);
	if (!game_done)
		sum += step2_move_between_column(step, game);
	if (!game_done)
		sum += step3_force_move_card_to_final(step, game);
	if (!game_done)
		sum += step4_move_between_space_and_column(step, game);

	if (sum) 
		step->state = ST_STEP;
	else {
		step->state = ST_DIE;

		// TODO: cut the branch, 
		// if the top step don't have live branch, cut it too ...

	}

	fprintf(dbg_info_out, "#### do_step end on step(%d) sub step nr = %d\n", step->id, sum);
	return sum;
}

int do_freecell_solve(game& rGame)
{
	int sum;
	step *s;
//	struct list_head *p = &game->all_step;
//	struct list_head *end = game->all_step.prev;
//	clock_t clk;

	//assert(pGame);
	s = rGame.root_step;

	game_done = 0;
	while (!game_done) {
		printf("DEEP: %d  STEP: %d\n", s->level, s->id);
		sum = do_step(s, &rGame);
		if (sum) {
			// try to test the first sub step ...
			assert(!list_empty(&s->sub_list_head));
			s = list_entry(s->sub_list_head.next, struct step, sub);
		} else {
//			struct step *old = s;

			if (s->sub.next != &s->up_step->sub_list_head) {
				// try to test right step, s & it's right step are all up_step's sub step ...
				s = list_entry(s->sub.next, struct step, sub);
			} else if (s->up_step) {
				// if the up step exist(only root step have no up step), it will be try 
				// again, but do_step routine return 0 immediately, so the top node's 
				// right or up will be tested one by one ...
				s = s->up_step;
			} else {
				s = NULL;
				printf("fail\n");
				break;
			}

//			release_step(old, game);
		}
	}


/*	
	
*/
		
	return 0;
}

bool DoWideSearch(game& rGame)
{
    int sum;
    struct step *s;
    //struct list_head *p = rGame.all_step;
    //struct list_head *end = rGame.all_step.prev;
    clock_t clk;

    //assert(pGame);
    s = rGame.root_step;

    // 广度优先遍历, 节点数太多, 存储器资源不足... 
    //do {
    //    //fprintf(dbg_info_out, "\n******************************************\n");
    //    //fprintf(dbg_info_out, "LOOP: %d begin ... \n", cnt);
    //    //printf("LOOP: %d begin ... \n", cnt);
    //    clk = clock();
    //    sum = 0;
    //    nr = 0;
    //    do {
    //        p = p->next;
    //        s = list_entry(p, struct step, list);
    //        sum += do_step(s, game);
    //        nr++;
    //    } while (p != end);

    //    p = end;
    //    end = game->all_step.prev;

    //    //		list_for_each_entry_safe(s, safe, &game->all_step, struct step, list) {
    //    //			sum = do_step(s, game);
    //    //		}

    //    //		p = game->all_step.prev;

    //    fprintf(dbg_info_out, "LOOP: %d end, do_step_nr=%d new_step=%d\n time=%d", 
    //        cnt, nr, sum, (clock() - clk)/CLK_TCK);
    //    fprintf(dbg_info_out, "******************************************\n\n");
    //    printf("LOOP: %d end, do_step_nr=%d new_step=%d time=%d\n", 
    //        cnt, nr, sum, (clock() - clk)/CLK_TCK);
    //    cnt++;
    //} while (sum && !game_done);
    return true;
}

///////////////////////////////////////////////////////

struct phase the_11760 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x42, 0x36, 0x33, 0x47, 0x25, 0x35, 0x27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x1d, 0x1b, 0x26, 0x48, 0x4b, 0x1c, 0x2d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x2c, 0x18, 0x19, 0x15, 0x17, 0x12, 0x4c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x23, 0x28, 0x16, 0x29, 0x1a, 0x2a, 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x3d, 0x31, 0x2b, 0x44, 0x13, 0x4a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x22, 0x49, 0x4d, 0x34, 0x3a, 0x11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x43, 0x45, 0x3c, 0x46, 0x37, 0x41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x38, 0x39, 0x21, 0x3b, 0x32, 0x24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	}, 
};

struct phase the_test_1 = {
	{0x41, 0x31, 0x21, 0x11}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x4d, 0x3b, 0x39, 0x47, 0x45, 0x33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x1d, 0x4b, 0x29, 0x37, 0x35, 0x43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x2d, 0x1b, 0x49, 0x17, 0x25, 0x13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x3d, 0x2b, 0x19, 0x27, 0x15, 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x3c, 0x3a, 0x18, 0x46, 0x44, 0x42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x2c, 0x4a, 0x48, 0x36, 0x34, 0x32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x4c, 0x1a, 0x38, 0x26, 0x24, 0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x1c, 0x2a, 0x28, 0x16, 0x14, 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	}, 
};

struct phase the_test_2 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x41, 0x3b, 0x39, 0x47, 0x45, 0x33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x11, 0x4b, 0x29, 0x37, 0x35, 0x43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x21, 0x1b, 0x49, 0x17, 0x25, 0x13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x31, 0x2b, 0x19, 0x27, 0x15, 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x3c, 0x3a, 0x18, 0x46, 0x44, 0x42, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x2c, 0x4a, 0x48, 0x36, 0x34, 0x32, 0x2d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x4c, 0x1a, 0x38, 0x26, 0x24, 0x22, 0x3d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x1c, 0x2a, 0x28, 0x16, 0x14, 0x12, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	}, 
};

struct phase the_test_3 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x41, 0x33, 0x35, 0x47, 0x49, 0x3b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x11, 0x43, 0x25, 0x37, 0x39, 0x4b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x21, 0x13, 0x45, 0x17, 0x29, 0x1b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x31, 0x23, 0x15, 0x27, 0x19, 0x2b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x32, 0x34, 0x16, 0x48, 0x4a, 0x4c, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x22, 0x44, 0x46, 0x38, 0x3a, 0x3c, 0x2d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x42, 0x14, 0x36, 0x28, 0x2a, 0x2c, 0x3d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0x12, 0x24, 0x26, 0x18, 0x1a, 0x1c, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	}, 
};

struct phase the_30722 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x18, 0x1b, 0x45, 0x12, 0x2a, 0x4b, 0x35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x1a, 0x11, 0x14, 0x3d, 0x25, 0x2d, 0x29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x39, 0x31, 0x13, 0x36, 0x32, 0x27, 0x16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x4d, 0x2b, 0x4c, 0x15, 0x28, 0x46, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x23, 0x2c, 0x17, 0x21, 0x4a, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
		0x49, 0x44, 0x1c, 0x48, 0x3a, 0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x43, 0x42, 0x37, 0x3c, 0x26, 0x24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x33, 0x38, 0x41, 0x47, 0x34, 0x3b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
	},
};

struct phase the_19334 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x1a, 0x44, 0x35, 0x21, 0x11, 0x23, 0x13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x4d, 0x49, 0x36, 0x27, 0x2d, 0x14, 0x3d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x43, 0x4b, 0x3b, 0x12, 0x37, 0x42, 0x4c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x28, 0x24, 0x16, 0x25, 0x29, 0x15, 0x26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x4a, 0x48, 0x18, 0x31, 0x46, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
		0x38, 0x39, 0x2b, 0x1c, 0x3a, 0x17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x1b, 0x22, 0x2c, 0x33, 0x41, 0x34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x32, 0x45, 0x3c, 0x2a, 0x47, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
	},
};

/// TODO !!!!!!!!
struct phase the_25028 = {
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0, 0, 0, 0, 0, 0, 0, 0}, {
		0x1a, 0x44, 0x35, 0x21, 0x11, 0x23, 0x13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x4d, 0x49, 0x36, 0x27, 0x2d, 0x14, 0x3d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x43, 0x4b, 0x3b, 0x12, 0x37, 0x42, 0x4c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x28, 0x24, 0x16, 0x25, 0x29, 0x15, 0x26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x4a, 0x48, 0x18, 0x31, 0x46, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
		0x38, 0x39, 0x2b, 0x1c, 0x3a, 0x17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x1b, 0x22, 0x2c, 0x33, 0x41, 0x34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
		0x32, 0x45, 0x3c, 0x2a, 0x47, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   
	},
};

///////////////////////////////////////////////////////

static void init_phase_pos(struct phase *phase)
{
	int i, j;
	assert(phase);
	for (i = 0; i < 8; i++) {
		phase->pos[i] = MAX_CARD_PER_COL - 1;
		for (j = 0; j < MAX_CARD_PER_COL - 1; j++) {
			if (phase->column[i][j] == 0x00) {
				phase->pos[i] = j - 1;
				break;
			}
		}
	}
}

static void init_dbg_info_stream(void)
{
#if 0
	dbg_info_out = fopen("f:\\dbg.txt", "wt");
#else
	dbg_info_out = stdout;
#endif //
	assert(dbg_info_out);
}

static void close_dbg_info_stream(void)
{
	fclose(dbg_info_out);
}

static void init_game(struct game *game, struct phase *init_phase)
{
//	INIT_LIST_HEAD(&game->all_step);
	game->step_nr = 0;

	game->hash_bucket = (step**)malloc(sizeof(step *) * MAX_HASH_BUCKET);
	assert(game->hash_bucket);
	memset(game->hash_bucket, 0, sizeof(step *) * MAX_HASH_BUCKET);

	game->root_step = clone_step_from_phase(init_phase, game);
	assert(game->root_step);
}

///////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	game game1;
    phase *p = 
        //&the_19334;	// done
    	 //&the_30722;	// done
    //	struct phase *p = &the_30722;	// done
    //	struct phase *p = &the_11760;	// done
     &the_test_2;	// done
    //	struct phase *p = &the_test_3;	// done
    int n;

	////////////////////////////////////////////////////////////////

	init_dbg_info_stream();

	init_phase_pos(p);
	check_phase(p);
	//dump_phase(p);

	init_game(&game1, p);

	n = do_freecell_solve(game1);

	close_dbg_info_stream();
	return n;
}
