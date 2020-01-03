/*******************************************************************
 Author:		Owen Douglas
 Date: 			Monday, April 1, 2019
 Description:	CIS 2500 - Assignment 4
--------------------------------------------------------------------
 This code is the sole property of the author and cannot be copied or
 altered for another program unless properly referenced. Any 
 violation of the above will be considered misconduct.
*******************************************************************/

#ifndef __LIBRARIES__
#define __LIBRARIES__
#define MAX_STRING 1000
#define FIRST_NAME 1
#define LAST_NAME 2
#define COMPANY_NAME 3
#define PHONE 4
#define EMAIL 5
#define YES "Yes"
#define NO "No"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct contact {
	unsigned long phone_number;
	long first_name_posn;
	long last_name_posn;
	long company_name_posn;
	long email_posn;
	long next;
} Contact;

typedef struct list {
	unsigned long phoneNumber;
	char firstName[MAX_STRING];
	char lastName[MAX_STRING];
	char companyName[MAX_STRING];
	char email[MAX_STRING];
	int isDeleted;
} List;

FILE *openFile(FILE *fptr);
int countContacts(FILE *fptr);
int fetchData(FILE *fptr, Contact **contacts, List **list);
void addContact(FILE *fptr, Contact **contacts, List **list, int *count);
void reallocateMemory(FILE *fptr, Contact **contacts, List **list, int *count);
int sortFunction(const void *a, const void *b);
void printSorted (List **list, int *count);
int takeAction(FILE *fptr, Contact **contacts, List **list, int *count);
int initialAction (FILE *fptr, Contact **contacts, List **list, int *count, int *contactNum, char *action, char *lastAction);
int returnAction(FILE *fptr, Contact **contacts, List **list, int *count, int *contactNum, char *action, char *lastAction);
void addAction(Contact **contacts, List **list, int *count, int *edited, char *action);
void viewAction(Contact **contacts, List **list, int *count, int *edited, char *action, int contactNum, char *oldEmail, long unsigned *oldPhone);
void editAction(Contact **contacts, List **list, int *count, int *edited, char *action, int contactNum, char *oldEmail, long unsigned *oldPhone);
int checkDigits(char *action);
void editContact(Contact **contacts, List **list, int contactNum, char *oldEmail, unsigned long *oldPhone);
void displayContact(int *contactNum, List **list, Contact **contacts, int action);
void addInfo(FILE *fptr, int field, int *count, List **list, Contact **contacts);
int checkPhone(char *string);
int checkEmail(char *string);
void writeFile(FILE *fptr, Contact **contacts, List **list, int *count);
void countSizes(int i, int lastWritten, Contact **contacts, List **list);
void writeContact(FILE *fptr, Contact **contacts, List **list, int i);
void freeMem(Contact *contacts, List *list);

#endif