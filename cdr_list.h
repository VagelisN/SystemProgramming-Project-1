#ifndef CDRLIST_H
#define CDRLIST_H
#define LISTELEMS 10

struct cdr_listnode
{
	struct cdr **cdr_array;
	int next_insertion;
	struct cdr_listnode* next;
};

int cdr_list_insert(struct cdr_listnode **, struct cdr*);
void cdr_printlist(struct cdr_listnode*);
int delete_cdr_list_item(struct cdr_listnode** ,char*);
void list_lookup_find(struct cdr_listnode*,char*, char*, int);
void list_topdest(struct cdr_listnode*);
int list_indist(struct cdr_listnode* ,struct indist_listnode **,int);
int check_for_indist(struct cdr_listnode* head,struct indist_listnode* inhead,int orig_or_dest);
void cdr_dumplist(struct cdr_listnode*,FILE*);
void cdr_deletelist(struct cdr_listnode*,int);

#endif