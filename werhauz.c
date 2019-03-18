#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cdr.h"
#include "dest_list.h"
#include "cdr_list.h"
#include "hash.h"
#include "heap.h"


int main(int argc, char **argv)
{
	if (argc!= 11 &&argc != 9){printf("wrong number of args given\n");exit(1);}
	struct hashmap *hash_orig, *hash_dest;
	struct cdr* temp_cdr;
	struct heap_node* heap=NULL;
	int i,operations_file=0,hash1_entries,hash2_entries,bucket_size,heap_nodes=0,k,l;
	float cost0,cost11,cost12,cost21,cost22,temp_cost,total_cost=0;
	size_t len;
	char* line=NULL,*token=NULL;
	FILE *fp=NULL,*tarrif_cfg=NULL;
	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i],"-o")==0)
		{
			operations_file=1;
			fp=fopen(argv[i+1],"r");
			if (fp==NULL){printf("cannot open operationsfile\n");exit(1);}
		}
		else if (strcmp(argv[i],"-h1")==0)
		{
			hash1_entries=atoi(argv[i+1]);
			if (hash1_entries <= 0){printf("hash entries have to be > 0");exit(1);}
		}
		else if (strcmp(argv[i],"-h2")==0)
		{
			hash2_entries=atoi(argv[i+1]);
			if (hash2_entries <= 0){printf("hash entries have to be > 0");exit(1);}
		}
		else if (strcmp(argv[i],"-s")==0)
		{
			bucket_size=atoi(argv[i+1]);
		}
		else if (strcmp(argv[i],"-c")==0)
		{
			tarrif_cfg=fopen(argv[i+1],"r");
			if (tarrif_cfg==NULL){printf("cannot open tarrif configuration file\n");exit(1);}
			while((fgetc(tarrif_cfg))!='\n');
			fscanf(tarrif_cfg,"%d;%d;%f",&k,&l,&cost0);
			fscanf(tarrif_cfg,"%d;%d;%f",&k,&l,&cost11);
			fscanf(tarrif_cfg,"%d;%d;%f",&k,&l,&cost12);
			fscanf(tarrif_cfg,"%d;%d;%f",&k,&l,&cost21);
			fscanf(tarrif_cfg,"%d;%d;%f",&k,&l,&cost22);
			fclose(tarrif_cfg);
		}
		else printf("unknown argument flag given\n flags to use: -h1 -h2 -s -c \noptional flag: -o\n");		
	}
	if(init_hashmap(&hash_orig,hash1_entries,bucket_size)!=0){printf("error initializing the hashmap\n");exit(1);}
	if(init_hashmap(&hash_dest,hash2_entries,bucket_size)!=0){printf("error initializing the hashmap\n");exit(1);}
	if (operations_file==0)fp=stdin;
	while(1)
	{
		if(getline(&line, &len, fp)== -1)
		{
			fclose(fp);
			fp=stdin;
		}
		else
		{
			if(line[strlen(line)-1]=='\n')line[strlen(line)-1]='\0';
			if(fp!=stdin)printf("%s\n",line);
			token=strtok(line," ");
			if (strcmp(token,"insert")==0)
			{
				token=strtok(NULL," ");
				temp_cdr=create_cdr(token);
				if(hash_insert(hash_orig,temp_cdr->originator_number,temp_cdr,0)!=0)fprintf(stderr, "Ierror\n");
				if(hash_insert(hash_dest,temp_cdr->destination_number,temp_cdr,1)!=0)fprintf(stderr, "Ierror\n");
				if(temp_cdr->fault_condition<=299 &&temp_cdr->fault_condition>=200)
				{
					if (temp_cdr->type==0)temp_cost=cost0;
					else if(temp_cdr->type==1&&temp_cdr->tarrif==1)temp_cost=cost11*temp_cdr->duration;
					else if(temp_cdr->type==1&&temp_cdr->tarrif==2)temp_cost=cost12*temp_cdr->duration;
					else if(temp_cdr->type==2&&temp_cdr->tarrif==1)temp_cost=cost21*temp_cdr->duration;
					else if(temp_cdr->type==2&&temp_cdr->tarrif==2)temp_cost=cost22*temp_cdr->duration;
					total_cost+=temp_cost;
					if(heap_insert(&heap,temp_cdr->originator_number,temp_cost,&heap_nodes)!=0)fprintf(stderr,"Ieroor\n");
				}
			}
			else if(strcmp(token,"delete")==0)
			{
				char* caller=strtok(NULL," ");
				char* uniq_id=strtok(NULL," ");
				if(delete_from_hash(hash_orig,caller,uniq_id)==1)fprintf(stderr,"Derror\n");
				else printf("Deleted %s\n",uniq_id);
				printf("\n");
			}
			else if(strcmp(token,"find")==0 ||strcmp(token,"lookup")==0)
			{
				struct hashmap* temp;
				if(strcmp(token,"find")==0 )temp=hash_orig;
				else temp=hash_dest;
				char* phone=strtok(NULL," ");
				char* str=strtok(NULL,";");
				if(str==NULL)find_lookup(temp,phone,NULL,NULL);//no time or year was given
				else
				{
					char str1[10]="k",str2[10]="k",str3[10]="k",str4[10]="k";
					sscanf(str,"%s %s %s %s",str1,str2,str3,str4);
					if(strlen(str1)==strlen(str2))find_lookup(temp,phone,str1,str2);//only time or only date
					else//all four values are given
					{
						char buff1[50],buff2[50];
						sprintf(buff1,"%s %s",str1,str2);
						sprintf(buff2,"%s %s",str3,str4);
						find_lookup(temp,phone,buff1,buff2);
					}
				}
				printf("\n");
			}
			else if(strcmp(token,"indist")==0)
			{
				char *caller1 =strtok(NULL," ");
				char *caller2 =strtok(NULL," ");
				struct indist_listnode* inhead1=NULL,*inhead2=NULL,*merged=NULL;
				indist(hash_orig ,hash_dest,&inhead1,caller1);
				indist(hash_orig,hash_dest,&inhead2,caller2);
				merge_indist_lists(&merged,inhead1,inhead2);
				final_indist(hash_orig,hash_dest,merged);
				indist_list_delete(inhead1);
				indist_list_delete(inhead2);
				indist_list_delete(merged);
				printf("\n");
			}
			else if(strcmp(token,"topdest")==0)
			{
				char* caller=strtok(NULL," ");
				topdest(hash_orig,caller);
				printf("\n");
			}
			else if(strcmp(token,"top")==0)
			{
				float topk=(total_cost*atof(strtok(NULL," ")))/100;
				printf("total_cost: %.2f percentage asked: %.2f\n",total_cost , topk );
				top_kappa(heap,topk);
				printf("\n");
			}
			else if(strcmp(token,"bye")==0)
			{
				print_heap(heap);
				delete_hashmap(&hash_orig,0,1);
				delete_hashmap(&hash_dest,1,1);
				delete_heap(heap);
				heap=NULL;
				heap_nodes=0;
				printf("\n");
			}
			else if(strcmp(token,"print")==0)
			{
				if (strcmp(strtok(NULL," "),"hashtable1")==0)print_hashmap(hash_orig);
				else print_hashmap(hash_dest);
				printf("\n");
			}
			else if(strcmp(token,"dump")==0)
			{
				char* hashname= strtok(NULL," ");
				FILE* fp;
				if((fp=fopen(strtok(NULL," "),"w"))==NULL)printf("could not open dump file\n");
				else
				{
					if(strcmp(hashname,"hashtable1"))dump(hash_orig,fp);
					else dump(hash_dest,fp);
				}
				printf("\n");
			}
			else if(strcmp(token,"exit")==0)
			{
				delete_hashmap(&hash_orig,0,0);
				delete_hashmap(&hash_dest,1,0);
				delete_heap(heap);
				break;
			}
			else printf("unknown command given\n");
		}
	}
	free(line);
	fclose(fp);
	return 0;
}