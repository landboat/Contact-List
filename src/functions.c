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

/*Function opens file.*/
FILE *openFile(FILE *fptr) {

	/*Output if error opening the .db file.*/
	if ((fptr = fopen("myContactList.db" , "ab+")) == NULL) {
		printf("Error opening file.\n");
		exit(0);
	}
	return fptr;
}

/*Function calculates and returns the number of contacts currently held in the database.*/
int countContacts(FILE *fptr) {

	int count = 0;
	Contact temp;

	fseek(fptr, 0, SEEK_SET);

	/*While a Contact struct is still read from file, increment count.*/
	while(fread(&temp, sizeof(Contact), 1, fptr) != 0) {
		fseek(fptr, temp.next, SEEK_SET);
		count++;
	}

	return count;
}

/*Function reads in data previously stored in database file at the start of a new session.*/
int fetchData(FILE *fptr, Contact **contacts, List **list) {

	int num = 0;
	int i = 0;
	char buffer[MAX_STRING];
	
	/*Determines number of contacts stored in database file.*/
	num = countContacts(fptr);

	/*Only reads in data from database if the file was not empty.*/
	if(num != 0) {

		/*Allocates enough memory to store whatever data was already in the file.*/
		*list = calloc(num, sizeof(List));
		*contacts = calloc(num, sizeof(Contact));

		fseek(fptr, 0, SEEK_SET);

		/*Loop reads in contacts struct followed by the strings for each contact.*/
		for(i = 0; i < num; i++) {
			fread(&(*contacts)[i], sizeof(Contact), 1, fptr);
			(*list)[i].phoneNumber = (*contacts)[i].phone_number;

			fread(buffer, sizeof(char), ((*contacts)[i].last_name_posn - (*contacts)[i].first_name_posn), fptr);
			strcpy((*list)[i].firstName, buffer);
			strcpy(buffer, "\0");

			fread(buffer, sizeof(char), ((*contacts)[i].company_name_posn - (*contacts)[i].last_name_posn), fptr);
			strcpy((*list)[i].lastName, buffer);
			strcpy(buffer, "\0");

			fread(buffer, sizeof(char), ((*contacts)[i].email_posn - (*contacts)[i].company_name_posn), fptr);
			strcpy((*list)[i].companyName, buffer);
			strcpy(buffer, "\0");

			fread(buffer, sizeof(char), ((*contacts)[i].next - (*contacts)[i].email_posn), fptr);
			strcpy((*list)[i].email, buffer);
			strcpy(buffer, "\0");

			/*Initializes delete flag for each contact to false.*/
			(*list)[i].isDeleted = 0;

			fseek(fptr, (*contacts)[i].next, SEEK_SET);			
		}
	}

	return num;
}

/*Function prompts user to add info for each new contact.*/
void addContact(FILE *fptr, Contact **contacts, List **list, int *count) {

	/*Initializes each contacts phone number and positions to 0 initially.*/
	(*contacts)[*count].phone_number = 0;
	(*contacts)[*count].first_name_posn = 0;
	(*contacts)[*count].last_name_posn = 0;
	(*contacts)[*count].company_name_posn = 0;
	(*contacts)[*count].email_posn = 0;
	(*contacts)[*count].next = 0;

	/*Set of prompts to trigger and respond to user input.*/
	printf("New Contact\nFirst Name: ");
	addInfo(fptr, FIRST_NAME, count, list, contacts);
	printf("Last Name: ");
	addInfo(fptr, LAST_NAME, count, list, contacts);
	printf("Company Name: ");
	addInfo(fptr, COMPANY_NAME, count, list, contacts);
	printf("Phone Number (enter only numbers): ");
	addInfo(fptr, PHONE, count, list, contacts);
	printf("Email: ");
	addInfo(fptr, EMAIL, count, list, contacts);

	/*Initalizes delete flag for each new contact to false.*/
	(*list)[*count].isDeleted = 0;

	/*Increments number of contacts.*/
	*count = *count + 1;

	return;
}

