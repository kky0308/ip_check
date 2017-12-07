#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipcheck.h"
void ip_check(struct check_args *pf,int cnt,int *data)
{
	int j=1;
	for(int i=0;i<cnt-1;i++)
	{
		for(int k=i+1;k<cnt;k++)
		{
			if(strcmp((pf+i)->ip,(pf+k)->ip)==0)
			{
				data[2*j-1]=(pf+i)->socket_id;
				data[2*j]=(pf+k)->socket_id;
				printf("부정행위 발견\n");
				printf("%s 실격\n",(pf+i)->name);
				printf("%s 실격\n",(pf+k)->name);
				j=j+2;
				data[0]=j-1;
			}
		}
	}
	
	
}
