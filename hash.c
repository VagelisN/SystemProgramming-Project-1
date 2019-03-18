#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdr.h"
#include "dest_list.h"
#include "cdr_list.h"
#include "hash.h"




int init_hashmap(struct hashmap** h_map,int num_of_buckets,int bucket_size)//allocate a struct hashmap and set the basic values
{
	if(((*h_map)=malloc(sizeof(struct hashmap)))==NULL)return 1;
	if(((*h_map)->map=malloc(num_of_buckets*sizeof(struct bucket_listnode*)))==NULL)return 1;
	int i;
	for (i = 0; i < num_of_buckets; i++)(*h_map)->map[i]=NULL;
	(*h_map)->num_of_elems=0;
	(*h_map)->num_of_buckets=num_of_buckets;
	(*h_map)->bucket_size=bucket_size;
	(*h_map)->elems_per_bucket=(bucket_size/sizeof(struct bucket_data));
	if((*h_map)->elems_per_bucket==0)(*h_map)->elems_per_bucket=1;
	return 0;
}

int hash_fun(char* key,int num_of_buckets)
{
	int place = (((int)key[strlen(key)]+(int)key[strlen(key)-1]))%num_of_buckets;
	return place;
}

int hash_insert(struct hashmap* h_map,char* key,struct cdr* tempcdr,int orig_or_dest)//finds the bucket and inserts it there
{
	int place=hash_fun(key,h_map->num_of_buckets);
	bucket_list_insert(&(h_map->map[place]),tempcdr,h_map->elems_per_bucket,orig_or_dest);
	return 0;
	
}

int bucket_list_insert(struct bucket_listnode **head,struct cdr* data,int bucket_elems,int orig_or_dest)
{
	/* scans the bucket. if there is a record for the originator or destinator number to insert,
	the cdr is inserted to his cdr list. Else it creates a record for the caller or callee and then
	it inserts the cdr to the newly created record's list */
	int i;
	if((*head)==NULL)//empty bucket_list
	{
		
		(*head)=malloc(sizeof(struct bucket_listnode));//create the bucket_listnode
		(*head)->data=malloc(bucket_elems*sizeof(struct bucket_data));
		for (i = 0; i <bucket_elems; i++)
		{
			(*head)->data[i].phone_num=NULL;
			(*head)->data[i].head=NULL;
		}
		(*head)->data[0].phone_num=malloc(strlen(data->originator_number)+1);
		if(orig_or_dest==0)strcpy((*head)->data[0].phone_num,data->originator_number);//orig_or_dest is to know if this map is the originator or the destination map		
		else strcpy((*head)->data[0].phone_num,data->destination_number);
		cdr_list_insert(&((*head)->data[0].head),data);//insert the cdr to the bucket_data pointer to cdr list
		(*head)->next=NULL;
	}
	else
	{
		i=0;
		struct bucket_listnode* temphead=(*head);
		while((*head)->data[i].phone_num!=NULL)//try to find the originator or destination number
		{
			if(orig_or_dest==0)
			{
				if(strcmp((*head)->data[i].phone_num,data->originator_number)==0)//if found insert it to its cdr list
				{
					cdr_list_insert(&((*head)->data[i].head),data);
					(*head)=temphead;
					return 0;
				}
			}
			else
			{
				if(strcmp((*head)->data[i].phone_num,data->destination_number)==0)
				{
					cdr_list_insert(&((*head)->data[i].head),data);
					(*head)=temphead;
					return 0;
				}
			}
			i++;
			if(i==bucket_elems)
			{
				if((*head)->next==NULL)
				{
					(*head)->next=malloc(sizeof(struct bucket_listnode));
					(*head)=(*head)->next;
					(*head)->data=malloc(bucket_elems*sizeof(struct bucket_data));
					int j;
					for (j = 0; j <bucket_elems; j++)
					{
						(*head)->data[j].phone_num=NULL;
						(*head)->data[j].head=NULL;
					}
				}
				else(*head)=(*head)->next;
				i=0;
			}
		}//if not found again insert a new record 
		(*head)->data[i].phone_num=malloc(strlen(data->originator_number)+1);
		if(orig_or_dest==0)strcpy((*head)->data[i].phone_num,data->originator_number);//orig_or_dest is to know if this map is the originator or the destination map		
		else strcpy((*head)->data[i].phone_num,data->destination_number);
		cdr_list_insert(&(*head)->data[i].head,data);
		(*head)->next=NULL;
		(*head)=temphead;
	}
	return 0;
}
void print_bucket_list(struct bucket_listnode* head,int elems_per_bucket)
{
	int i;
	while(head!=NULL)
	{
		for (int i = 0; i < elems_per_bucket; ++i)
		{
			if(head->data[i].phone_num!=NULL)
			{
				printf("printing %s CDRS:\n",head->data[i].phone_num );
				cdr_printlist(head->data[i].head);
				printf("\n");
			}
		}
		head=head->next;
	}
}
/*find the caller from the bucket list and then call delete_cdr_list_item to search
his cdrs and remove the cdr with uniq id if found*/
int delete_from_bucket_list(struct bucket_listnode* head,int elems_per_bucket,char* caller ,char* uniq_id)
{
	int i=0;
	while(head != NULL &&strcmp(head->data[i].phone_num,caller)!=0)//find the caller and then call delete_cdr_list_item
	{
		i++;
		if(i==elems_per_bucket)
		{
			i=0;
			head=head->next;
		}
	}
	if(head==NULL)return 1;
	else
	{
		if(delete_cdr_list_item(&(head->data[i].head),uniq_id)==1)return 1;
	}
	return 0;
}

