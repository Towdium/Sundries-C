/**************************************************************************************
@Name:
	A student database
@Copyright:
	Copyright (C) 2015 Juntong Liu
	This program uses GNU V2 lisence
@FileName:
	1405096_5.c
@Author:
	Juntong Liu
@Date:
	5st Dec 2015
@Description:
	This program enables the user to generate a database of student
	or open a database generated by the same program.
	Operations available are add one student, search for one student
	by his ID number and sort & print all the records.
	Each student has following records: family name, given name,
	ID number, email, recent 6 marks and the average grade.
@Constraint:
	The student's family name and given name is set to be no more than
	19 characters and email address is set to be no more than 39 
	characters. The ID number is set to be 7 numbers intendedly.
@Remarks:
	This program save all students in linked list, the first element
	of which is empty and this strying email is used to record the opend
	filename for possible usage. In addition, the list is kept sorted
	by ID number everytime the program runs. This will provide convenience
	when sorting and can simplify the operation when sorting the list.

	This program treat the database as a structre containing a file
	pointer and a pointer to the first element (empty element) of the
	list.
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*stucture define*/
typedef struct StudentStrt {
	char nameFamily[20];
	char nameGiven[20];
	int numberID;
	char email[40];
	int grade1;
	int grade2;
	int grade3;
	int grade4;
	int grade5;
	int grade6;
	float gradeAvg;
	struct Student* stdFormer;
	struct Student* stdLatter;
}Student;

typedef struct DatabaseStrct {
	Student* listStudent;
	FILE* fileStudent;
}Database;

/*fcunction declare*/
int getChoice(int length);
Database databaseNew();
Database databaseOpen();
Database databaseLoad(char filename[]);
Database databaseErrorExist(char filename[]);
Database databaseErrorNotfound(char filename[]);
void databaseSearchElement(Database* db);
int databaseOperates(Database* db);
void quit();
void elementInsert(Student *elementDest, Student *elementToInsert);
int listGetPos(Student* list, int numberID, Student** buffer);
void listFree(Student* list);
void databaseAddElement(Database* db);
void databaseSave(Database* db);
int checkNum(char str[], int length);
void databasePrint(Database* db);
void elementPrint(Student* stu);
Student* elementNew();
int markInput();


/******************************************************
name:
	main
input:
	void
output:
	int=0 if terminated normally
function:
	Ask the user whether to open or create a database.
	Then call functions to read or initialize the data.
	Then call functions to provide operations
*******************************************************/
int main() {
	int loop = 1;
	do {
		int inputChoice, i = 0;
		Database db;
		system("cls");
		puts("=======================================================");
		puts("\tWelcome to the student database!");
		puts("\tFollowing choices are available:");
		puts("=======================================================");
		puts("\t1. Create a new database;");
		puts("\t2. Load an exist database;");
		puts("\t0. Quit");
		puts("=======================================================");
		printf("Please choose one action by entering the number: ");
		inputChoice = getChoice(2);
		switch (inputChoice) {
		case 1:
			db = databaseNew();
			break;
		case 2:
			db = databaseOpen();
			break;
		case 0:
			quit();
			break;
		default:
			break;
		}
		while (i == 0) {
			i = databaseOperates(&db);
		}
		/*close operations*/
		databaseSave(&db);
		fclose(db.fileStudent);
		listFree(db.listStudent);
	} while (loop == 1);
	quit();
}

/******************************************************
name:
	getChoice
input:
	int length: The amount of available choices
output:
	int: the entered choice
function:
	Ask the user to input one choice (integer)
	and check if it is within the acceptable range.
	Ask to input again when out of range.
*******************************************************/
int getChoice(int length) {
	int inputChoice, inputValid = 1;
	/*input*/
	rewind(stdin);
	scanf("%d", &inputChoice);
	/*chech value*/
	if (inputChoice<0 || inputChoice>length) {
		inputValid = 0;
	}
	/*check if input is pure number*/
	else if (getchar() != '\n') {
		inputValid = 0;
	}
	/*ask to input again and check*/
	while (inputValid == 0) {
		inputValid = 1;
		printf("Invalid input, please try again: ");
		rewind(stdin);
		scanf("%d", &inputChoice);
		if (inputChoice<0 || inputChoice>length) {
			inputValid = 0;
		}
		else if (getchar() != '\n') {
			inputValid = 0;
		}
	}
	return inputChoice;
}

