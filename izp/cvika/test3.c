#include <stdio.h>
#include <stdbool.h>

float sum(float a, float b)
{
	return a + b;
}

int my_crazy_min(int a, int b)
{
	if (a < 0)
	{
		a = -a;
	}
	if (b < 0)
	{
		b = -b;
	}

	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

int strlen_m(char *str)
{
	int len = 0;

	char c = str[len];
	while (c != '\0')
	{
		len++;
		c = str[len];
	}

	return len;
}

int my_strchr(char *str, char ch)
{
	int i = 0;

	char c = str[i];
	while (c != ch)
	{
		i++;
		c = str[i];
	}

	return i;
}

bool includes(int *arr, int i, int len)
{
	for (int j = 0; j < len; j++)
	{
		if (arr[j] == i)
		{
			return true;
		}
	}

	return false;
}

int cisla_stejne(int *arr1, int *arr2, int len)
{
	int res = 0;
	int duplikaty[len];
	int duplikaty_i = 0;
	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < len; j++)
		{
			if (arr1[i] == arr2[j] && !includes(duplikaty, arr1[i], duplikaty_i))
			{
				res++;
				duplikaty[duplikaty_i] = arr1[i];
				duplikaty_i++;
			}
		}
	}

	return res;
}

int main()
{
	printf("Sum: %lf\n", sum(2, 30));
	printf("My crazy min: %d\n", my_crazy_min(4, 3));
	printf("Length: %d\n", strlen_m("Hello"));
	int arr1[] = {1, 1, 5, 5, 2};
	int arr2[] = {5, 5, 1, 1, 3};
	printf("Stejna cisla: %d\n", cisla_stejne(arr1, arr2, 5));

	return 0;
}