int delete_bucket_list(struct bucket_listnode* head,int elems_per_bucket,int del_or_not)
{
	int i;
	struct bucket_listnode* temp;
	while(head!=NULL)
	{
		for (int i = 0; i < elems_per_bucket; i++)
		{
			if(head->data[i].phone_num!=NULL)
			{
				if(del_or_not==1)cdr_deletelist(head->data[i].head,1);
				else cdr_deletelist(head->data[i].head,0);
				free(head->data[i].phone_num);
				head->data[i].phone_num=NULL;
			}
		}
		temp=head;
		head=head->next;
		free(temp->data);
		free(temp);
	}
	return 0;
}

int delete_hashmap(struct hashmap** h_map,int delete_or_not,int full)
{
	int i;
	for (int i = 0; i < (*h_map)->num_of_buckets; i++)
	{
		if ((*h_map)->map[i]!=NULL)
		{	
			if(delete_or_not==1)delete_bucket_list((*h_map)->map[i],(*h_map)->elems_per_bucket,1);
			else delete_bucket_list((*h_map)->map[i],(*h_map)->elems_per_bucket,0);
			(*h_map)->map[i]=NULL;
		}
	}
	if(full==0)
	{
		free((*h_map)->map);
		free((*h_map));
	}
}



void print_hashmap(struct hashmap* h_map)
{
	int i;
	for (int i = 0; i < h_map->num_of_buckets; i++)
	{
		print_bucket_list(h_map->map[i],h_map->elems_per_bucket);
	}
}

//find the bucket listnode in which caller has hashed 
int delete_from_hash(struct hashmap*h_map,char *caller,char* uniq_id)
{
	int place =hash_fun(caller,h_map->num_of_buckets);
	if(delete_from_bucket_list(h_map->map[place],h_map->elems_per_bucket,caller,uniq_id)==1)return 1;
	else return 0;
}

//find the cdr list of the phone and call list_lookup find to check if his cdrs meet the conditions given
void find_lookup(struct hashmap* h_map, char* phone, char* time1,char* time2)
{
	if(h_map==NULL)return;
	int found=1,place;
	place=hash_fun(phone,h_map->num_of_buckets);
	struct bucket_listnode *temp=h_map->map[place];
	while(temp!=NULL&&found==1)
	{
		for (int i = 0; i < h_map->elems_per_bucket;i++)
		{
			if(temp->data[i].phone_num!=NULL && strcmp(temp->data[i].phone_num,phone)==0)
			{
				found=0;
				list_lookup_find(temp->data[i].head, time1, time2, h_map->elems_per_bucket);
				break;
			}
		}
		if(found==1)temp=temp->next;
	}
	if(found==1)printf("phone number not in the list\n");
}

//finds the callers cdr list and calls list_topdest to print the top destination called 
void topdest(struct hashmap* h_map,char*caller)
{
	int place =hash_fun(caller,h_map->num_of_buckets);
	int found=1;
	struct bucket_listnode *temp=h_map->map[place];
	while(temp!=NULL&&found==1)
	{
		for (int i = 0; i < h_map->elems_per_bucket;i++)
		{
			if(temp->data[i].phone_num!=NULL && strcmp(temp->data[i].phone_num,caller)==0)
			{
				found=0;
				list_topdest(temp->data[i].head);
				break;
			}
		}
		if(found==1)temp=temp->next;
	}
	if(found==1)printf("phone number not in the list\n");
}

