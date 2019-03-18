#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dest_list.h"


/* a list used for topdest */
int dest_list_insert(struct dest_listnode** head, char* country_code)
{
	if((*head)==NULL)
	{
		(*head)=malloc(sizeof(struct dest_listnode));
		(*head)->country_code=malloc(sizeof(char)*strlen(country_code)+1);
		strcpy((*head)->country_code,country_code);
		(*head)->times_called=1;
		(*head)->next=NULL;
	}
	else
	{
		struct dest_listnode *temp=(*head);
		while((*head)!=NULL)
		{
			if(strcmp((*head)->country_code,country_code)==0)
			{
				(*head)->times_called++;
				break;
			}
			else 
			{
				if((*head)->next==NULL)
				{
					(*head)->next=malloc(sizeof(struct dest_listnode));
					(*head)=(*head)->next;
					(*head)->country_code=malloc(sizeof(char)*strlen(country_code)+1);
					strcpy((*head)->country_code,country_code);
					(*head)->times_called=1;
					(*head)->next=NULL;
				}
				(*head)=(*head)->next;
			}
		}
		(*head)=temp;
	}
	return 0;
}

void dest_list_print_max(struct dest_listnode* head)
{
	int max=0;
	char* country_code=NULL;
	if (head==NULL)fprintf(stderr, "No calls found\n");
	while(head!=NULL)
	{
		if(head->times_called >= max)
		{
			max=head->times_called;
			if (country_code==NULL)country_code=malloc(sizeof(char)*strlen(head->country_code)+1);
			strcpy(country_code,head->country_code);
		}
		head=head->next;
	}
	printf("Country Code: %s Calls made: %d\n",country_code,max);
	free(country_code);
}

void dest_list_delete(struct dest_listnode* head)
{
	struct dest_listnode *temp;
	while(head!=NULL)
	{
		temp=head;
		head=head->next;
		free(temp->country_code);
		free(temp);
	}
}

/* a list used for indest */

int indist_list_insert(struct indist_listnode **head , char* phone)
{
	if((*head)==NULL)
	{
		(*head)=malloc(sizeof(struct indist_listnode));
		(*head)->phone=malloc(sizeof(char)*strlen(phone)+1);
		strcpy((*head)->phone,phone);
		(*head)->next=NULL;
	}
	else
	{
		struct indist_listnode *temp=(*head);
		while((*head)!=NULL)
		{
			if(strcmp((*head)->phone,phone)==0)break;
			else 
			{
				if((*head)->next==NULL)
				{
					(*head)->next=malloc(sizeof(struct indist_listnode));
					(*head)=(*head)->next;
					(*head)->phone=malloc(sizeof(char)*strlen(phone)+1);
					strcpy((*head)->phone,phone);
					(*head)->next=NULL;
				}
				(*head)=(*head)->next;
			}
		}
		(*head)=temp;
	}
	return 0;
}

//merges the common elements of two lists into one 
int merge_indist_lists(struct indist_listnode** new,struct indist_listnode* head1,struct indist_listnode* head2)
{
	struct indist_listnode* temp=NULL;
	while(head1!=NULL)
	{
		temp=head2;
		while(temp!=NULL)
		{
			if (strcmp(head1->phone,temp->phone)==0)
			{
				indist_list_insert(&(*new),head1->phone);
				break;
			}
			else temp=temp->next;
		}
		head1=head1->next;
	}
	return 0;
}


void indist_list_delete(struct indist_listnode* head)
{
	struct indist_listnode *temp;
	while(head!=NULL)
	{
		temp=head;
		head=head->next;
		free(temp->phone);
		free(temp);
	}
}
void indist_print_list(struct indist_listnode* head)
{
	while(head!=NULL)
	{
		printf("%s\n",head->phone);
		head=head->next;
	}
}