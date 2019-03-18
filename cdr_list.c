#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cdr.h"
#include "dest_list.h"
#include "cdr_list.h"


int cdr_list_insert(struct cdr_listnode **head, struct cdr* data)//imediate insert with the use of next_insertion
{
	if((*head)==NULL)//empty list 
	{
		(*head)=malloc(sizeof(struct cdr_listnode));
		(*head)->cdr_array=malloc(LISTELEMS*sizeof(struct cdr*));
		int i;
		for (i = 0; i < LISTELEMS; i++)(*head)->cdr_array[i]=NULL;
		(*head)->next=NULL;
		(*head)->next_insertion=1;
		(*head)->cdr_array[0]=data;
	}
	else//insert to the first NULL cdr array or if the array is full go to the next node
	{
		struct cdr_listnode* temp=(*head);
		int inserted=1;
		while(inserted==1)
		{
			if((*head)->next_insertion<LISTELEMS)//if there is space in the array insert imediately 
			{
				(*head)->cdr_array[(*head)->next_insertion]=data;
				(*head)->next_insertion++;
				inserted=0;
			}
			else //else go to the next node because the array is full
			{
				if ((*head)->next==NULL)//if there is no next node allocate a new one
				{
					(*head)->next=malloc(sizeof(struct cdr_listnode));
					(*head)=(*head)->next;
					(*head)->cdr_array=malloc(LISTELEMS*sizeof(struct cdr*));
					int i;
					for (i = 0; i < LISTELEMS; i++)(*head)->cdr_array[i]=NULL;
					(*head)->next=NULL;
					(*head)->next_insertion=1;
					(*head)->cdr_array[0]=data;
					inserted=0;
				}
				else(*head)=(*head)->next;
			}
		}
		(*head)=temp;
	}
	return 0;
}

void cdr_printlist(struct cdr_listnode*head)
{
	int i;
	if(head==NULL)printf("No CDRs for this phone number\n");
	while(head!=NULL)
	{
		for (i = 0; i < LISTELEMS; i++)
			{
				if (head->cdr_array[i]!=NULL)print_cdr(head->cdr_array[i]);
			}
		i=0;
		head=head->next;
	}
}


/*makes the pointer to the cdr NULL and reforms the list so that
no gaps are left between non null pointers.if a cdr array is empty
delete the whole node*/
int delete_cdr_list_item(struct cdr_listnode** head,char* uniq_id)
{
	int found=1,i=0;
	struct cdr_listnode* temp=(*head),*prev=(*head);
	while((*head)!=NULL && found==1)//search the list for the cdr 
	{
		for (i = 0; i < LISTELEMS; i++)
		{
			if ((*head)->cdr_array[i]!=NULL && strcmp((*head)->cdr_array[i]->cdr_uniq_id,uniq_id)==0)
			{
					found=0;
					break;
			}
		}
		if(found==1)
		{
			prev=(*head);
			(*head)=(*head)->next;
		}
	}
	if(found==1){(*head)=temp;return 1;}//if not found
	(*head)->cdr_array[i]=NULL;//else make the pointer to the cdr NULL
	if(i==LISTELEMS-1)(*head)->next_insertion=LISTELEMS-1;//if the last item was deleted the next insertion is at the last position
	else//else move the rest od the cdrs one place up the array and set the next insertion correclty
	{
		while((*head)->cdr_array[i+1]!=NULL && i<LISTELEMS-1)
		{
			(*head)->cdr_array[i]=(*head)->cdr_array[i+1];
			(*head)->cdr_array[i+1]=NULL;
			i++;
		}
		(*head)->next_insertion=i;
	}
	if(i==0)//the last item in the list was deleted. delete the whole node
	{
		if(prev==(*head))//delete the first node
		{
			temp=(*head)->next;
			free((*head)->cdr_array);
			free(*head);
			(*head)=NULL;
		}
		else
		{
			prev->next=(*head)->next;
			free((*head)->cdr_array);
			free(*head);
			(*head)=NULL;
		}
	}
	(*head)=temp;
	return 0;
}

