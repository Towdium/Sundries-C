#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct Database {
	struct Costomer* cosList;
	struct Account* accList;
} Db;

typedef struct Costomer {
	struct AccountRecord* accList;
	char name[30];
	char address[100];
	char telephone[20];
	char ID[20];
	struct Costomer* CosLast;
	struct Costomer* CosNext;
} Cos;

typedef struct Account{
	float balance;
	char ownerID[20];
	long accountNumber;
	int PIN;
	int state;/*1 when active, 0 when freezed*/
	struct StandOrderRecord* SORecList;
	struct OperationRecord* OPRecList;
	struct Account* AccLast;
	struct Account* AccNext;
} Acc;

/*For internal use*/
typedef struct StandOrderRecord {
	long timeAssigned;
	long timeLastOperation;
	long interval;
	long timeEnd;
	struct StandOrderRecord* SORecLast;
	struct StandOrderRecord* SORecNext;
} SORec;

/*For internal use*/
typedef struct OperationRecord {
	/*0 when withdraw, 1 when deposit*/
	/*2 when stand order in, 3 when stand order out*/
	int type;
	char time[20];
	float amount;
	char info[20];
	struct OperationRecord* OPRecLast;
	struct OperationRecord* OPRecNext;
} OPRec;

typedef struct AccountRecord {
	long acc;
	struct AccountRecord* AccRecLast;
	struct AccountRecord* AccRecNext;
} AccRec;


/********************************************************************************
return:
- Db*: the loaded database
function:
- Load the exist database, or creat when there isn't
remarks:
- This should be used once the program runs
********************************************************************************/
Db* db_Load();

/********************************************************************************
input:
- Db* db: pointer to the database to free
function:
- Free all the allocated memory to the database
remarks:
- Highly suggested to run before close the program
********************************************************************************/
void db_Free(Db* db);

/********************************************************************************
input:
- Db* db: pointer to the database
- char ID[]: string of ID
return:
- Cos*: the costomer found or NULL when not found
function:
- Search and return the costomer with corresponding ID in the database
- Return NULL when no such costomer found
********************************************************************************/
Cos* db_GetCos(Db* db, char ID[]);

/********************************************************************************
input:
- Db* db: pointer to the database
- long accNum: account number
return:
- Acc*: the account found or NULL when not found
function:
- Search and return the account with corresponding account number in the database
- Return NULL when no such costomer found
********************************************************************************/
Acc* db_GetAcc(Db* db, long accNum);

/********************************************************************************
input:
- Db* db: pointer to the database
- long accNum: account number
return:
- Acc*: the account found or NULL when not found
function:
- Search and return the account with corresponding account number in the costomer
- Return NULL when no such costomer found
********************************************************************************/
Acc* cos_GetAcc(Cos* cos, long accNum);

/********************************************************************************
input:
- Db* db: pointer to the database
- others: costomer information
return:
- Cos*: generated costomer
function:
- Add one costomer to the exist database
- Remenber to check if there is already a costomer with the ID. Use db_GetCos
********************************************************************************/
Cos* db_AddCos(Db* db, char address[], char ID[], char name[], char telephone[]);

/********************************************************************************
input:
- Db* db: the destination database
- Cos* cos: the destination costomer
- int PIN: the PIN number
return:
- The generated account
function:
- Add one account to the exist costomer
********************************************************************************/
Acc* db_AddAcc(Db* db, int PIN, Cos* cos);

/********************************************************************************
input:
- Acc* acc: the destination account
- long interval: the interval between each transfer
- float amount: the amount to move
function:
- Add one stand order record to exsiting account
********************************************************************************/
void acc_AddSORec(Acc* acc, long interval, float amount);

/********************************************************************************
input:
- Acc* acc: the destination account
- float amount: the amount to deposit
function:
- Deposit money to exsiting account
remarks:
- This function will automatically add operation record
********************************************************************************/
void acc_Deposit(Acc* acc, float amount);

