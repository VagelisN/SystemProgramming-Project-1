#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

//IMPLEMENTATION OF A BINARY MAX HEAP AS A BINARY TREE


int heap_insert(struct heap_node** head, char* cur_phone ,float cur_amount,int *num_of_nodes)
{
	if(heap_find((*head),cur_phone,cur_amount)==1)
	{
		max_heapify(*head);
		return 0;//if the phone is already in the heap just update its value
	}
	else
	{
		if((*num_of_nodes)==0)//insert to an empty bin heap
		{
			(*head)=malloc(sizeof(struct heap_node));
			(*head)->left=NULL;
			(*head)->right=NULL;
			(*head)->parent=NULL;
			(*head)->phone=malloc(sizeof(char)*(strlen(cur_phone)+1));
			strcpy((*head)->phone,cur_phone);
			(*head)->amount=cur_amount;
			(*num_of_nodes)++;
		}
		/*To find the correct place to insert the new node see how many nodes are in the heap. 
		add one and take its binary vale. remove the first digit and then for the remaining
		digits if the digit is 0 go left else go right. At the end we have the exact node we need*/
		else
		{
			(*num_of_nodes)++;
			struct heap_node *temp_head=(*head),*temp_parent=NULL;
			int num_of_digits=0;
			int temp=*num_of_nodes;
			while(temp!=0)//find the number of digits
			{
				num_of_digits++;
				temp >>= 1;
			}
			num_of_digits-=1;
			while(num_of_digits!=1)//for all the remaining bits except for the last one
			{
				temp_parent=(*head);
				temp=*num_of_nodes>>(num_of_digits-1);
				if ((temp&1)==0)(*head)=(*head)->left;//if the bit is equal to 0 go left
				else (*head)=(*head)->right;//if its equal to 1 go right
				num_of_digits--;
			}
			temp_parent=(*head);
			temp=*num_of_nodes>>(num_of_digits-1);
			if ((temp&1)==0)//for the last bit we have to create the node either left or right depending on its value
			{
				if(((*head)->left=malloc(sizeof(struct heap_node)))==NULL)return 1;
				(*head)=(*head)->left;
			}
			else 
			{
				if(((*head)->right=malloc(sizeof(struct heap_node)))==NULL)return 1;
				(*head)=(*head)->right;
			}
			(*head)->left=NULL;
			(*head)->right=NULL;
			(*head)->parent=temp_parent;
			if(((*head)->phone=malloc(sizeof(char)*(strlen(cur_phone)+1)))==NULL)return 1;
			strcpy((*head)->phone,cur_phone);
			(*head)->amount=cur_amount;
			(*head)=temp_head;
		}
	}
	max_heapify((*head));//heapify after the insertion 
	return 0;
}

void max_heapify(struct heap_node* head)
{
	if(head==NULL)return ;
	max_heapify(head->left);//recursive calls for both left and right child
	max_heapify(head->right);
	if ((head->parent !=NULL) && (head->amount > head->parent->amount))//swap the child with the parent if the childs value is greater 
	{
		float temp_amount=head->amount;
		char* temp_phone=malloc(sizeof(char)*(strlen(head->phone)+1));
		strcpy(temp_phone,head->phone);
		head->amount=head->parent->amount;
		strcpy(head->phone,head->parent->phone);
		head->parent->amount=temp_amount;
		strcpy(head->parent->phone,temp_phone);
		free(temp_phone);
	}
	return;
}

void print_heap(struct heap_node* head)//recursive print
{
	if(head==NULL)return;
	printf("%s %.2f\n",head->phone,head->amount);
	print_heap(head->left);
	print_heap(head->right);
}

int heap_find(struct heap_node* head,char* phone,float amount)// returns 1 if it is found 
{
	if (head==NULL)return 0;
	if (strcmp(head->phone,phone)==0)
	{
		head->amount+=amount;
		return 1;
	}
	if(heap_find(head->left,phone,amount)==1)return 1;
	if(heap_find(head->right,phone,amount)==1)return 1;
	else return 0;
}

float find_top_max(struct heap_node* head,float prev_max)
{
	if(head==NULL)return -1;
	float left=find_top_max(head->left,prev_max);
	float right=find_top_max(head->right,prev_max);
	if (head->amount < prev_max && head->amount>left && head->amount>right)return head->amount;
	else if(left<= prev_max &&left >right) return left;
	else if(right<=prev_max && right> left)return right;
	else return -1;
}

void top_kappa(struct heap_node* head,float topk)
{
	if(head==NULL)return;
	struct aux_heap_node* aux=NULL;
	struct heap_node* temp=NULL;
	int num_of_nodes=0;
	printf("%s %.2f\n",head->phone,head->amount);
	if(head->left!=NULL)aux_heap_insert(&aux,head->left,head->left->amount,&num_of_nodes);
	if(head->right!=NULL)aux_heap_insert(&aux,head->right,head->right->amount,&num_of_nodes);
	topk-=head->amount;
	while(topk>0)
	{
		if(aux!=NULL)
		temp=aux_delete_root(&aux,&num_of_nodes);
		topk-=temp->amount;
		if(temp->left!=NULL)aux_heap_insert(&aux,temp->left,temp->left->amount,&num_of_nodes);
		if(temp->right!=NULL)aux_heap_insert(&aux,temp->right,temp->right->amount,&num_of_nodes);
	}
	delete_aux_heap(aux);
}

