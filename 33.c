#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <pthread.h>

int arr[100000];
int *myarr;
pthread_mutex_t lock;
typedef struct params
{
	int startind;
	int endind;
}params;

void swap(int *x, int *y) 
{ 
    int temp = *x; 
    *x = *y; 
    *y = temp; 
} 
void selectionSort(int s) 
{ 
    int i, j, ind; 
    for (i = 0; i < s-1; i++) 
    { 
        ind = i; 
        for (j = i+1; j < s; j++) 
        {
          if (myarr[j] < myarr[ind])
          { 
            ind = j;
          } 
  		}
        swap(&myarr[ind], &myarr[i]); 
    } 
} 
void merge(int st, int mi, int end) 
{ 
	
	int mid = mi+1;
	int k=1;
	int ind = st;
	int i = st;
	int j = mid;
	while(i<=mi && j<=end)
	{
		if(myarr[i] <= myarr[j])
		{
			arr[k] = arr[i];
			k++;
			i++;
		}
		else
		{
			arr[k] = myarr[j];
			k++;
			j++;
		}
	}
	while(i<=mi)
	{
		arr[k] = myarr[i];
		k++;
		i++;
	}
	while(j <=end)
	{
		arr[k] = myarr[j];
		k++;
		j++;
	}
	for(int i=1;i<k;i++)
	{
		myarr[ind+i-1] = arr[i];
	}
} 


void *sort(void *data) 
{ 
	params *myparams = data;
	int startind = myparams->startind;
	int endind = myparams->endind;
	int i, size=endind-startind+1;
	if (startind >= endind) 
	{  
		return NULL;
	}  
	else if(endind>startind)
	{
		int m = (startind+endind)/2;
		int size = endind-startind+1;
		if(size <= 5)
		{
			pthread_mutex_lock(&lock);
			for(int i=startind;i<endind;i++)
			{
				int min = i;
				for(int j=i+1;j<=endind;j++)
				{
					if(myarr[j] < myarr[min])
						min = j;
				}
				int temp = myarr[min];
				myarr[min] = myarr[i];
				myarr[i] = temp;
			}
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL);
		}
		pthread_t l,r; 
		params *leftchild = (params *)malloc(sizeof(params));
		params *rightchild = (params *)malloc(sizeof(params));
		leftchild->startind = startind;
		leftchild->endind = m;
		rightchild->startind = m+1;
		rightchild->endind = endind;
		int ch1 = pthread_create(&l,NULL,sort,leftchild);
		int ch2 = pthread_create(&r,NULL,sort,rightchild);
		if(ch1)
		{
			perror("failed to create left thread");
		}
		if(ch2)
		{
			perror("failed to create right thread");
		}
		pthread_join(l,NULL);
		pthread_join(r,NULL);
		pthread_mutex_lock(&lock);
		merge(startind,m,endind);
		pthread_mutex_unlock(&lock);
		pthread_exit(NULL);
	}
} 
int main() 
{ 	 
	int array_size;
	printf("Enter the size of array\n");
	scanf("%d",&array_size);  
	printf("Enter array elements\n");
	myarr = (int *)malloc(sizeof(int)*(array_size+5));
	for(int i=0;i<array_size;i++)
	{
		scanf("%d",&myarr[i]);
	}
	// if(array_size <=5)
	// {
	// 	selectionSort(array_size);
	// 	return 0;
	// }
	params *start = malloc(sizeof(params));
	start->startind =0;
	start->endind = array_size-1;
	pthread_t starter;
	pthread_create(&starter,NULL,sort,start);
	pthread_join(starter,NULL);
	for(int i=0;i<array_size;i++)
	{
		printf("%d ",myarr[i]);
	}
	printf("\n");
	return 0; 
} 