/*Function determines/executes memory re-allocation if necessary.*/
void reallocateMemory(FILE *fptr, Contact **contacts, List **list, int *count) {

	int num = *count + 20;
	static int oldNum = 0;

	/*Allocates num number of Contact structs.*/
	if(countContacts(fptr) == *count){
		*contacts = realloc(*contacts, sizeof(Contact) * num);
		*list = realloc(*list, sizeof(List) * num);
		oldNum = num;
	}

	/*Reallocates more memory for Contact structs when it is almost full.*/
	else if (*count == (oldNum - 1)) {
		num = num * 1.5;
		*contacts = realloc((*contacts), sizeof (Contact) * num);
		*list = realloc((*list), sizeof(List) * num);
		oldNum = num;
	}		

}

/*Function used by qsort to sort contacts alphabetically. Return value indicates ordering for qsort.*/
int sortFunction(const void *a, const void *b) {

	int value = 0;

	/*Condition if neither a or b has a lastname.*/
	if(strlen(((List *)a)->lastName) != 0 && strlen(((List *)b)->lastName) != 0) {
		value = strcmp(((List *)a)->lastName, ((List *)b)->lastName);
	}
	/*Condition if a doesn't have a lastname but b does.*/
	else if(strlen(((List *)a)->lastName) != 0 && strlen(((List *)b)->lastName) == 0) {
		value = strcmp(((List *)a)->lastName, ((List *)b)->companyName);
	}
	/*Condition if a has a lastname but b doesn't.*/
	else if(strlen(((List *)a)->lastName) == 0 && strlen(((List *)b)->lastName) != 0) {
		value = strcmp(((List *)a)->companyName, ((List *)b)->lastName);
	}
	/*Condition if both a and b have lastnames.*/
	else if(strlen(((List *)a)->lastName) == 0 && strlen(((List *)b)->lastName) == 0) {
		value = strcmp(((List *)a)->companyName, ((List *)b)->companyName);
	}

	return value;
}

/*Function uses qsort to sort contacts alphabetically, then prints the sorted list.*/
void printSorted (List **list, int *count) {

	int i = 0;
	int numDeleted = 0;
	int (*compare_ptr)(const void *a, const void *b);

	/*Assigns function pointer to sortFunction.*/
	compare_ptr = &sortFunction;
	qsort ((*list), *count, sizeof(List), compare_ptr);

	/*Loop determines number of contacts that have been soft-deleted.*/
	for(i = 0; i < *count; i++) {
		if((*list)[i].isDeleted == 1) {
			numDeleted++;
		}
	}
	/*Prints number of contacts still included.*/
	printf("Number of Contacts = %d\n", (*count - numDeleted));

	/*Resets count of soft-deleted contacts.*/
	numDeleted = 0;

	/*Loop prints sorted list by lastname or otherwise company name, ignoring soft-deleted entries.*/
	for(i = 0; i < *count; i++) {
		if((*list)[i].isDeleted == 0) {
			if(strlen((*list)[i].lastName) != 0) {
				printf("%d. %s %s\n", (i + 1 - numDeleted), (*list)[i].firstName, (*list)[i].lastName);
			}
			else {
				printf("%d. %s\n", (i + 1 - numDeleted), (*list)[i].companyName);
			}
		}
		else {
			numDeleted++;
		}
	}
	return;
}