void list_lookup_find(struct cdr_listnode *head, char* timestr1, char* timestr2,int elems_per_bucket)
{
	int i,onlytime=1,found=1;
	struct tm tm;
	memset(&tm, 0, sizeof(struct tm));
	char buff[50];
	time_t cur_time,time1,time2;
	if (timestr1==NULL)
	{
		time1=0;
		time2=999999999;
	}
	else if (strlen(timestr1)==5)//time only
	{
		onlytime=0;
		strptime(timestr1,"%H:%M", &tm);
		time1=mktime(&tm);
		strptime(timestr2,"%H:%M", &tm);
		time2=mktime(&tm);
	}
	else if(strlen(timestr2)==8)//only date was given
	{
		strptime(timestr1,"%d%m%Y", &tm);
		time1=mktime(&tm);
		strptime(timestr2,"%d%m%Y", &tm);
		time2=mktime(&tm);		
	}
	else//all four were given 
	{
		strptime(timestr1,"%H:%M %d%m%Y", &tm);
		time1=mktime(&tm);
		strptime(timestr2,"%H:%M %d%m%Y", &tm);
		time2=mktime(&tm);
	}
	while(head!=NULL)//search the cdrs and if one meets the conditions print it
	{
		for (i = 0; i < LISTELEMS; i++)
		{
			if(head->cdr_array[i]!=NULL)
			{
				if(onlytime==1)
				{
					sprintf(buff,"%s %s",head->cdr_array[i]->date,head->cdr_array[i]->init_time);
					strptime(buff,"%d%m%Y %H:%M", &tm);
				}
				else
				{
					sprintf(buff,"%s",head->cdr_array[i]->init_time);
					strptime(buff,"%H:%M", &tm);
				}
				cur_time=mktime(&tm);
				if(onlytime==0)
				{
					if(difftime(time1,time2)>0)
					{
						if((difftime(cur_time,time2)<=0) || (difftime(cur_time,time1)>=0))
						{
							print_cdr(head->cdr_array[i]);
							found=0;
						}
					}
					else if((int)difftime(cur_time,time1)>=0 && (int)difftime(time2,cur_time)>=0)
					{
						print_cdr(head->cdr_array[i]);
						found=0;
					}
				}
				else if((int)difftime(cur_time,time1)>=0 && (int)difftime(time2,cur_time)>=0)
				{
					print_cdr(head->cdr_array[i]);
					found=0;
				}
			}
		}
		head=head->next;
	}
	if(found==1)fprintf(stderr,"No CDRs found\n");
}

/*makes a list of the countries that the caller has called
and the times he has called this country then prints the max value*/
void list_topdest(struct cdr_listnode* head)
{
	struct dest_listnode* temp=NULL;
	char country_buff[4];
	memset(country_buff,'\0', sizeof(country_buff));
	while(head!=NULL)
	{
		int i;
		for (i = 0; i < LISTELEMS; i++)
		{
			if (head->cdr_array[i]!=NULL)
			{
				strncpy(country_buff,head->cdr_array[i]->destination_number,3);
				dest_list_insert(&temp,country_buff);
			}
		}
		head=head->next;
	}
	dest_list_print_max(temp);
	dest_list_delete(temp);
}


/*inserts all the phones that have called the caller
or that the caller has called in a list*/
int list_indist(struct cdr_listnode* head,struct indist_listnode** inhead,int orig_or_dest)
{
	while(head!=NULL)
	{
		int i;
		for (i = 0; i < LISTELEMS; i++)
		{
			if (head->cdr_array[i]!=NULL)
			{
				if(orig_or_dest==0)indist_list_insert(&(*inhead),head->cdr_array[i]->destination_number);
				else indist_list_insert(&(*inhead),head->cdr_array[i]->originator_number);
			}

		}
		head=head->next;
	}
	return 0;
}

/*after the common elements of the indist lists of the callers have been found
this function is used to tell if the people in the lists have communicated with eachother*/
int check_for_indist(struct cdr_listnode* head,struct indist_listnode* inhead,int orig_or_dest)
{
	struct indist_listnode* temp;
	int i,found=1;
	while(head!=NULL)
	{
		for (i = 0; i < LISTELEMS; i++)
		{
			temp=inhead;
			while(temp!=NULL)
			{
				if(orig_or_dest==0)
				{
					if(head->cdr_array[i]!=NULL)
					{
						if(strcmp(head->cdr_array[i]->destination_number,temp->phone)==0)
						{
							found=0;
							return 1;
						}
					}
				}
				else
				{
					if(head->cdr_array[i]!=NULL)
					{
						if(strcmp(head->cdr_array[i]->originator_number,temp->phone)==0)
						{
							found=0;
							return 1;
						}	
					}
				}
				temp=temp->next;
			}
		}
		head=head->next;
	}
	return 0;
}

void cdr_dumplist(struct cdr_listnode*head ,FILE* fp)
{
	int i;
	if(head==NULL)printf("empty list given\n");
	while(head!=NULL)
	{
		for (i = 0; i < LISTELEMS; i++)
		{
			if (head->cdr_array[i]!=NULL)
			{
				fprintf(fp, "insert " );
				dump_cdr(head->cdr_array[i],fp);
			}
		}
		i=0;
		head=head->next;
	}
}

void cdr_deletelist(struct cdr_listnode* head, int del_or_not)
{
	struct cdr_listnode* temp;
	while(head!=NULL)
	{
		int i;
		for (i = 0; i < LISTELEMS; i++)
		{
			if (head->cdr_array[i]!=NULL)
			{
				if(del_or_not==1)delete_cdr(head->cdr_array[i]);
				head->cdr_array[i]=NULL;
			}
		}
		free(head->cdr_array);
		temp=head;
		head=head->next;
		free(temp);
	}
}