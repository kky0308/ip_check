#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipcheck.h"
void ip_check(struct check_args *pf,int cnt,int *data)
{
	int j=1;
	int i,k;
	int flag=0;
	for(i=0;i<cnt+1;i++)
	{
		for(k=0;k<cnt+1;k++)
		{	
			if(i==k)
			{
				continue;
			}
			if(strcmp((pf+i)->ip,(pf+k)->ip)==0)
			{
				data[2*j-1]=(pf+i)->socket_id;
				data[2*j]=(pf+k)->socket_id;
				if(flag ==0)
				{
					printf("부정행위 발견\n");
					flag=1;
				}				
				printf("%s %s %d실격\n",(pf+k)->name,(pf+k)->ip,(pf+k)->socket_id);
				j=j+2;
				data[0]=j-1;
			}
		}
		
	}
	
	
}