//finds all the people that caller has called or the people that have called the caller using both hashmaps
int indist(struct hashmap* hash_orig,struct hashmap* hash_dest,struct indist_listnode **head,char* phone)
{
	int place_orig=hash_fun(phone,hash_orig->num_of_buckets);
	int place_dest=hash_fun(phone,hash_dest->num_of_buckets);
	struct bucket_listnode *temp=hash_orig->map[place_orig];
	int found=1,i;
	while(temp!=NULL&&found==1)
	{
		for (i = 0; i < hash_orig->elems_per_bucket;i++)
		{
			if(temp->data[i].phone_num!=NULL && strcmp(temp->data[i].phone_num,phone)==0)
			{
				found=0;
				list_indist(temp->data[i].head,&(*head),0);
				break;
			}
		}
		if(found==1)temp=temp->next;
	}
	if(found==1)printf("phone number not in the list\n");
	temp=hash_dest->map[place_dest];
	found=1;
	while(temp!=NULL&&found==1)
	{
		for (i = 0; i < hash_dest->elems_per_bucket;i++)
		{
			if(temp->data[i].phone_num!=NULL && strcmp(temp->data[i].phone_num,phone)==0)
			{
				found=0;
				list_indist(temp->data[i].head,&(*head),1);
				break;
			}
		}
		if(found==1)temp=temp->next;
	}

	return 0;
}

/* this function takes the merged list that has the common people that have
called or have been called from both caller 1 and caller 2 and calls
check for indist to check if any one from the list hash contacted someone else from the list.
if not this number is printed */ 
void final_indist(struct hashmap *hash_orig,struct hashmap *hash_dest,struct indist_listnode* merged)
{
	if(merged==NULL)return;
	int place_orig,place_dest,found=1,i,print=0,num_of_prints=0;
	struct indist_listnode* temp=merged;
	struct bucket_listnode* buck=NULL;
	if(merged!=NULL&&merged->next==NULL)
	{
		if(print==0)printf("%s\n",merged->phone);
		return;
	}
	else{
	while(merged!=NULL)
	{
		found =1;
		place_orig=hash_fun(merged->phone,hash_orig->num_of_buckets);
		place_dest=hash_fun(merged->phone,hash_dest->num_of_buckets);
		buck=hash_orig->map[place_orig];
		while(buck!=NULL && found==1)
		{
			for (i = 0; i < hash_orig->elems_per_bucket;i++)
			{
				if(buck->data[i].phone_num!=NULL)
				{
					if(strcmp(buck->data[i].phone_num,merged->phone)==0)
					{
						found=0;
						print=check_for_indist(buck->data[i].head,temp,0);
						break;
					}
				}
			}
			buck=buck->next;
		}
		if(print==0)
		{
			found=1;
			buck=hash_dest->map[place_dest];
			while(buck!=NULL && found==1)
			{
				for (i = 0; i < hash_dest->elems_per_bucket;i++)
				{
					if(buck->data[i].phone_num!=NULL )
					{
						if(strcmp(buck->data[i].phone_num,merged->phone)==0)
						{
							found=0;
							print=check_for_indist(buck->data[i].head,temp,1);
							break;
						}
					}
				}
				buck=buck->next;
			}
			if(print==0)
			{
				printf("%s\n",merged->phone);
				num_of_prints++;
			}
		}
		merged=merged->next;
	}
	if(num_of_prints==0)fprintf(stderr, "No indist found\n");
}
}

void dump(struct hashmap* h_map,FILE* fp)
{
	int i;
	for (int i = 0; i < h_map->num_of_buckets; i++)dump_bucket_list(h_map->map[i],h_map->elems_per_bucket,fp);
}
void dump_bucket_list(struct bucket_listnode* head,int elems_per_bucket,FILE* fp)
{
	int i;
	while(head!=NULL)
	{
		for (int i = 0; i < elems_per_bucket; ++i)
		{
			if(head->data[i].phone_num!=NULL)
			{
				cdr_dumplist(head->data[i].head,fp);
			}
		}
		head=head->next;
	}
}