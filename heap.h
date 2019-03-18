#ifndef HEAP_H
#define HEAP_H

struct heap_node
{
	char* phone;
	float amount;
	struct heap_node* left;
	struct heap_node* right;
	struct heap_node* parent;
};

struct aux_heap_node
{
	float amount;
	struct heap_node* pointer;
	struct aux_heap_node* left;
	struct aux_heap_node* right;
	struct aux_heap_node* parent;
};

int heap_insert(struct heap_node** , char* ,float,int*);
void max_heapify(struct heap_node*);
void print_heap(struct heap_node*);
int heap_find(struct heap_node*,char*,float);
void top_kappa(struct heap_node*,float);
void delete_heap(struct heap_node*);

int aux_heap_insert(struct aux_heap_node**,struct heap_node*,float,int*);
void aux_max_heapify(struct aux_heap_node*);
struct heap_node* aux_delete_root(struct aux_heap_node**,int*);
void delete_aux_heap(struct aux_heap_node*);


#endif