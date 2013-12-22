#ifndef _LIST_H_
#define _LIST_H_


struct list_head {
	struct list_head *prev, *next;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)	do { \
		(ptr)->prev = (ptr); (ptr)->next = (ptr); \
	} while (0)

/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	(type *)((char *)ptr - (char *)(&((type *)0)->member))

/**
 * list_for_each        -       iterate over a list
 * @pos:        the &struct list_head to use as a loop counter.
 * @head:       the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
		pos = pos->next)

/**
 * list_for_each_prev   -       iterate over a list backwards
 * @pos:        the &struct list_head to use as a loop counter.
 * @head:       the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
		pos = pos->prev)

/**
* list_for_each_safe   -       iterate over a list safe against removal of list entry
* @pos:        the &struct list_head to use as a loop counter.
* @n:          another &struct list_head to use as temporary storage
* @head:       the head for your list.
*/
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
* list_for_each_entry  -       iterate over list of given type
* @pos:        the type * to use as a loop counter.
* @head:       the head for your list.
* @member:     the name of the list_struct within the struct.
*/
#define list_for_each_entry(pos, head, type, member)						\
	for (pos = list_entry((head)->next, type, member);		\
		&pos->member != (head);										\
		pos = list_entry(pos->member.next, type, member))

/**
* list_for_each_entry_reverse - iterate backwards over list of given type.
* @pos:        the type * to use as a loop counter.
* @head:       the head for your list.
* @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, type, member)				\
	for (pos = list_entry((head)->prev, type, member);		\
		&pos->member != (head);										\
		pos = list_entry(pos->member.prev, type, member))

 /**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:        the type * to use as a loop counter.
 * @n:          another type * to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
*/
#define list_for_each_entry_safe(pos, n, head, type, member)	\
	for (pos = list_entry((head)->next, type, member),			\
			n = list_entry(pos->member.next, type, member);		\
		&pos->member != (head);									\
		pos = n, n = list_entry(n->member.next, type, member))



/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
extern void __list_add(struct list_head *node, struct list_head *prev, struct list_head *next);

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
extern void list_add(struct list_head *node, struct list_head *head);

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
extern void list_add_tail(struct list_head *node, struct list_head *head);

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
extern void __list_del(struct list_head *prev, struct list_head *next);

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
extern void list_del(struct list_head *entry);

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
extern void list_del_init(struct list_head *entry);

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
extern void list_move(struct list_head *list, struct list_head *head);

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
extern void list_move_tail(struct list_head *list, struct list_head *head);

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
extern int list_empty(const struct list_head *head);

/**
 * list_empty_careful - tests whether a list is
 * empty _and_ checks that no other CPU might be
 * in the process of still modifying either member
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 *
 * @head: the list to test.
 */
extern int list_empty_careful(struct list_head *head);

extern void __list_splice(struct list_head *list, struct list_head *head);

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
extern void list_splice(struct list_head *list, struct list_head *head);

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
extern void list_splice_init(struct list_head *list, struct list_head *head);

#endif // _LIST_H_