/********************************************************************************
input:
- Acc* acc: the destination account
- float amount: the amount to withdraw
function:
- Withdraw money to exsiting account
remarks:
- Remember to check if there is enough balance
- This function will automatically add operation record
********************************************************************************/
int acc_Withdraw(Acc* acc, float amount);

/********************************************************************************
input:
- Cos* cos: the destination costomer
return:
- float: the avarage balance of the costomer
function:
- Calculate and return the avarage balance of the costomer
********************************************************************************/
float cos_GetAvg(Cos* cos);

/********************************************************************************
input:
- Db* db: database to print
- int mode: 1 to print all costomers
function:
- Print information in the database, for debug use
********************************************************************************/
void db_Print(Db* db, int mode);

/********************************************************************************
input:
- Cos* cos: costomer to print
- int mode: 
- - 1 to print all costomer information
- - 2 to print all costomer with account
function:
- Print information of the costomer
remarks:
- This is used as a debug tool
********************************************************************************/
void cos_Print(Cos* cos, int mode, Db* db);


/*for internal use*/

void db_Fread(Db* db);
void db_LinkCos(Db* dest, Cos* cos);
void db_Fwrite(Db* db);
void cos_Fwrite(Cos* cos, FILE* stream, Db* db);
void cos_Fread(Cos* cos, FILE* stream, Db* db);
void checkString(char str[]);
void acc_Print(Acc* acc, int mode);
void db_LinkAcc(Db* dest, Acc* acc);
void acc_Fwrite(Acc* acc);
void cos_AddAcc(Cos* cos, long accNum);
void acc_Fread(Db* db, long accNum);
void acc_LinkSORec(Acc* dest, SORec* buffer);
void acc_LinkOPRec(Acc* dest, OPRec* opRec);


/*sample code*/
int main() {
	Db* db = db_Load();
	Cos* cos;
	db_AddCos(db, "Suzhou", "320105199509260000", "Juntong Liu", "18661206723");
	db_AddCos(db, "Nanjing", "320100000000000000", "Wole Gequ", "18318312345");
	cos = db_GetCos(db, "320105199509260000");
	db_AddAcc(db, 123456, cos);
	db_Print(db, 2);
	db_Free(db);
	system("pause");
}


/*function code*/

Db* db_Load() {
	Db* db = malloc(sizeof(Db));
	db->cosList = NULL;
	/*declare an empty costomer as the first element in the list*/
	Cos* cos = malloc(sizeof(Cos));
	cos->accList = NULL;
	cos->CosLast = NULL;
	cos->CosNext = NULL;
	strcpy(cos->address, "");
	strcpy(cos->ID, "");
	strcpy(cos->name, "");
	strcpy(cos->telephone, "");
	db->cosList = cos;
	Acc* acc = malloc(sizeof(Acc));
	acc->balance = 0;
	acc->AccLast = NULL;
	acc->AccNext = NULL;
	acc->accountNumber = 0;
	acc->OPRecList = NULL;
	acc->PIN = 0;
	acc->SORecList = NULL;
	acc->state = 0;
	strcpy(acc->ownerID, "");
	db->accList = acc;
	/*check if the file exists*/
	FILE* file = fopen(".\\data\\information.db", "r");
	/*if doesn't exist*/
	if (file == NULL) {
		/*create the database file*/
		system("mkdir data");
		file = fopen(".\\data\\information.db", "w+");
		fclose(file);
		return db;
	}
	/*if exist*/
	else {
		fclose(file);
		db_Fread(db);
		return db;
	}
}

void db_Fread(Db* db) {
	FILE* file = fopen(".\\data\\information.db", "r+");
	Cos* temp = malloc(sizeof(Cos));
	int i = 1;
	if (fgetc(file) == EOF) {
		free(temp);
		return;
	}
	fseek(file, -1, SEEK_CUR);
	fread(temp, sizeof(Cos), 1, file);
	free(temp);
	do {
		Cos* buffer = malloc(sizeof(Cos));
		cos_Fread(buffer, file, db);
		if (strcmp(buffer->ID, "") != 0) {
			db_LinkCos(db, buffer);
		}
		else {
			i = 0;
			free(buffer);
		}
	} while (i);
}