/*Function facilitates user navigation of the program. Returns edited flag to determine if any changes have been made during session.*/
int takeAction(FILE *fptr, Contact **contacts, List **list, int *count) {

	unsigned long oldPhone = 0;
	char oldEmail[MAX_STRING] = "\0";
	char lastAction[5] = "\0";
	char action[5] = "\0";
	int contactNum = -1;
	int edited = 0;
	int flag = 0;
	int i = 0;
	int j = 0;

	/*Loops until user enters X to end the session.*/
	while(strcmp(action, "X") != 0) {
		/*Prompts user for action.*/
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");
		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
		/*Condition if user has not made any previous actions, or if user enters invalid contact number.*/
		if((lastAction[0] == '\0')) {
			flag = initialAction(fptr, contacts, list, count, &contactNum, action, lastAction);
			if(flag == 0) {
				strcpy(lastAction, action);
			}
		}
		/*Condition if user's last action was to delete, save, or return to sorted list.*/
		else if((lastAction[0] == 'D') || (lastAction[0] == 'S') || (lastAction[0] == 'R')) {
			flag = returnAction(fptr, contacts, list, count, &contactNum, action, lastAction);
			if(flag == 0) {
				strcpy(lastAction, action);
			}
		}
		/*Condition if user's last action was to add a contact.*/
		else if((lastAction[0] == 'A')) {
			addAction(contacts, list, count, &edited, action);
			strcpy(lastAction, action);
		}
		/*Condition if user's last action was to view a contact from sorted list.*/
		else if(checkDigits(lastAction) == 1) {
			/*Loop determines how many contacts are currently in the list.*/
			for(i = 0; i < *count; i++) {
				if((*list)[i].isDeleted == 0) {
					j++;
				}
			}
			/*Condition if number entered by user corresponds to an entry in the list.*/
			if(atoi(lastAction) > 0 && atoi(lastAction) < (j + 1)) {
				viewAction(contacts, list, count, &edited, action, contactNum, oldEmail, &oldPhone);
				strcpy(lastAction, action);
			}
			/*Condition if user entered an invalid contact number.*/
			else {
				printf("Invalid contact number.\n");
				strcpy(lastAction, "\0");
			}
		}
		/*Condition if user's last action was to edit a contact.*/
		else if((lastAction[0] == 'E')) {
			editAction(contacts, list, count, &edited, action, contactNum, oldEmail, &oldPhone);
			strcpy(lastAction, action);
		}

		/*Prompt if user entered an invalid action.*/
		else {
			printf("Invalid action.\n");
		}
	} 
	return edited;
}

/*Function processes user's initial action, or after inputting an invalid contact number. Returns flag if invalid contact number was entered.*/
int initialAction (FILE *fptr, Contact **contacts, List **list, int *count, int *contactNum, char *action, char *lastAction) {
	int i = 0;
	int j = 0;
	int temp = 0;
	int flag = 0;
	int index[*count];

	/*Initializes index array to 0's.*/
	for(i = 0; i < *count; i++) {
		index[i] = 0;
	}
	/*Loops until valid input is entered.*/
	while((action[0] != 'A') && (action[0] != 'X') && (checkDigits(action) != 1)) {
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");
		
		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
	}
	/*Condition if action was to add a contact.*/
	if((action[0] == 'A')) {
		reallocateMemory(fptr, contacts, list, count);
		addContact(fptr, contacts, list, count);
	}
	/*Condition if action was to view a contact.*/
	if(checkDigits(action) == 1) {
		/*Loop determines how many contacts are currently in the list, storing the index of each one that has not been soft-deleted.*/
		for(i = 0; i < *count; i++) {
			if((*list)[i].isDeleted == 0) {
				index[j] = i;
				j++;
			}
		}
		/*Condition if number entered by user corresponds to an entry in the list.*/
		if((atoi(action) > 0) && (atoi(action) < (j + 1))) {
			/*Convert number entered by user into corresponding contact's struct index.*/
			*contactNum = (atoi(action) - 1);
			temp = index[*contactNum];
			*contactNum = temp;

			displayContact(contactNum, list, contacts, atoi(action));
		}
		/*Condition if user entered an invalid contact number.*/
		else {
			printf("Invalid contact number.\n");
			strcpy(lastAction, "\0");
			flag = 1;
		}
	}
	return flag;
}