void delete_heap(struct heap_node* head)//recursively delete all the heap nodes 
{
	if(head==NULL)return;
	delete_heap(head->left);
	delete_heap(head->right);
	free(head->phone);
	free(head);
	head=NULL;
} 


/* for top k an auxiliary heap is created.
the root element of main heap is printed as it is the top customer and its childen are added to the aux heap
while topk hasnt been surpassed print the root delete it and add its children to the aux heap*/
int aux_heap_insert(struct aux_heap_node** head,struct heap_node *pointer,float cur_amount,int *num_of_nodes)
{
	
		if(pointer==NULL)return -1;
		if((*num_of_nodes)==0)//insert to an empty bin heap
		{
			(*head)=malloc(sizeof(struct aux_heap_node));
			(*head)->left=NULL;
			(*head)->right=NULL;
			(*head)->parent=NULL;
			(*head)->amount=cur_amount;
			(*head)->pointer=pointer;
			(*num_of_nodes)++;
		}
		/*To find the correct place to insert the new node see how many nodes are in the heap. 
		add one and take its binary value. remove the first digit and then for the remaining
		digits if the digit is 0 go left else go right. At the end we have the exact node we need*/
		else
		{
			(*num_of_nodes)++;
			struct aux_heap_node *temp_head=(*head),*temp_parent=NULL;
			int num_of_digits=0;
			int temp=*num_of_nodes;
			while(temp!=0)//find the number of digits
			{
				num_of_digits++;
				temp >>= 1;
			}
			num_of_digits-=1;
			while(num_of_digits!=1)//for all the remaining bits except for the last one
			{
				temp_parent=(*head);
				temp=*num_of_nodes>>(num_of_digits-1);
				if ((temp&1)==0)(*head)=(*head)->left;//if the bit is equal to 0 go left
				else (*head)=(*head)->right;//if its equal to 1 go right
				num_of_digits--;
			}
			temp_parent=(*head);
			temp=*num_of_nodes>>(num_of_digits-1);
			if ((temp&1)==0)//for the last bit we have to create the node either left or right depending on its value
			{
				if(((*head)->left=malloc(sizeof(struct aux_heap_node)))==NULL)return 1;
				(*head)=(*head)->left;
			}
			else 
			{
				if(((*head)->right=malloc(sizeof(struct aux_heap_node)))==NULL)return 1;
				(*head)=(*head)->right;
			}
			(*head)->left=NULL;
			(*head)->right=NULL;
			(*head)->parent=temp_parent;
			(*head)->amount=cur_amount;
			(*head)->pointer=pointer;
			(*head)=temp_head;
			aux_max_heapify((*head));//heapify after the insertion 
		}
		return 0;
}

void aux_max_heapify(struct aux_heap_node* head)
{
	if(head==NULL)return ;
	aux_max_heapify(head->left);//recursive calls for both left and right child
	aux_max_heapify(head->right);
	if ((head->parent !=NULL) && (head->amount > head->parent->amount))//swap the child with the parent if the childs value is greater 
	{
		float temp_amount=head->amount;
		struct heap_node* temp_pointer=head->pointer;
		head->amount=head->parent->amount;
		head->pointer=head->parent->pointer;
		head->parent->amount=temp_amount;
		head->parent->pointer=temp_pointer;
	}
	return;
}

/* deletes the root and re heapifies the heap*/
struct heap_node* aux_delete_root(struct aux_heap_node** head,int* num_of_nodes)
{
	if(head!=NULL)printf("%s %.2f\n",(*head)->pointer->phone,(*head)->amount);
	if(*num_of_nodes==1)
	{
		struct heap_node* retval=(*head)->pointer;//retval is a pointer to the original heap node and is used to add its children to the aux heap
		free((*head));
		(*head)=NULL;
		return retval;
	}
	
	struct heap_node* retval=(*head)->pointer;
	struct aux_heap_node* temproot=(*head);
	struct aux_heap_node *temp_head=(*head);
	int num_of_digits=0;
	int temp=*num_of_nodes;
	while(temp!=0)//find the number of digits
	{
		num_of_digits++;
		temp >>= 1;
	}
	num_of_digits-=1;
	while(num_of_digits >1)//for all the remaining bits except for the last one
	{
		temp=*num_of_nodes>>(num_of_digits-1);
		if ((temp&1)==0)(*head)=(*head)->left;//if the bit is equal to 0 go left
		else (*head)=(*head)->right;//if its equal to 1 go right
		num_of_digits--;
	}
	temp=*num_of_nodes>>(num_of_digits-1);
	if(num_of_digits!=0)
	{
		if ((temp&1)==0)//for the last bit we have to change the root node with the left or right child of the current node
		{
			temproot->amount=(*head)->left->amount;
			temproot->pointer=(*head)->left->pointer;
			free((*head)->left);
			(*head)->left=NULL;
		}
		else 
		{
			temproot->amount=(*head)->right->amount;
			temproot->pointer=(*head)->right->pointer;
			free((*head)->right);
			(*head)->right=NULL;
		}
		(*num_of_nodes)--;
		(*head)=temproot;
		aux_max_heapify((*head));//heapify again after the root has been deleted
	}
	return retval;
}


void delete_aux_heap(struct aux_heap_node* head)//recursively delete all the heap nodes 
{
	if(head==NULL)return;
	delete_aux_heap(head->left);
	delete_aux_heap(head->right);
	free(head);
} 