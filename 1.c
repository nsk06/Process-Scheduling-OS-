#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct pinfo
{
	int pno;
	int enter;
	int playing;
}pinfo;
typedef struct rinfo
{
	int rno;
	int enter;
	int ingame;
}rinfo;
typedef struct pass
{
	int type;
}pass;
pinfo players[20000];
rinfo refrees[10000];
int nplayers = 0;
int nreferees = 0;
int orgbusy  = 0;
pthread_mutex_t mutex;
void meetroganiser(int Playernumber,int chek)
{
	if(chek == 1)
	{
		printf("Player %d meeting with organiser\n",Playernumber);
	}
	else if(chek == 2)
	{
		printf("Refree %d meeting with organiser\n",Playernumber);
	}
}
void entercourt(int Playernumber,int chek)
{
	if(chek == 1)
	{
		printf("Player %d is in court\n",Playernumber);
	}
	else if(chek == 2)
	{
		printf("Refree %d is in court\n",Playernumber);
	}
}
void warmup(pinfo p)
{
	printf("Player %d is warming up\n",p.pno);
	sleep(1);
}
void adjustequpment(rinfo r)
{
	printf("Refree %d is adjusting the equipements for the game\n",r.rno);
	sleep(.5);
}
void startgame(rinfo r)
{
	printf("Refree %d started the game\n",r.rno);
}
void *EnterAcademy(void * data)
{
	pass *t = data;
	int type = t->type;
	if(type == 1)
	{
		printf("Player %d has entered the Academy\n",nplayers);
		int curplayer = nplayers;
		players[curplayer].enter = 1;
		if(orgbusy == 0)
		{
			meetroganiser(players[curplayer].pno,1);
		}
		if(nplayers%2 == 1)
		{
			while(players[curplayer+1].enter == 0);
			while(refrees[curplayer/2+1].enter == 0);
			while(refrees[curplayer/2 +1].ingame == 0);
			entercourt(curplayer,1);
			warmup(players[curplayer]);
			players[curplayer].playing = 1;
		}
		else
		{
			while(players[curplayer-1].enter == 0);
			while(refrees[curplayer/2].enter == 0);
			while(refrees[curplayer/2].ingame == 0);
			entercourt(curplayer,1);
			warmup(players[curplayer]);
			players[curplayer].playing = 1;
		}
	}
	else if (type == 2)
	{
		printf("Refree %d has entered the Academy\n",nreferees);
		int curref = nreferees;
		refrees[curref].enter = 1;
		if(orgbusy == 0)
		{
			meetroganiser(refrees[curref].rno,2);
		}
		while(players[2*curref-1].enter == 0);
		while(players[2*curref].enter == 0);
		pthread_mutex_lock(&mutex);
		orgbusy = 1;
		entercourt(curref,2);
		adjustequpment(refrees[curref]);
		refrees[curref].ingame = 1;
		while(players[2*curref-1].playing == 0);
		while(players[2*curref].playing == 0);
		startgame(refrees[curref]);
		pthread_mutex_unlock(&mutex);
		orgbusy = 1;
	}
}

int main()
{
	int n,i,j,remp,remr,prob;
	printf("Enter number of matches\n");
	scanf("%d",&n);
	pthread_t pl[2*n+5],ref[n+5];
	for(i=1;i<=2*n;i++)
	{
		players[i].pno = i;
		players[i].enter = 0;
		players[i].playing = 0;
	}
	for(i=1;i<=n;i++)
	{
		refrees[i].rno = i;
		refrees[i].enter = 0;
		refrees[i].ingame= 0;
	}
	while(nplayers < 2*n || nreferees < n)
	{
		//printf("fwfrwe\n");
		remr = n - nreferees;
		remp = 2*n - nplayers;
		prob = rand()%(remr + remp)+1;
		if(prob <= remp)
			{
				nplayers++;
				pass *param = malloc(sizeof(pass));
				param->type = 1;
				int c = pthread_create(&(pl[nplayers]),NULL,EnterAcademy,param);
				if(c)
				{
					printf("Failed to create person thread\n");
					exit(-1);
				}
				//pthread_join(pl[nplayers],NULL);
			}
			else if(remp < prob <= remp+remr)
			{
				nreferees++;
				pass *param = malloc(sizeof(pass));
				param->type = 2;
				int c = pthread_create(&(ref[nreferees]),NULL,EnterAcademy,param);
				if(c)
				{
					printf("Failed to create Refree thread\n");
					exit(-1);
				}
				//pthread_join(ref[nreferees],NULL);
			}
			sleep((rand()%3)+1);
	}
	//printf("Heya\n");
	for(int i=1 ;i <=2*n;i++)
	{
		pthread_join(pl[i],NULL);
	}
	//printf("byea\n");
	for(int i=1;i<=n;i++)
	{
		pthread_join(ref[i],NULL);
	}
	return 0;
}