/*Function processes user's action after deleting, saving or returning to sorted list. Returns flag if invalid contact number was entered.*/
int returnAction(FILE *fptr, Contact **contacts, List **list, int *count, int *contactNum, char *action, char *lastAction) {
	int i = 0;
	int j = 0;
	int temp = 0;
	int flag = 0;
	int index[*count];

	/*Initializes index array to 0's.*/
	for(i = 0; i < *count; i++) {
		index[i] = 0;
	}
	/*Loops until valid input is entered.*/
	while((action[0] != 'A') && (action[0] != 'X') && (checkDigits(action) != 1)) {
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");

		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
	}
	/*Condition if action was to add a contact.*/
	if(action[0] == 'A') {
		reallocateMemory(fptr, contacts, list, count);
		addContact(fptr, contacts, list, count);
	}
	/*Condition if action was to view a contact.*/
	if(checkDigits(action) == 1) {
		/*Loop determines how many contacts are currently in the list, storing the index of each one that has not been soft-deleted.*/
		for(i = 0; i < *count; i++) {
			if((*list)[i].isDeleted == 0) {
				index[j] = i;
				j++;
			}
		}
		/*Condition if number entered by user corresponds to an entry in the list.*/
		if((atoi(action) > 0) && (atoi(action) < (j + 1))) {
			*contactNum = (atoi(action) - 1);
			
			temp = index[*contactNum];
			*contactNum = temp;
			displayContact(contactNum, list, contacts, atoi(action));
		}
		/*Condition if user entered an invalid contact number.*/
		else {
			printf("Invalid contact number.\n");
			strcpy(lastAction, "\0");
			flag = 1;
		}
	}
	return flag;
}

/*Funtion processes user's action after deciding to add a new contact.*/
void addAction(Contact **contacts, List **list, int *count, int *edited, char *action) {
	/*Loops until valid input is entered.*/
	while((action[0] != 'S') && (action[0] != 'R')){
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");
		
		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
	}
	/*Condition if user's action was to save the new contact.*/
	if(action[0] == 'S') {
		*edited = 1;
		printSorted(list, count);
	}
	/*Condition if user's action was to return to the sorted list without saving the new contact.*/
	if(action[0] == 'R') {
		*count = *count - 1;
		printSorted(list, count);
	}
	return;
}

/*Funtion processes user's action after deciding to view an existing contact.*/
void viewAction(Contact **contacts, List **list, int *count, int *edited, char *action, int contactNum, char *oldEmail, long unsigned *oldPhone){
	/*Loops until valid input is entered.*/
	while((action[0] != 'D') && (action[0] != 'R') && (action[0] != 'E')) {
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");
		
		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
	}
	/*Condition if the user's action was to delete the contact.*/
	if(action[0] == 'D') {
		/*Sets edited flag to true. Now data must be written to file at the end of the session.*/
		*edited = 1;
		/*Sets selected contact's delete flag to true. Contact will be removed from list and not written to file.*/
		(*list)[contactNum].isDeleted = 1;
		printSorted(list, count);
	}
	/*Condition if user's action was to return to sorted list.*/
	if(action[0] == 'R') {
		printSorted(list, count);
	}
	/*Condition if user's action was to edit the contact.*/
	if(action[0] == 'E') {
		editContact(contacts, list, contactNum, oldEmail, oldPhone);
	}
	return;
}

/*Function processes user's action after deciding to edit a contact.*/
void editAction(Contact **contacts, List **list, int *count, int *edited, char *action, int contactNum, char *oldEmail, long unsigned *oldPhone) {
	/*Loops until valid input is entered.*/
	while((action[0] != 'D') && (action[0] != 'R') && (action[0] != 'S')) {
		printf("Action: ");
		fgets(action, 4, stdin);
		printf("\n");
		
		/*Removes newline from end of user input.*/
		if (action[strlen(action) - 1] == '\n') {
			action[strlen(action) - 1] = '\0';
		}
		/*Converts input to uppercase if not a digit.*/
		if(checkDigits(action) != 1) {
			action[0] = toupper(action[0]);
		}
	}
	/*Condition if user's action was to delete the contact.*/
	if(action[0] == 'D') {
		/*Sets edited flag to true. Now data must be written to file at the end of the session.*/
		*edited = 1;
		/*Sets selected contact's delete flag to true. Contact will be removed from list and not written to file.*/
		(*list)[contactNum].isDeleted = 1;
		printSorted(list, count);
	}
	/*Condition if user's action was to return to sorted list without saving edit.*/
	if(action[0] == 'R') {
		printSorted(list, count);
		/*Restores contact information to data held before edit.*/
		(*list)[contactNum].phoneNumber = *oldPhone;
		strcpy((*list)[contactNum].email, oldEmail);
	}
	/*Condition if user's action was to save the edit to the contact.*/
	if(action[0] == 'S') {
		*edited = 1;
		printSorted(list, count);
	}
	return;
}

