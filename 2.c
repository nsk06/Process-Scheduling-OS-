#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct booth booth;
typedef struct evm evm;
typedef struct voter voter;
void * initiate_voters(void * params);
void * initiate_evms(void * params);

typedef struct booth
{
	int b_id;
	int nvoters,dvoters,rvoters;
	int nevms;
	pthread_mutex_t lock;
	pthread_cond_t votecond,aftercond;
	voter *bvoters;
	evm *bevms;

}booth;
typedef struct evm
{
	int ev_id;
	int nslots;
	pthread_t evt;
	booth * bt;
	int echeck;	
}evm;
typedef struct voter
{
	int v_id;
	int cur_state;
	pthread_t vt;
	booth * bt;
	evm * ev;	
}voter;
void voter_in_slot(booth *bth,evm *em,voter * V)
{
	pthread_mutex_t * lockptr = &(bth->lock);
	pthread_cond_t * afcond_checkpt = &(bth->aftercond);
	pthread_cond_t * votecond_checkpt = &(bth->votecond);
 	pthread_mutex_lock(lockptr);
 	while(em->echeck == 0)
 	{
 		pthread_cond_wait(votecond_checkpt, lockptr);
 	}
 	em->nslots-=1;
 	printf("Voter %d at Booth %d has casted its vote at Evm %d\n", V->v_id,em->bt->b_id, em->ev_id);
 	pthread_mutex_unlock(lockptr);
 	pthread_cond_broadcast(afcond_checkpt);
}
int polling_ready_evm( booth *b, int count,evm * em)
{
	booth * B = em->bt;
	pthread_mutex_t * lockptr = &(B->lock);
	int x = 0;
	pthread_cond_t* votecond_checkpt = &(B->votecond);
	pthread_cond_t * afcond_checkpt = &(B->aftercond);
	while(x< count)
		{
			int randt = B->nvoters;
			int var = 0;
			int i = (rand() % randt)+1;
			pthread_mutex_lock(lockptr);
			int k=0;
			if(B->bvoters[i].cur_state == 1)
			{
				B->bvoters[i].cur_state = 2;
				B->bvoters[i].ev = em;
				k=1;
				B->dvoters+=k;
				B->rvoters -= k;
				printf("Voter %d at Booth %d has been allocated with EVM %d\n", i,B->b_id, em->ev_id);
				x++;
			}
			if(B->dvoters == B->nvoters || B->rvoters == 0)
			{
				pthread_mutex_unlock(lockptr);
				break;
			}
			pthread_mutex_unlock(lockptr);
			var = 1;
		}
		return x;
}
void voter_wait_for_evm(booth *bth,voter *V)
{
	pthread_mutex_t * lockptr = &(bth->lock);
	int var = 0;
	pthread_cond_t * afcond_checkpt = &(bth->aftercond);
	pthread_cond_t * votecond_checkpt = &(bth->votecond);
	pthread_mutex_lock(lockptr);
	V->cur_state = 1;
	while(V->cur_state == 1) 
	{
		pthread_cond_wait(votecond_checkpt, lockptr);
	}
	pthread_mutex_unlock(lockptr);
	var = 1;
}

void * initiate_booths(void* params)
{
	//printf("in here\n");
	booth * B = (booth*)params;
	int i=1;
	while(i <= B->nvoters)
	{
		// if(i > (B->nvoters))
		// 	break;
		int var=0;
		var = 1;
		B->bvoters[i].v_id = i;
		B->bvoters[i].bt = B;
		B->bvoters[i].ev = NULL;
		B->bvoters[i].cur_state = 3;
		i++;
	}
	//printf("dekhey kha hai\n");
	i=1;
	while(i <= B->nevms)
	{
		int var=0;
		var =1;
		B->bevms[i].nslots = 0;
		B->bevms[i].echeck = 0;
		B->bevms[i].ev_id = i;
		B->bevms[i].bt = B;
		i++;
	}
	i=1;
	while(i <= B->nvoters)
	{
		int c = pthread_create(&(B->bvoters[i].vt),NULL, initiate_voters,&(B->bvoters[i]));
		if(c)
		{
			perror("Error in creating Voter thread");
		}
		i++;
	}
	//printf("yoooo\n");
	i=1;
	while(i <= (B->nevms))
	{
		int c = pthread_create(&(B->bevms[i].evt),NULL, initiate_evms,&(B->bevms[i]));
		if(c)
		{
			perror("Error in creating evm thread");
		}
		i++;
	}
	//printf("Tata\n");
	for(int i=1; i<=B->nvoters; i++)
	{
		pthread_join(B->bvoters[i].vt,NULL);
	}
	//printf("check2\n");
	for(int i=1; i<=B->nevms; i++)
	{
		pthread_join(B->bevms[i].evt, NULL);
	}
	//printf("check1\n");
	printf(ANSI_COLOR_RED "Voters at Booth Number %d have finished voting.\n" ANSI_COLOR_RESET, B->b_id);
}

