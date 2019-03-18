#ifndef CDR_H
#define CDR_H

struct cdr
{
	char* cdr_uniq_id;
	char* originator_number;
	char* destination_number;
	char date[9];
	char init_time[6];
	int duration;
	short int type;
	short int tarrif;
	short int fault_condition;
};

struct cdr* create_cdr(char*);
void print_cdr(struct cdr*);
void dump_cdr(struct cdr*,FILE*);
void delete_cdr(struct cdr*);

#endif