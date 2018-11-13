#include <stdio.h>
void merge(int arr1[],int size1,int arr2[],int size2,int res[])
{
	int i=0,j=0;
	while(i<size1 && j<size2)
	{
		if(arr1[i] < arr2[j])
		{
			res[i+j]=arr1[i];
			i++;
		}
		else
		{
			res[i+j]=arr2[j];
			j++;
		}
	}
	while(i<size1)
	{
		res[i+j]=arr1[i];
		i++;
	}
	while(j<size2)
	{
		res[i+j]=arr2[j];
		j++;
	}
}
void sort(int arr[],int size)
{
	int s1,s2,t1[size+2],t2[size+2],mid,i;
	if(size<=1)
	{
		return;
	}
	else
	{
		s1=size/2;
		s2=size-s1;
		for(i=0;i<s1;i++)
			t1[i]=arr[i];
		for(i=0;i<s2;i++)
			t2[i]=arr[i+s1];
		sort(t1,s1);
		sort(t2,s2);
		merge(t1,s1,t2,s2,arr);
	}
}
int main()
{
	int i,j,n,arr[1000000];
	printf("Enter the size of array\n");
	scanf("%d",&n);
	printf("Enter array elements\n");
	for(i=0;i<n;i++)
		scanf("%d",&arr[i]);
	sort(arr,n);
	for(i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
	return 0;
}

