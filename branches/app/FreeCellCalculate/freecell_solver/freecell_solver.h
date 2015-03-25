#pragma once



#define CARD_SPADE		1
#define CARD_HEART		2
#define CARD_CLUB		3
#define CARD_DIAMOND	4

#define MAX_CARD_PER_COL	20

#define ST_DIE		0x0001
#define ST_TRY		0x0002
#define ST_DONE		0x0004
#define ST_GOOD		0x0008
#define ST_DUP		0x0010
#define ST_INIT		0x0020
#define ST_STEP		0x0040
#define ST_WIN		0x0080

#define MAX_HASH_BUCKET		(1024*1024)


class phase 
{
public:
    char space[4];
    char final[4];
    char pos[8];
    char column[8][MAX_CARD_PER_COL];
};

class step 
{
public:
    phase phase;

    struct list_head sub;
    struct list_head sub_list_head;
    //	struct list_head list;

    int id;
    step *up_step;
    int level;

    step *hash_next;
    u32 hash;

    int state;
};

class game 
{
public:
    step *root_step;

    //	struct list_head all_step;
    int step_nr;

    step **hash_bucket;

    //	struct list_head death_list;
    //	struct list_head dup_list;
};

////////////////////////////////////////////////////////////////

#define make_card(t, d)		((char)(((t) & 0x0f) << 4 | ((d) & 0x0f)))

#define assert_card(c) 	assert(get_type(c) >= 1 && get_type(c) <= 4 && get_digital(c) >= 1 && get_digital(c) <= 0x0d)


#define for_each_space_card(card, phase) \
    for(;;)
#define for_each_not_null_column(phase) \
    for(;;)



//////////////////////////

int do_freecell_solve(game& rgame);

int main(int argc, char* argv[]);

static inline void on_done_callback(void);

static void card_to_string(char *buf, char card);

static void dump_step(const step *step);