void db_LinkCos(Db* dest, Cos* cos) {
	Cos* buffer = dest->cosList;
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
	}
	buffer->CosNext = cos;
	cos->CosLast = buffer;
}

void cos_LinkAccRec(Cos* dest, AccRec* accRec) {
	AccRec* buffer = dest->accList;
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	buffer->AccRecNext = accRec;
	accRec->AccRecLast = buffer;
}

Cos* db_AddCos(Db* db, char address[], char ID[], char name[], char telephone[]) {
	Cos* buffer = malloc(sizeof(Cos));
	checkString(address);
	checkString(ID);
	checkString(name);
	checkString(telephone);
	/*set the first element in the account list to be empty*/
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	buffer->accList = accRec;
	/*assign input value*/
	strcpy(buffer->address, address);
	strcpy(buffer->name, name);
	strcpy(buffer->telephone, telephone);
	strcpy(buffer->ID, ID);
	buffer->CosLast = NULL;
	buffer->CosNext = NULL;
	db_LinkCos(db, buffer);
	/*update file content*/
	db_Fwrite(db);
	return buffer;
}

void cos_Fread(Cos* cos, FILE* stream, Db* db) {
	Cos* temp = malloc(sizeof(Cos));
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	cos->accList = accRec;
	fread(temp, sizeof(Cos), 1, stream);
	cos->CosLast = NULL;
	cos->CosNext = NULL;
	strcpy(cos->ID, temp->ID);
	strcpy(cos->name, temp->name);
	strcpy(cos->address, temp->address);
	strcpy(cos->telephone, temp->telephone);
	if (strcmp(temp->ID, "") == 0) {
		cos->accList = NULL;
	}
	else {
		int i = 1;
		AccRec* temp = malloc(sizeof(AccRec));
		fread(temp, sizeof(AccRec), 1, stream);
		free(temp);
		while (i) {
			AccRec* buffer = malloc(sizeof(AccRec));
			fread(buffer, sizeof(AccRec), 1, stream);
			if (buffer->acc == 0) {
				i = 0;
				free(buffer);
			}
			else {
				cos_LinkAccRec(cos, buffer);
				acc_Fread(db, buffer->acc);
			}
		}
	}
}

void cos_Print(Cos* cos, int mode, Db* db) {
	if (mode < 0) {
		return;
	}
	printf("Name: %s\n", cos->name);
	printf("ID  : %s\n", cos->ID);
	printf("Tel : %s\n", cos->telephone);
	printf("Add : %s\n", cos->address);
	if (mode > 0) {
		AccRec* buffer = cos->accList;
		Acc* acc;
		if (buffer->AccRecNext == NULL) {
			puts("No account.");
		}
		else {
			while (buffer->AccRecNext != NULL) {
				buffer = buffer->AccRecNext;
				acc = db_GetAcc(db, buffer->acc);
				acc_Print(acc, mode - 1);
			}
		}
	}
}

void db_Print(Db* db, int mode) {
	Cos* buffer = db->cosList;
	if (buffer->CosNext == NULL) {
		puts("No record");
	}
	else {
		while (buffer->CosNext != NULL) {
			buffer = buffer->CosNext;
			cos_Print(buffer, mode - 1, db);
			puts("");
		}
		puts("Finished");
	}
}

void db_Free(Db* db) {
	Cos* bufferC = db->cosList;
	while (bufferC->CosNext != NULL) {
		bufferC = bufferC->CosNext;
	}
	while (bufferC->CosLast != NULL) {
		bufferC = bufferC->CosLast;
		free(bufferC->CosNext);
	}
	free(bufferC);
	Acc* bufferA = db->accList;
	while (bufferA->AccNext != NULL) {
		bufferA = bufferA->AccNext;
	}
	while (bufferA->AccLast != NULL) {
		bufferA = bufferA->AccLast;
		free(bufferA->AccNext);
	}
	free(bufferA);
	free(db);
}