/******************************************************
name:
	databaseNew
input:
	void
output:
	Database: the generated database
function:
	Ask the user to enter a filename 
	and create a new database.
	When there is already one file with this name,
	call function to deal with it.
*******************************************************/
Database databaseNew() {
	Database db;
	FILE* file;
	Student* list = elementNew();
	char fileName[34];
	/*input filename*/
	printf("Please input filename of the database (witout suffix): ");
	rewind(stdin);
	fgets(fileName, 30, stdin);
	fileName[strlen(fileName) - 1] = 0;
	strcat(fileName, ".db");
	/*check file exist*/
	file = fopen(fileName, "rb");
	if (file != NULL) {
		fclose(file);
		return databaseErrorExist(fileName);
	}
	file = fopen(fileName, "wb+");
	/*assign*/
	db.fileStudent = file;
	db.listStudent = list;
	/*record filename*/
	strcpy(db.listStudent->email, fileName);
	return db;
}

/******************************************************
name:
	databaseOpen
input:
	void
output:
	Database: the generated database
function:
	Ask the user to enter a filename 
	and open the database.
	When there is no file with this name,
	call fcunction to deal with it.
*******************************************************/
Database databaseOpen() {
	Database db;
	FILE* file;
	char fileName[34];
	/*input filename*/
	printf("Please input filename of the database (witout suffix): ");
	rewind(stdin);
	fgets(fileName, 30, stdin);
	fileName[strlen(fileName) - 1] = 0;
	strcat(fileName, ".db");
	/*check file exist*/
	file = fopen(fileName, "rb");
	if (file == NULL) {
		db = databaseErrorNotfound(fileName);
		return db;
	}
	fclose(file);
	/*load*/
	file = fopen(fileName, "rb+");
	return databaseLoad(fileName);
}

