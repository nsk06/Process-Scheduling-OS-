#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

int arr[100000];
int myid; 
key_t key = IPC_PRIVATE; 
int *myarr;
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

void sort(int startind, int endind) 
{ 
	int i, size=endind-startind+1,leftchild,rightchild,childstatus;
	if(startind > endind)
	{
		return ;
	}
	else if(endind>=startind)
	{
		if (size<=5) 
		{  
			for(i=startind;i<endind;i++)
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
			return; 
		}  
		int m = (startind+endind)/2;
		leftchild = fork(); 
		if (leftchild<0) 
		{ 
			perror("Forking error in leftchild");
		} 
		else if (leftchild==0) 
		{ 
			sort(startind,m); 
			_Exit(0); 
		} 
		//else
		//{ 
			
			rightchild = fork(); 
			if (rightchild<0) 
			{ 
				
				perror("Forking error in rightchild");
			} 
			else if(rightchild==0) 
			{ 
				sort(m+1,endind); 
				_Exit(0); 
			} 
			
		//} 
		waitpid(leftchild, &childstatus,WUNTRACED);
		waitpid(rightchild, &childstatus,WUNTRACED); 
		merge(startind,(startind+endind)/2, endind); 
	}
} 
int main() 
{ 	 
	int array_size;
	printf("Enter the size of array\n");
	scanf("%d",&array_size); 
	myid = shmget(key,4*array_size, IPC_CREAT | 0666);
	myarr = shmat(myid, NULL, 0);
	if ( myid == -1) 
	{ 
		perror("shmget failed"); 
		exit(EXIT_FAILURE); 
	} 
	if ( myarr == (void *) -1) 
	{ 
		perror("shmat failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Enter array elements\n");
	for(int i=0;i<array_size;i++)
	{
		scanf("%d",&myarr[i]);
	}
	sort(0, array_size-1); 
	for(int i=0;i<array_size;i++)
	{
		printf("%d ",myarr[i]);
	}
	printf("\n");
	if (shmdt(myarr) == -1) 
	{ 
		perror("shmdt failed"); 
		exit(EXIT_FAILURE); 

	} 
	return 0; 
} 
