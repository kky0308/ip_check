#ifndef IPCHECK_H
#define IPCHHCK_H

struct check_args{
	char * ip;
	char * name;
	int socket_id;
};

void ip_check(struct check_args *data,int cnt,int *re_Data);

#endif