void db_Fwrite(Db* db) {
	Cos* buffer = db->cosList;
	Cos empty = { NULL, "", NULL, NULL, "", "", "" };
	FILE* file = fopen(".\\data\\information.db", "w+");
	cos_Fwrite(buffer, file, db);
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
		cos_Fwrite(buffer, file, db);
	}
	cos_Fwrite(&empty, file, db);
	fclose(file);
}

void cos_Fwrite(Cos* cos, FILE* stream, Db* db) {
	if (cos->accList == NULL) {
		fwrite(cos, sizeof(Cos), 1, stream);
	}
	else {
		AccRec* index = cos->accList;
		AccRec empty = { 0, NULL, NULL };
		Acc* acc;
		fwrite(cos, sizeof(Cos), 1, stream);
		fwrite(index, sizeof(AccRec), 1, stream);
		while (index->AccRecNext != NULL) {
			index = index->AccRecNext;
			fwrite(index, sizeof(AccRec), 1, stream);
			acc = db_GetAcc(db, index->acc);
			acc_Fwrite(acc);
		}
		fwrite(&empty, sizeof(AccRec), 1, stream);
	}
}

Cos* db_GetCos(Db* db, char ID[]) {
	Cos* index = db->cosList;
	while (index->CosNext != NULL) {
		index = index->CosNext;
		if (strcmp(index->ID, ID) == 0) {
			return index;
		}
	}
	return NULL;
}

void checkString(char str[]) {
	int l = strlen(str);
	int i = 0;
	for (i = 0; i < l; i++) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}

Acc* db_AddAcc(Db* db, int PIN, Cos* cos) {
	Acc* buffer = malloc(sizeof(Acc));
	/*set the first element in the account list to be empty*/
	SORec* soRec = malloc(sizeof(SORec));
	OPRec* opRec = malloc(sizeof(OPRec));
	soRec->interval = 0;
	soRec->timeAssigned = 0;
	soRec->timeEnd = 0;
	soRec->timeLastOperation = 0;
	soRec->SORecLast = NULL;
	soRec->SORecNext = NULL;
	buffer->SORecList = soRec;
	opRec->amount = 0;
	opRec->OPRecLast = NULL;
	opRec->OPRecNext = NULL;
	opRec->type = -1;
	strcpy(opRec->info, "");
	strcpy(opRec->time, "");
	buffer->OPRecList = opRec;
	/*assign input value*/
	buffer->PIN = PIN;
	buffer->state = 1;
	buffer->AccLast = NULL;
	buffer->AccNext = NULL;
	buffer->accountNumber = time(0);
	buffer->balance = 0;
	strcpy(buffer->ownerID, "");
	strcpy(buffer->ownerID, cos->ID);
	db_LinkAcc(db, buffer);
	cos_AddAcc(cos, buffer->accountNumber);
	/*update file content*/
	db_Fwrite(db);
	acc_Fwrite(buffer);
	
	return buffer;
}

void db_LinkAcc(Db* dest, Acc* acc) {
	Acc* buffer = dest->accList;
	while (buffer->AccNext != NULL) {
		buffer = buffer->AccNext;
	}
	buffer->AccNext = acc;
	acc->AccLast = buffer;
}

void acc_Fwrite(Acc* acc) {
	char filename[25] = ".\\data\\";
	char accNumC[25];
	FILE* file;
	SORec* indexS = acc->SORecList;
	SORec emptyS = { 0, NULL, NULL, 0, 0, 0 };
	OPRec* indexO = acc->OPRecList;
	OPRec emptyO = { -1, "", 0, "", NULL, NULL };
	sprintf(accNumC, "%ld.db", acc->accountNumber);
	strcat(filename, accNumC);
	file = fopen(filename, "w+");
	fwrite(acc, sizeof(Acc), 1, file);
	/*print stand order record*/
	fwrite(indexS, sizeof(SORec), 1, file);
	while (indexS->SORecNext != NULL) {
		indexS = indexS->SORecNext;
		fwrite(indexS, sizeof(SORec), 1, file);
	}
	fwrite(&emptyS, sizeof(SORec), 1, file);
	/*print operation record*/
	fwrite(indexO, sizeof(OPRec), 1, file);
	while (indexO->OPRecNext != NULL) {
		indexO = indexO->OPRecNext;
		fwrite(indexO, sizeof(OPRec), 1, file);
	}
	fwrite(&emptyO, sizeof(OPRec), 1, file);
	fclose(file);
}

