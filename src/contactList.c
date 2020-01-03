/*******************************************************************
 Author:		Owen Douglas
 Date: 			Monday, April 1, 2019
 Description:	CIS 2500 - Assignment 4
--------------------------------------------------------------------
 This code is the sole property of the author and cannot be copied or
 altered for another program unless properly referenced. Any 
 violation of the above will be considered misconduct.
*******************************************************************/

#include "libraries.h"

int main () {

	FILE *fptr = 0;
	Contact *contacts = NULL;
	List *list = NULL;
	int count = 0;
	int edited = 0;

	/*Call to open file. Returns pointer to opened file.*/
	fptr = openFile(fptr);

	/*Transfers contents of database into list struct.*/
	count = fetchData(fptr, &contacts, &list);

	/*Inital print of sorted contacts imported from the .db file.*/
	printSorted(&list, &count);

	/*Function responsible for generating the interface of the program. Returns a flag to determine if any changes were made to the data in the database.*/
	edited = takeAction(fptr, &contacts, &list, &count);

	/*Writes data to database only if edited flag is true.*/
	if(edited == 1) {
		writeFile(fptr, &contacts, &list, &count);
	}

	/*Frees dynamically allocated memory.*/
	freeMem(contacts, list);
	
	return 0;
}