/*Function determines whether user's action was an integer. Returns digit flag to convey result.*/
int checkDigits(char *action) {
	int i = 0;
	int digit = 2;

	/*Loops through each character of the user's action.*/
	while(i < strlen(action)) {
		/*Sets digit flag to true if character was a digit.*/
		if(isdigit(action[i])) {
			digit = 1;
		}
		/*Sets digit flag to false and breaks from loop when non digit is encountered.*/
		else {
			digit = 0;
			break;
		}
		i++;
	}
	return digit;
}

/*Function prompts user so input information to be stored in the edited contact.*/
void editContact(Contact **contacts, List **list, int contactNum, char *oldEmail, unsigned long *oldPhone) {
	char string[MAX_STRING] = "\0";
	unsigned long temp = 0;
	int newContact = 0;

	/*Stores old email and phone number incase user decides to not save the edited data.*/
	*oldPhone = (*list)[contactNum].phoneNumber;
	strcpy(oldEmail, (*list)[contactNum].email);

	printf("Contact #%d\n", (contactNum + 1));
	printf("First Name: %s\n", (*list)[contactNum].firstName);
	printf("Last Name: %s\n", (*list)[contactNum].lastName);
	printf("Company Name: %s\n", (*list)[contactNum].companyName);
	
	/*Loops until proper input is provided.*/
	while(!checkPhone(string)) {
		printf("Phone Number (enter only numbers): ");
		fgets(string, MAX_STRING - 1, stdin);
		if (string[strlen(string) - 1] == '\n') {
			string[strlen(string) - 1] = '\0';
		}
	}
	/*Converts char input to an unsigned long.*/
	temp = strtoul(string, NULL, 10);

	/*Stores temp in contacts' phone_number variable.*/
	(*contacts)[contactNum].phone_number = temp;
	(*list)[contactNum].phoneNumber = temp;
	
	/*Loops until proper input is provided.*/
	while(!checkEmail(string)) {
		printf("Email: ");
		fgets(string, MAX_STRING - 1, stdin);
		if (string[strlen(string) - 1] == '\n') {
			string[strlen(string) - 1] = '\0';
		}
	}
	/*Copies string to email.*/
	strcpy((*list)[contactNum].email, string);
	if (strlen((*list)[contactNum].email) != 0) {
		newContact = newContact + strlen((*list)[contactNum].email) + 1;
	}
	return;
}

/*Function displays contact information corresponding to list entry specified by user.*/
void displayContact(int *contactNum, List **list, Contact **contacts, int action) {
	printf("Contact #%d\n", (action));
	printf("First Name: %s\n", (*list)[*contactNum].firstName);
	printf("Last Name: %s\n", (*list)[*contactNum].lastName);
	printf("Company Name: %s\n", (*list)[*contactNum].companyName);
	printf("Phone Number: %lu\n", (*list)[*contactNum].phoneNumber);
	printf("Email: %s\n", (*list)[*contactNum].email);
	return;
}

