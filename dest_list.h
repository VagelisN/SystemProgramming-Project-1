#ifndef DLIST_H
#define DLIST_H

struct dest_listnode
{
	char* country_code;
	int times_called;
	struct dest_listnode* next;
};

struct indist_listnode
{
	char* phone;
	struct indist_listnode* next;
};

int dest_list_insert(struct dest_listnode** , char*);
void dest_list_delete(struct dest_listnode* );
void dest_list_print_max(struct dest_listnode*);

int indist_list_insert(struct indist_listnode** , char*);
int merge_indist_lists(struct indist_listnode**,struct indist_listnode*,struct indist_listnode*);
void indist_print_list(struct indist_listnode* );
void indist_list_delete(struct indist_listnode* );

#endif