void * initiate_voters(void * params)
{
	voter * Vo = (voter*)params;
	booth * b = Vo->bt;
	voter_wait_for_evm(b,Vo);
	evm * e = Vo->ev;
	voter_in_slot(b,e,Vo);
}

void * initiate_evms(void * params)
{
	evm * EV = (evm *)params;
	booth * B = EV->bt;
	pthread_mutex_t * lockptr = &(B->lock);
	pthread_cond_t * afcond_checkpt = &(B->aftercond);
	pthread_cond_t * votecond_checkpt = &(B->votecond);

	while(1)
	{
		pthread_mutex_lock(lockptr);
		if(B->dvoters == B->nvoters || B->rvoters == 0)
		{
			pthread_mutex_unlock(lockptr);
			break;
		}
		pthread_mutex_unlock(lockptr);
		int x = 0;
		int evmslots = rand()%10 + 1;
		pthread_mutex_lock(lockptr);
		EV->echeck = 0;
		EV->nslots = evmslots;
		pthread_mutex_unlock(lockptr);
		printf("Evm %d at Booth  number %d  is idle with slots = %d\n", EV->ev_id ,B->b_id, evmslots);

		int o = polling_ready_evm(B,evmslots,EV);

		if(o == 0)
		{
			break;
		}
		printf("Evm %d at Booth number %d is shifting to voting phase.\n",  EV->ev_id,B->b_id);

		pthread_mutex_lock(lockptr);
		EV->nslots = o;
		EV->echeck = 1;
		pthread_cond_broadcast(votecond_checkpt);
		while(EV->nslots)
		{
			//printf("pta nhi\n");
			pthread_cond_wait(afcond_checkpt, lockptr);
		}
		pthread_mutex_unlock(lockptr);
		printf("Evm %d at Booth number %d is done with voting.\n",  EV->ev_id,B->b_id);
	}
}

int main()
{
	booth b[4000];
	int numberbooths;
	printf("Enter the number of Booths\n");
	scanf("%d",&numberbooths);
	pthread_t Booths[numberbooths];
	srand(time(0));
	printf("Enter details for each booth\n");
	for(int i=1;i<=numberbooths;i++)
	{
		int numvoters,numevms;
		scanf("%d %d",&numvoters,&numevms);
		b[i].b_id = i;
		b[i].nvoters = numvoters;
		b[i].nevms = numevms;
		b[i].dvoters = 0;
		b[i].rvoters = numvoters;
		//voter x[numvoters+5];
		//b[i].bvoters = x;
		//memcpy(b[i].bvoters,x,sizeof(x));
		//evm y[numevms+5];
		//b[i].bevms = y;
		//memcpy(b[i].bevms,y,sizeof(y));
		b[i].bvoters = (voter*)malloc(sizeof(voter)*(numvoters+1));
		b[i].bevms = (evm*)malloc(sizeof(evm)*(numevms+1));
		pthread_mutex_init(&(b[i].lock), NULL);
		pthread_cond_init(&(b[i].votecond), NULL);
		pthread_cond_init(&(b[i].aftercond), NULL);
	}
	printf("ELECTION Begins.\n");
	for(int i=1; i<=numberbooths; i++)
	{
		int c = pthread_create(&(Booths[i]), NULL, initiate_booths, &(b[i]));
		if(c)
		{
			perror("Error in creating booth thread");
		}
	}
	//printf("heya\n");
	for(int i=1; i<=numberbooths; i++)
	{
		pthread_join(Booths[i],NULL);
	}
	printf("ELECTION finished\n");
	return 0;
}