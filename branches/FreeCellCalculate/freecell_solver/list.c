#include <stdio.h>
#include "list.h"

void __list_add(struct list_head *node, struct list_head *prev, struct list_head *next)
{
	prev->next = node;
	node->next = next;
	node->prev = prev;
	next->prev = node;
}

void list_add(struct list_head *node, struct list_head *head)
{
	__list_add(node, head, head->next);
}

void list_add_tail(struct list_head *node, struct list_head *head)
{
	__list_add(node, head->prev, head);
}

void __list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->prev = NULL;
	entry->next = NULL;
}

void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

void list_move_tail(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

int list_empty(const struct list_head *head)
{
	return head->next == head;
}

int list_empty_careful(struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

void __list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

void list_splice_init(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
	}
}