/******************************************************
name:
	databaseOperates
input:
	Database* db
output:
	int: return 1 when chose to return to main menu
		otherwise return 0
function:
	Provide a list of operations available
	Call different functions for operations
*******************************************************/
int databaseOperates(Database* db) {
	int choice;
	/*print*/
	system("cls");
	puts("=======================================================");
	puts("\tDatabase is ready to use,\n\tfollowing choices are available: ");
	puts("=======================================================");
	puts("\t1. Add a student;");
	puts("\t2. Search for studets's information;");
	puts("\t3. Sort and print database;");
	puts("\t4. Return to main menu;");
	puts("\t0. Quit.");
	puts("=======================================================");
	printf("Please choose one action: ");
	choice = getChoice(4);
	switch (choice)
	{
	case 1:
		databaseAddElement(db);
		break;
	case 2:
		databaseSearchElement(db);
		break;
	case 3:
		databasePrint(db);
		break;
	case 4:
		return 1;
		break;
	case 0:
		/*procedure before quitting*/
		databaseSave(db);
		fclose(db->fileStudent);
		listFree(db->listStudent);
		quit();
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return 0;
}

/******************************************************
name:
	databaseErrorExist
input:
	char filename[]: filename (with suffix)
output:
	Database: database generated
function:
	This function is created to deal with situation
	when user is trying to create a new file with the
	filename already occupied.
	The function provides a list of operations available
	including open the file, clear the content then open
	and use another name.
	Call different functions for operations
*******************************************************/
Database databaseErrorExist(char filename[]) {
	int choice;
	Database db;
	Student* student = elementNew();
	FILE* file;
	/*print*/
	system("cls");
	puts("=======================================================");
	puts("\tFile already exist,\n\tyou have following actions available:");
	puts("=======================================================");
	puts("\t1. Open the file;");
	puts("\t2. Clear the content and open the file;");
	puts("\t3. create another file;");
	puts("\t0. Quit.");
	puts("=======================================================");
	printf("Please choose one action: ");
	choice = getChoice(3);
	/*operations*/
	switch (choice)
	{
	case 1:
		return databaseLoad(filename);
		break;
	case 2:
		file = fopen(filename, "wb+");
		db.fileStudent = file;
		db.listStudent = student;
		strcpy((db.listStudent)->email, filename);
		break;
	case 3:
		return databaseNew();
		break;
	case 0:
		quit();
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return db;
}

/******************************************************
name:
	quit
input:
	void
output:
	void
function:
	Provide warning and quit the program
remarks:
	This function will not do any of preparation
	before quitting. All the procedure should be
	done before calling this function including
	freeing memory and save file.
*******************************************************/
void quit() {
	int t;
	for (t = 3; t != 0; t--) {
		system("cls");
		puts("=======================================================");
		printf("\r\tQuit in %d seconds.\n", t);
		puts("\tHave fun with the database!");
		puts("=======================================================");
		Sleep(1000);
	}
	exit(0);
}

/******************************************************
name:
	databaseLoad
input:
	char filename[]: filename (with suffix)
output:
	Database: the loaded database
function:
	Load the function from a exist file.
remarks:
	This function will not check whether the file exists.
	It should be checked before calling this function.
*******************************************************/
Database databaseLoad(char filename[]) {
	Student* stuList = elementNew();
	FILE* file = fopen(filename, "rb+");
	Database db;
	db.fileStudent = file;
	db.listStudent = stuList;
	/*record file name*/
	strcpy(db.listStudent->email, filename);
	/*load students*/
	while (EOF != fgetc(file)) {
		fseek(file, -1, SEEK_CUR);
		Student* temp = (Student*)malloc(sizeof(Student));
		Student* pos = NULL;
		fread(temp, sizeof(Student), 1, file);
		temp->stdFormer = NULL;
		temp->stdLatter = NULL;
		listGetPos(db.listStudent, temp->numberID, &pos);
		elementInsert(pos, temp);
	}
	return db;
}

/******************************************************
name:
	databaseErrorNotFound
input:
	char filename[]: filename (with suffix)
output:
	Database: database generated
function:
	This function is created to deal with situation
	when user is trying to load the file when there is
	no such file.
	The function provides a list of operations available
	including choose another file and create this file.
	Call different functions for operations.
*******************************************************/
Database databaseErrorNotfound(char filename[]) {
	int choice;
	Database db;
	Student* student = elementNew();
	FILE* file;
	/*print*/
	system("cls");
	puts("=======================================================");
	puts("\tFile is not found,\n\tyou have following actions available:");
	puts("=======================================================");
	puts("1. Choose another file to open;");
	puts("2. Create this file;");
	puts("0. Quit.");
	puts("=======================================================");
	printf("Please choose one action: ");
	/*operate*/
	choice = getChoice(2);
	switch (choice)
	{
	case 1:
		return databaseOpen();
		break;
	case 2:
		file = fopen(filename, "wb+");
		db.fileStudent = file;
		db.listStudent = student;
		strcpy(db.listStudent->email, filename);
		break;
	case 0:
		quit();
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return db;
}

/******************************************************
name:
	elementInsert
input:
	Student* elementDest: The position wanted to insert.
		(The element before the inserted element after
		insertion)
	Student* elementToInsert: Element to insert
output:
	void
function:
	Insert one element to the linked list at the given
	position.
*******************************************************/
void elementInsert(Student* elementDest, Student* elementToInsert) {
	Student* temp;
	/*position provided is the last element in the list*/
	if (elementDest->stdLatter == NULL) {
		elementDest->stdLatter = elementToInsert;
		elementToInsert->stdFormer = elementDest;
	}
	/*position provided is not the last element in the list*/
	else {
		temp = elementDest->stdLatter;
		elementDest->stdLatter = elementToInsert;
		temp->stdFormer = elementToInsert;
		elementToInsert->stdFormer = elementDest;
		elementToInsert->stdLatter = temp;
	}
}

/******************************************************
name:
	listGetPos
input:
	Student* list: The linked list of students
	int numberID: The ID number to find the position
	Student** buffer: pointer to contain the result structure
output:
	int: -1 when there already exist an element with the
		given ID number; 0 when there isn't
function:
	Search for the element with the provided ID number.
	If there exists, assign the element to pointer buffer
	and return -1;
	If there doesn't exist, assign the element with largest
	ID number smaller than the given number to the pointer
	and return 0;
remarks:
	The return value seems a little weird because the
	function of this functions has been changed, but it
	works at this stage.
*******************************************************/
int listGetPos(Student* list, int numberID, Student** buffer) {
	Student *pIndex;
	pIndex = list;
	/*get to the element with largest ID smaller than or equal to the given ID*/
	while (pIndex->numberID < numberID && pIndex->stdLatter != NULL) {
		pIndex = pIndex->stdLatter;
	}
	/*case when the ID in element equals to the geven one*/
	if (pIndex->numberID == numberID) {
		*buffer = pIndex;
		return -1;
	}
	/*case when the ID in element smaller than the geven one*/
	else if (pIndex->numberID > numberID) {
		pIndex = pIndex->stdFormer;
	}
	*buffer = pIndex;
	return 0;
}

/******************************************************
name:
	databaseAddElement
input:
	Database* db: the database
output:
	void
function:
	Ask the user to input the information of one student
	and add it into the list in the database.
*******************************************************/
void databaseAddElement(Database* db) {
	/*initialize*/
	Student* stu = elementNew();
	Student* buffer = NULL;
	char numberID[9];
	int duplicate = -1;
	system("cls");
	puts("=======================================================");
	puts("\tPlease fill following information.");
	puts("=======================================================");
	/*ID number*/
	printf("Student ID: ");
	rewind(stdin);
	fgets(numberID, 9, stdin);
	while (checkNum(numberID, 7) != 1) {
		puts("Incorrect format, ID number should be 7 numbers.");
		printf("Student ID: ");
		rewind(stdin);
		fgets(numberID, 9, stdin);
	}
	stu->numberID = atoi(numberID);
	/*check if duplicate and add*/
	duplicate = listGetPos(db->listStudent, stu->numberID, &buffer);
	while (duplicate == -1) {
		puts("Duplicate ID number, try again.");
		printf("Student ID: ");
		rewind(stdin);
		fgets(numberID, 9, stdin);
		while (checkNum(numberID, 7) != 1) {
			puts("Incorrect format, ID number should be 7 numbers.");
			printf("Student ID: ");
			rewind(stdin);
			fgets(numberID, 9, stdin);
		}
		stu->numberID = atoi(numberID);
		duplicate = listGetPos(db->listStudent, stu->numberID, &buffer);
		/*the position to insert the element will be ontained in buffer*/
		/*when the ID number dosen't duplicate*/
	}
	/*family name*/
	printf("Family name: ");
	rewind(stdin);
	fgets(stu->nameFamily, 20, stdin);
	stu->nameFamily[strlen(stu->nameFamily) - 1] = 0;
	/*given name*/
	printf("Given name: ");
	rewind(stdin);
	fgets(stu->nameGiven, 20, stdin);
	stu->nameGiven[strlen(stu->nameGiven) - 1] = 0;
	/*email*/
	printf("Email address: ");
	rewind(stdin);
	fgets(stu->email, 40, stdin);
	stu->email[strlen(stu->email) - 1] = 0;
	/*marks*/
	printf("1st mark: ");
	stu->grade1 = markInput();
	printf("2nd mark: ");
	stu->grade2 = markInput();
	printf("3rd mark: ");
	stu->grade3 = markInput();
	printf("4st mark: ");
	stu->grade4 = markInput();
	printf("5st mark: ");
	stu->grade5 = markInput();
	printf("6st mark: ");
	stu->grade6 = markInput();
	/*get average mark*/
	stu->gradeAvg = (stu->grade1 + stu->grade2 + stu->grade3 + stu->grade4 + stu->grade5 + stu->grade6) / 6;
	/*insert element*/
	elementInsert(buffer, stu);
}

/******************************************************
name:
	databaseSave
input:
	Database* db
output:
	void
function:
	Clear the content in the file.
	Save the records in the memory into the file.
*******************************************************/
void databaseSave(Database* db) {
	Student* stu;
	/*clean file content*/
	fclose(db->fileStudent);
	fopen(db->listStudent->email, "wb+");
	stu = db->listStudent;
	/*save records*/
	while (stu->stdLatter != NULL) {
		stu = stu->stdLatter;
		fwrite(stu, sizeof(Student), 1, db->fileStudent);
	}
}

/******************************************************
name:
	checkNum
input:
	char str[]: the strying to check 
	int length: the required length of the number
output:
	int: 1 when meets require, 0 when not
function:
	Check if the given string is constructed with
	certain amount of numbers.
remarks:
	For instance, "0000001" is acceptable and "1" is not
	when the required length is 7.
*******************************************************/
int checkNum(char str[], int length) {
	int i;
	for (i = 0; i < length; i++) {
		if ((str[i] - '0') > 10 || (str[i] - '0') < 0) {
			return 0;
		}
	}
	if (str[i] != '\n') {
		return 0;
	}
	return 1;
}

/******************************************************
name:
	databasePrint
input:
	Database* db: 
output:
	void
function:
	Print all the content in the database
*******************************************************/
void databasePrint(Database* db) {
	Student* stu;
	system("cls");
	puts("=======================================================");
	puts("\tThe database has following records:");
	puts("=======================================================");
	stu = db->listStudent;
	/*first element*/
	/*the first element is isolated due to type setting reason*/
	if (stu->stdLatter != NULL) {
		stu = stu->stdLatter;
		elementPrint(stu);
	}
	else {
		puts("\tNo record in the database.");
	}
	/*following elements*/
	while (stu->stdLatter != NULL) {
		puts("\t---------------------------------------------");
		stu = stu->stdLatter;
		elementPrint(stu);
	}
	puts("=======================================================");
	printf("Finished. Press enter to continue.");
	rewind(stdin);
	getchar();
}

/******************************************************
name:
	elementPrint
input:
	Student* stu: element to print
output:
	void
function:
	Print the record of the student.
*******************************************************/
void elementPrint(Student* stu) {
	printf("\tStudent %07d:\n", stu->numberID);
	printf("\t   Name:                %s %s;\n", stu->nameGiven, stu->nameFamily);
	printf("\t   Email:               %s\n", stu->email);
	printf("\t   Recent 6 grades:     %d; %d; %d; %d; %d; %d;\n", stu->grade1, stu->grade2, stu->grade3, stu->grade4, stu->grade5, stu->grade6);
	printf("\t   Average of 6 grades: %.2f;\n", stu->gradeAvg);
}

/******************************************************
name:
	databaseSearchElement
input:
	Database* db: the database
output:
	void
function:
	Ask the user to enter an ID number.
	Find the element with the ID number.
	If it exists, print it.
	If it doesn't exist, print feedback.
*******************************************************/
void databaseSearchElement(Database* db) {
	Student* stu = NULL;
	int numberIDIn;
	char numberIDCh[9];
	system("cls");
	puts("=======================================================");
	puts("\t Enter student information to start.");
	puts("=======================================================");
	printf("Enter the ID number of the student to search: ");
	/*enter ID number*/
	rewind(stdin);
	fgets(numberIDCh, 9, stdin);
	while (checkNum(numberIDCh, 7) != 1) {
		puts("Incorrect format, ID number should be 7 numbers.");
		printf("Try Again: ");
		rewind(stdin);
		fgets(numberIDCh, 9, stdin);
	}
	numberIDIn = atoi(numberIDCh);
	/*record found*/
	if (-1 == listGetPos(db->listStudent, numberIDIn, &stu)) {
		puts("=======================================================");
		puts("\tStudent found:");
		puts("\t---------------------------------------------");
		elementPrint(stu);
		puts("=======================================================");
	}
	/*record not found*/
	else {
		puts("=======================================================");
		puts("\tSorry. Student not found.");
		puts("=======================================================");
	}
	printf("Press enter to contimue.");
	rewind(stdin);
	getchar();
}

/******************************************************
name:
	markInput
input:
	void
output:
	int: the entered mark
function:
	Input a number and check if it is from 0 - 100
	If not, ask to input again.
remark:
	This function will not provide any instruction
	to ask the user to input the number.
	Instructions should be provided before calling
	this function.
*******************************************************/
int markInput() {
	int i;
	/*input*/
	rewind(stdin);
	scanf("%d", &i);
	/*check format*/
	while (i < 0 || i > 100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("Try again: ");
		rewind(stdin);
		scanf("%d", &i);
	}
	return i;
}

/******************************************************
name:
	listFree
input:
	Student* list
output:
	void
function:
	Free all the memory allocated in the list.
*******************************************************/
void listFree(Student* list) {
	Student *temp;
	temp = list;
	/*find last element*/
	while ( temp->stdLatter != NULL) {
		temp = temp->stdLatter;
	}
	/*move pointer temp from tail to head*/
	/*then free the element after it*/
	while (temp->stdFormer != NULL) {
		temp = temp->stdFormer;
		free(temp->stdLatter);
	}
	/*free the last element*/
	free(temp);
}

/******************************************************
name:
	elementNew
input:
	void
output:
	Student*: pointer to the Student structure generated 
function:
	Call some memory and create a Student structure.
	Then initialize it.
*******************************************************/
Student* elementNew() {
	Student* stu = (Student*)malloc(sizeof(struct StudentStrt));
	strcat(stu->email, "");
	strcat(stu->nameFamily, "");
	strcat(stu->nameGiven, "");
	stu->grade1 = 0;
	stu->grade2 = 0;
	stu->grade3 = 0;
	stu->grade4 = 0;
	stu->grade5 = 0;
	stu->grade6 = 0;
	stu->gradeAvg = 0;
	stu->numberID = -1;
	stu->stdFormer = NULL;
	stu->stdLatter = NULL;
	return stu;
}