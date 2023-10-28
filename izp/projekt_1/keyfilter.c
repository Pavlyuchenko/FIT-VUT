/*
 * File:    keyfilter.c
 * Date:    28. 10. 2023
 * Author:  Michal Pavlíček, xpavlim00@stud.fit.vutbr.cz
 */

// Header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// Constants
#define MAX_ADRESS_LENGTH 101

// Error codes
#define ERROR_TOO_LONG_LINE 1	// Line from stdin is too long
#define ERROR_TOO_LONG_SEARCH 2 // Search argument is too long

// List of functions
bool is_substring(char *str1, char *str2);
void to_upper(char *str);
void load_line(char *store, char c);
int remove_duplicates(char *arr, int number_of_allowed_chars);
void sort_chars(char *chars, int len);
void print_result(int number_of_allowed_chars, char *allowed_chars, char *found_address);

/**
 * Checks if first string is substring of the other string
 * @param str1 - char array that is being checked for substring
 * @param str2 - char array that str1 is being checked against
 * @return bool -> true if str1 is substring of str2
 */
bool is_substring(char *str1, char *str2)
{
	/*
		Checks if str1 is a substring of str2
		Ex.: is_substring("abc", "abcde") -> true
	 */
	int str1_length = strlen(str1);
	int str2_length = strlen(str2);

	if (str1_length > str2_length) // Can't be substring if str1 is longer than str2
	{
		return false;
	}

	if (str1_length == 0 || str1[0] == '\0') // Empty string is a substring of any string
	{
		return true;
	}

	for (int i = 0; i < str1_length; i++)
	{
		if (str1[i] != str2[i])
		{
			return false;
		}
	}

	return true;
}

/**
 * Inplace conversion of all chars in str to uppercase
 * @param str - string of any case
 */
void to_upper(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		str[i] = toupper(str[i]);
		i++;
	}
}

/**
 * Loads a line inplace from stdin into store variable
 * @param store - string of characters that the line will be loaded into
 * @param c - the first character of the line
 */
void load_line(char *store, char c)
{
	int i = 0;
	while (c != '\n' && c != EOF && c != '\0')
	{
		if (i > MAX_ADRESS_LENGTH)
		{
			fprintf(stderr, "Error: Line from stdin is too long. Max length is %d.\n", MAX_ADRESS_LENGTH);
			exit(ERROR_TOO_LONG_LINE);
		}

		c = toupper(c);
		store[i] = c;
		i++;

		c = getchar();
	}

	store[i] = '\0';
}

/**
 * Removes duplicate characters from arr and adjusts the resulting number of characters
 */
int remove_duplicates(char *arr, int number_of_allowed_chars)
{
	for (int i = 0; i < number_of_allowed_chars; i++)
	{
		for (int j = i + 1; j < number_of_allowed_chars; j++)
		{
			if (arr[i] == arr[j])
			{
				arr[j] = arr[number_of_allowed_chars - 1];
				number_of_allowed_chars--;
				j--;
			}
		}
	}

	return number_of_allowed_chars;
}

/**
 * Sort characters inplace in an array. Using O(n**2) Insertion sort.
 */
void sort_chars(char *chars, int len)
{
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = i + 1; j < len; j++)
		{
			if (chars[i] > chars[j])
			{
				char temp = chars[j];
				chars[j] = chars[i];
				chars[i] = temp;
			}
		}
	}
}

/**
 * Helper function for printing the results to stdout.
 */
void print_result(int number_of_allowed_chars, char *allowed_chars, char *found_address)
{
	if (number_of_allowed_chars == 0)
	{
		/* 3. Adresa nenalezena */
		printf("Not found\n");
	}
	else if (number_of_allowed_chars == 1)
	{
		/* 1. Adresa nalezena */
		printf("Found: %s\n", found_address);
	}
	else
	{
		/* 2. Adresa vyžaduje specifikaci */
		printf("Enable: ");

		number_of_allowed_chars = remove_duplicates(allowed_chars, number_of_allowed_chars);
		sort_chars(allowed_chars, number_of_allowed_chars);

		for (int i = 0; i < number_of_allowed_chars; i++)
		{
			putchar(allowed_chars[i]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	char search[MAX_ADRESS_LENGTH];

	if (argc >= 2)
	{
		if (strlen(argv[1]) > MAX_ADRESS_LENGTH)
		{
			fprintf(stderr, "Error: Your search argument is too long. Max length is %d.\n", MAX_ADRESS_LENGTH);
			return ERROR_TOO_LONG_SEARCH;
		}

		strcpy(search, argv[1]);
	}
	else
	{
		strcpy(search, "\0");
	}
	to_upper(search);

	int search_length = strlen(search);

	char found_address[MAX_ADRESS_LENGTH]; // For storing found address in case it has to be printed (if it was the only one FOUND)

	int number_of_allowed_chars = 0;
	char allowed_chars[26];

	char c = getchar();
	while (c != EOF)
	{
		char temp_address[MAX_ADRESS_LENGTH]; // For storing address from stdin
		load_line(temp_address, c);

		bool is_substr = is_substring(search, temp_address);
		if (is_substr)
		{
			allowed_chars[number_of_allowed_chars] = temp_address[search_length]; // Add the char to allowed_chars (for ENABLE)

			number_of_allowed_chars++;
			strcpy(found_address, temp_address); // Store the address in case it has to be printed (if it was the only one FOUND)
		}

		c = getchar();
	}

	print_result(number_of_allowed_chars, allowed_chars, found_address);

	return 0;
}