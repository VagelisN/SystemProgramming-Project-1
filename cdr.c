#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdr.h"

struct cdr* create_cdr(char* cdrstr)//takes a string from the insert command and turns it into a cdr
{
	if (cdrstr==NULL)
	{
		printf("NULL string given to create_cdr\n");
		exit(1);
	}
	struct cdr* temp=malloc(sizeof(struct cdr));
	char *token=NULL;
	token=strtok(cdrstr,";");
	temp->cdr_uniq_id=malloc(sizeof(char)*strlen(token)+1);
	strcpy(temp->cdr_uniq_id,token);
	token=strtok(NULL,";");
	temp->originator_number=malloc(sizeof(char)*strlen(token)+1);
	strcpy(temp->originator_number,token);
	token=strtok(NULL,";");
	temp->destination_number=malloc(sizeof(char)*strlen(token)+1);
	strcpy(temp->destination_number,token);
	token=strtok(NULL,";");
	strcpy(temp->date,token);
	token=strtok(NULL,";");
	strcpy(temp->init_time,token);
	token=strtok(NULL,";");
	temp->duration=atoi(token);
	token=strtok(NULL,";");
	temp->type=atoi(token);
	token=strtok(NULL,";");
	temp->tarrif=atoi(token);
	token=strtok(NULL,";");
	temp->fault_condition=atoi(token);
	return temp;
}

void print_cdr(struct cdr* temp)
{
	printf("%s ",temp->cdr_uniq_id);
	printf("%s ",temp->originator_number);
	printf("%s ",temp->destination_number);
	printf("%s ",temp->date);
	printf("%s ",temp->init_time);
	printf("%d ",temp->duration);
	printf("%d ",temp->type);
	printf("%d ",temp->tarrif);
	printf("%d\n",temp->fault_condition);
}

void dump_cdr(struct cdr* temp,FILE* fp)
{
	fprintf(fp,"%s;",temp->cdr_uniq_id);
	fprintf(fp,"%s;",temp->originator_number);
	fprintf(fp,"%s;",temp->destination_number);
	fprintf(fp,"%s;",temp->date);
	fprintf(fp,"%s;",temp->init_time);
	fprintf(fp,"%d;",temp->duration);
	fprintf(fp,"%d;",temp->type);
	fprintf(fp,"%d;",temp->tarrif);
	fprintf(fp,"%d\n",temp->fault_condition);
}


void delete_cdr(struct cdr* temp)
{
	free(temp->cdr_uniq_id);
	free(temp->originator_number);
	free(temp->destination_number);
	free(temp);
}