void cos_AddAcc(Cos* dest, long accNum) {
	AccRec* accRec = malloc(sizeof(AccRec));
	AccRec* buffer = dest->accList;
	accRec->acc = accNum;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	buffer->AccRecNext = accRec;
	accRec->AccRecLast = buffer;
}

void acc_Print(Acc* acc, int mode) {
	printf("Acc Num : %ld\n", acc->accountNumber);
	printf("Balance : %f\n", acc->balance);
	printf("Costomer: %s\n", acc->ownerID);
	printf("PIN     : %06d\n", acc->PIN);
	printf("State   : %d\n", acc->state);
}

Acc* db_GetAcc(Db* db, long accNum) {
	Acc* index = db->accList;
	while (index->AccNext != NULL) {
		index = index->AccNext;
		if (index->accountNumber = accNum) {
			return index;
		}
	}
	return NULL;
}

void acc_Fread(Db* db, long accNum) {
	char filename[25] = ".\\data\\";
	char accNumC[25];
	Acc* buffer = malloc(sizeof(Acc));
	SORec* temp1 = malloc(sizeof(SORec));
	OPRec* temp2 = malloc(sizeof(OPRec));
	SORec* temp3 = malloc(sizeof(SORec));
	OPRec* temp4 = malloc(sizeof(OPRec));
	FILE* file;
	int i = 1;
	sprintf(accNumC, "%ld.db", accNum);
	strcat(filename, accNumC);
	file = fopen(filename, "r+");
	fread(buffer, sizeof(Acc), 1, file);
	fread(temp1, sizeof(SORec), 1, file);
	free(temp1);
	temp3->interval = 0;
	temp3->SORecLast = NULL;
	temp3->SORecNext = NULL;
	temp3->timeAssigned = 0;
	temp3->timeEnd = 0;
	temp3->timeLastOperation = 0;
	buffer->SORecList = temp3;

	temp4->amount = 0;
	temp4->OPRecLast = NULL;
	temp4->OPRecNext = NULL;
	temp4->type = -1;
	strcpy(temp4->info, "");
	strcpy(temp4->time, "");
	buffer->OPRecList = temp4;
	while (i) {
		SORec* buffer1 = malloc(sizeof(SORec));
		fread(buffer1, sizeof(SORec), 1, file);
		if (buffer1->timeAssigned == 0) {
			i = 0;
			free(buffer1);
		}
		else {
			acc_LinkSORec(buffer, buffer1);
		}
	}

	fread(temp2, sizeof(OPRec), 1, file);
	free(temp2);
	i = 1;
	while (i) {
		OPRec* buffer2 = malloc(sizeof(OPRec));
		fread(buffer2, sizeof(OPRec), 1, file);
		if (buffer2->type == -1) {
			i = 0;
			free(buffer2);
		}
		else {
			acc_LinkOPRec(buffer, buffer2);
		}
	}
	db_LinkAcc(db, buffer);
}

void acc_LinkSORec(Acc* dest, SORec* soRec) {
	SORec* buffer = dest->SORecList;
	while (buffer->SORecNext != NULL) {
		buffer = buffer->SORecNext;
	}
	buffer->SORecNext = soRec;
	soRec->SORecLast = buffer;
}

void acc_LinkOPRec(Acc* dest, OPRec* opRec) {
	OPRec* buffer = dest->OPRecList;
	while (buffer->OPRecNext != NULL) {
		buffer = buffer->OPRecNext;
	}
	buffer->OPRecNext = opRec;
	opRec->OPRecLast = buffer;
}