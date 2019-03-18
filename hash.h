#ifndef HASH_H
#define HASH_H

struct hashmap
{
	struct bucket_listnode** map;
	int num_of_elems;
	int num_of_buckets;
	int bucket_size;
	int elems_per_bucket;
};

struct bucket_listnode
{
	struct bucket_data* data;
	struct bucket_listnode* next;
};

struct bucket_data
{
	char* phone_num;
	struct cdr_listnode* head;
};


int bucket_list_insert(struct bucket_listnode**,struct cdr*,int,int);
void print_bucket_list(struct bucket_listnode*,int);
int delete_bucket_list(struct bucket_listnode*,int,int);
int hash_insert(struct hashmap* ,char* ,struct cdr*,int);
int init_hashmap(struct hashmap**,int,int);
void print_hashmap(struct hashmap*);
int delete_from_hash(struct hashmap*,char*,char*);
void find_lookup(struct hashmap*, char*, char*, char*);
void topdest(struct hashmap*, char*);
int indist(struct hashmap* ,struct hashmap* ,struct indist_listnode **,char* );
void final_indist(struct hashmap *,struct hashmap *,struct indist_listnode*);
void dump(struct hashmap*,FILE*);
void dump_bucket_list(struct bucket_listnode*,int,FILE*);
int delete_hashmap(struct hashmap**,int,int);

#endif