/*Function recieves input from user and stores it in the list struct for each contact.*/
void addInfo(FILE *fptr, int field, int *count, List **list, Contact **contacts) {
	unsigned long temp = 0;
	char string[MAX_STRING];

	/*Temporarily stores user input in string.*/
	fgets(string, MAX_STRING - 1, stdin);

	/*Removes newline from end of user input.*/
	if (string[strlen(string) - 1] == '\n') {
		string[strlen(string) - 1] = '\0';
	}

	/*Copies string into the list struct depending on which contact field was entered.*/
	switch (field) {

		/*Case for First Name field.*/
		case 1:	
			/*Copies string to firstName.*/		
			strcpy((*list)[*count].firstName, string);
			break;

		/*Case for Last Name field.*/
		case 2:
			/*Copies string to lastName.*/
			strcpy((*list)[*count].lastName, string);
			break;

		/*Case for Company Name field.*/
		case 3:
			/*Condition if a contact's lastname was left empty.*/
			if(strlen((*list)[*count].lastName) == 0) {
				/*Loops until input is provided.*/
				while(strlen(string) == 0) {
					printf("Company Name: ");
					fgets(string, MAX_STRING - 1, stdin);
					if (string[strlen(string) - 1] == '\n') {
						string[strlen(string) - 1] = '\0';
					}
				}
			}
			/*Copies string to companyName.*/
			strcpy((*list)[*count].companyName, string);
			break;

		/*Case for Phone Number field.*/
		case 4:
			/*Loops until proper input is provided.*/
			while(!checkPhone(string)) {
				printf("Phone Number (enter only numbers): ");
				fgets(string, MAX_STRING - 1, stdin);
				if (string[strlen(string) - 1] == '\n') {
					string[strlen(string) - 1] = '\0';
				}
			}
			/*Converts char input to an unsigned long.*/
			temp = strtoul(string, NULL, 10);

			/*Stores temp in contacts' phone_number variable.*/
			(*contacts)[*count].phone_number = temp;
			(*list)[*count].phoneNumber = temp;
			break;

		/*Case for Email field. This is where writing to file occurs.*/
		case 5:
			/*Loops until proper input is provided.*/
			while(!checkEmail(string)) {
				printf("Email: ");
				fgets(string, MAX_STRING - 1, stdin);
				if (string[strlen(string) - 1] == '\n') {
					string[strlen(string) - 1] = '\0';
				}
			}
			/*Copies string to email.*/
			strcpy((*list)[*count].email, string);
			break;	
		
		default:
			break;
	}
	return;
}

/*Function determines if phone number was entered as exactly 7 or 10 digits.*/
int checkPhone(char *string) {
	int result = -1;
	int i = 0;

	/*Ensures input was an acceptable length.*/
	if (strlen(string) == 7 || strlen(string) == 10){
		/*Loops until the end of the string or until a non-digit is encountered.*/
		do {
			result = isdigit(string[i]);
			++i;
		} while (result != 0 && i < strlen(string));
	}
	else {
		result = 0;
	}
	return result;
}

/*Function determines if an email was entered. If yes, ensures input followed the guidelines for email addresses.*/
int checkEmail(char *string) {
	int result = 0;
	int i = 0;

	/*Loops through every character in string.*/
	for(i = 0; i < strlen(string); i++) {
		/*Condition if the string holds an '@' somewhere, and the next character is not a period.*/
		if(string[i] == '@' && string[i+1] != '.') {
			/*Loops through the rest of the string.*/
			while(i < strlen(string)) {
				/*Condition if a period is encountered somewhere in the remaining string.*/
				if(string[i] == '.') {
					result = 1;
				}
				i++;
			}
		}
	}
	/*Since email is not mandatory, if string is empty flag is set to true.*/
	if(string[0] == '\0') {
		result = 1;
	}
	return result;
}

