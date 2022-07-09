//selection sorting example.
//Find 101-th smallest number in data
#include<stdio.h>
#define DATA_SIZE 10000
int main()
{
	int data[DATA_SIZE];

	int i, j, temp; // index for sort
	int min=0;
	int res=0;
	int count=0;
	#include "input_data.h"

	for (i = 0; i < 102; i ++) {
		min = i;
		for (j = i+1 ; j < DATA_SIZE ; j++) {
			// find min
			if (data[j] < data[min]) {
				min = j;
			}
			count++;
		}
		temp = data[min];
		data[min] = data[i];
		data[i] = temp;
	}
// for X86
/*
	printf("101-th smallest data: %d \n", data[100]);
	return 0;
*/
	res = data[100];
	printf("%d\n",res);
	return res;
}