/*Function deletes old .db file, creates a new one, and writes all contacts to the database.*/
void writeFile(FILE *fptr, Contact **contacts, List **list, int *count) {
	int i = 0;
	int flag = 1;
	int lastWritten = -1;
	
	/*Closes original database file.*/
	fclose(fptr);
	fptr = NULL;
	/*Deletes original database file.*/
	flag = remove("myContactList.db");

	/*Prompt if original file could not be deleted.*/
	if(flag != 0) {
		printf("Error erasing old data.\n");
	}

	/*Function call to re-create "myContactList.db".*/
	fptr = openFile(fptr);
	fseek(fptr, 0, SEEK_SET);

	/*Loops through each contact stored in memory.*/
	for(i = 0; i < *count; i++) {
		/*Condition if a contact has not been deleted.*/
		if ((*list)[i].isDeleted == 0) {
			/*Function call to determine file positions.*/
			countSizes(i, lastWritten, contacts, list);
			/*Function call to write contacts and list structs to file.*/
			writeContact(fptr, contacts, list, i);
			/*Stores index of contact - used to determine positions for next written contact.*/
			lastWritten = i;
		}
	}

	/*Closes the database file.*/
	fclose(fptr);
	return;
}

/*Function determines string lengths and calculates file positions for the contacts struct.*/
void countSizes(int i, int lastWritten, Contact **contacts, List **list) {
	int contactSize = 0;

	/*Copies phone number from sorted list.*/
	(*contacts)[i].phone_number = (*list)[i].phoneNumber;

	/*Calculates first_name_posn.*/
	/*Condition for the first contact in the array.*/
	if(i == 0) {
		(*contacts)[i].first_name_posn = sizeof(Contact);
	}
	/*Condition if all previous contacts have been soft-deleted.*/
	else if(lastWritten < 0) {
		(*contacts)[i].first_name_posn = sizeof(Contact);
	}
	else {
		(*contacts)[i].first_name_posn = (*contacts)[lastWritten].next + sizeof(Contact);
	}

	/*Updates variable storing size of contact info.*/
	if (strlen((*list)[i].firstName) != 0) {
		contactSize = strlen((*list)[i].firstName) + 1;
	}

	/*Calculates last_name_posn.*/
	(*contacts)[i].last_name_posn = ((*contacts)[i].first_name_posn + contactSize);

	/*Updates variable storing size of contact info.*/
	if (strlen((*list)[i].lastName) != 0) {
		contactSize = contactSize + strlen((*list)[i].lastName) + 1;
	}

	/*Calculates company_name_posn.*/
	(*contacts)[i].company_name_posn = ((*contacts)[i].first_name_posn + contactSize);

	/*Updates variable storing size of contact info.*/
	if (strlen((*list)[i].companyName) != 0) {
		contactSize = contactSize + strlen((*list)[i].companyName) + 1;
	}

	/*Calculates email_posn.*/
	(*contacts)[i].email_posn = ((*contacts)[i].first_name_posn + contactSize);

	/*Updates variable storing size of contact info.*/
	if (strlen((*list)[i].email) != 0) {
		contactSize = contactSize + strlen((*list)[i].email) + 1;
	}

	/*Calculates next_posn.*/
	(*contacts)[i].next = ((*contacts)[i].first_name_posn + contactSize);
	return;
}

/*Function writes data to database file.*/
void writeContact(FILE *fptr, Contact **contacts, List **list, int i) {

	fwrite(&(*contacts)[i], sizeof(Contact), 1, fptr);

	if (strlen((*list)[i].firstName) != 0) {
		fwrite((*list)[i].firstName, sizeof(char), strlen((*list)[i].firstName) + 1, fptr);
	}
	if (strlen((*list)[i].lastName) != 0) {
		fwrite((*list)[i].lastName, sizeof(char), strlen((*list)[i].lastName) + 1, fptr);
	}
	if (strlen((*list)[i].companyName) != 0) {
		fwrite((*list)[i].companyName, sizeof(char), strlen((*list)[i].companyName) + 1, fptr);
	}
	if (strlen((*list)[i].email) != 0) {
		fwrite((*list)[i].email, sizeof(char), strlen((*list)[i].email) + 1, fptr);
	}
	return;
}

/*Function frees all dynamically allocated memory.*/
void freeMem(Contact *contacts, List *list) {

	free(contacts);
	free(list);
	return;
}