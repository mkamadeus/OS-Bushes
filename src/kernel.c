// INTERRUPT HANDLING
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void printInteger(int n);
void printHexa(int n);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *path, int *sectorCount, char parentIndex);
void executeProgram(char *filename, int segment, int *success, char parentIndex);
void returnToKernel(char *string);
void putchar(int x, int y, char cc, char color);
void printStringFormat(int x, int y, char *string, char color);

int div(int a, int b);
int mod(int a, int b);

int getEmptySectorCount(char *buffer, int sectors);
int getFirstEmptySector(char *buffer, int sectors);

// String ops
char isStringEqual(char *a, char *b, int length);
char isStringStartsWith(char *a, char *b, int length);
int stringLength(char *string, int max);

int getCurrentFolderIndex(char *currentPath);
int getPathIndex(char parentIndex, char *filePath);

// MAIN FUNCTIONS
void printBootLogo();


int main () {
	char command[512];
	char filename[512];
	char fileRead[512 * 20];
	int flag = 1;
	int i;

    makeInterrupt21();

	printBootLogo();

	printString("\n");

    while (1) {
		// printString("bushes:~ ");
		// readString(command);
		// if(isStringEqual(command, "moo", 3))
		// {
		// 	executeProgram("moo", 0x2000, &flag);
		// }
		// else if(isStringEqual(command, "hello", 5))
		// {
		// 	executeProgram("hello", 0x2000, &flag);
		// }
		// else if(isStringEqual(command, "uwu", 3))
		// {
		// 	readFile(fileRead, "key.txt", &flag);
		// 	printString(fileRead);
		// 	printString("\r\n");
		// }
		// else if(isStringEqual(command, "milestone1", 10))
		// {
		// 	executeProgram("milestone1", 0x2000, &flag);
		// }
		// clear(fileRead, 512 * 20);
		// printString("Write a command [cat|run|ls]: ");
		// readString(input);
		// if (isStringEqual(input, "cat", 100)) {
		// 	printString("Pick a file to load: ")
		// }
		readString(command);
		printString(command);
		writeFile(command, "apel", &flag, 0xFF);
		// writeFile(command, "cavendish", &flag, 0x00);
	}
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	char AL, AH;
	AL = (char) (AX);
	AH = (char) (AX >> 8);
	switch (AL) {
		case 0x00:
			printString(BX);
			break;
		case 0x01:
			readString(BX);
			break;
		case 0x02:
			readSector(BX, CX);
			break;
		case 0x03:
			writeSector(BX, CX);
			break;
		case 0x04:
			readFile(BX, CX, DX, AH);
			break;
		case 0x05:
			writeFile(BX, CX, DX, AH);
			break;
		case 0x06:
			executeProgram(BX, CX, DX, AH);
			break;
		default:
			printString("Invalid interrupt");
	}
}


void printString(char *string) {
	char * pointer = string;

	// While not NULL...
	while (*pointer != 0x00) {
		interrupt(0x10, 0x0e00 + *pointer, 0x000F, 0, 0);	// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
		pointer++;
	}
}

void printInteger(int n) {
	int tmp;
	char* number;

	tmp=n;

	if(tmp == 0) {printString("0");return;}

	while(tmp!=0)
	{
		number[0] = mod(tmp,10) + '0';
		number[1] = 0;
		
		printString(number);
		tmp = div(tmp, 10);
	}
}

void printHexa(int n)
{
	int tmp = n;
	char number[3];
	number[2] = 0;

	number[0] = div(tmp, 16);
	if(number[0] >= 10) number[0] = (number[0]-10) + 'A';
	else number[0] = number[0] + '0';
	number[1] = tmp - div(tmp,16)*16;
	if(number[1] >= 10) number[1] = (number[1]-10) + 'A';
	else number[1] = number[1] + '0';

	printString(number);

}

void readString(char *string) {
	// Interrupt for reading keystroke (16)
	int charInput = interrupt(0x16, 0, 0, 0, 0);

	// Initialize buffer position
	int pos = 0;

	while (charInput != '\r') {
		// If character is not backspace...
		if (charInput != '\b')
		{
			string[pos++] = charInput;
			interrupt(0x10, 0xe00 + charInput, 0xF, 0, 0);	// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
		}
		// If character is backspace and position is not 0 (not at starting point)
		else if(pos>0)
		{
			// Clear previous character
			interrupt(0x10, 0xe00 + '\b', 0xF, 0, 0);	// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
			interrupt(0x10, 0xe00 + ' ', 0xF, 0, 0);	// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
			interrupt(0x10, 0xe00 + '\b', 0xF, 0, 0);	// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
			
			// Replace deleted character with NULL (0)
			string[--pos] = 0;
		}
		charInput = interrupt(0x16, 0, 0, 0, 0);		// int 16=Keyboard; Ah 00=Get Keystroke Ret AH= char input
	} 
	// CRLF
	interrupt(0x10, 0xe00 + '\n', 0xF, 0, 0);		// int 10=Video; AH 0e=TTY Output; BL 0F=White Front
	interrupt(0x10, 0xe00 + '\r', 0xF, 0, 0);		// int 10=Video; AH 0e=TTY Output; BL 0F=White Front

	string[pos] = 0;
}

void readSector(char *buffer, int sector) {
	// int 13=Disk; AH 02=Read Sector; AL 01=Read 1 sector; BX Output Buffer; CH Cylinder #; CL Sector #; DH Head #; DL Drive #
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	// int 13=Disk; AH 03=Write Sector; AL 01=Read 1 sector; BX Input Buffer; CH Cylinder #; CL Sector #; DH Head #; DL Drive #
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

int div(int a, int b) {
	int x = 0;
	while (a > b) {
		a -= b;
		x++;
	}
	return x;
}

int mod(int a, int b) {
	while (a > b) {
		a -= b;
	}
	return a;
}

void clear(char *buffer, int length) {
	int i;
	for (i = 0; i < length; i++) {
		buffer[i] = 0;
	}
}

void writeFile(char *buffer, char *path, int *sectorCount, char parentIndex) {
	char map[512];
	char files[1024];
	char sectors[512];

	int filesRow;
	int sectorsRow;
	int sectorToWrite;
	int i;
	int tmp;

	// Read map, files, sectors
	// 0x100, 0x101-0x102, 0x103
	readSector(map, 256);
	readSector(files, 0x101);
	readSector(files+512, 0x102);
	readSector(sectors, 0x103);
	
	// Check for empty row in files
	for (filesRow = 0; filesRow < 64; filesRow++) {

		// If filename empty (no file/folder exists)...
		if (files[(filesRow << 4) + 2] == 0)
		{
			printString("Files sector available\n\r");
			break;
		}
	}

	for(i = 0; i<512 ; i++)
	{
		printHexa(map[i]);
		printString(" ");
	}
	printString("\n\r");

	// If files sector is full...
	if (filesRow == 64) {
		printString("Failed to write file, files sector limit reached");
		return;
	}

	// If there are not enough sectors to be written...
	if (getEmptySectorCount(map, 256) < *sectorCount) {
		// Stop writing process
		printString("Failed to write file, map sector limit reached");
		return;
	}

	// Check for empty row in sectors
	for(sectorsRow = 0; sectorsRow < 32; sectorsRow++)
	{
		// If sectors row empty...
		if((sectors[sectorsRow << 4]) == 0){
			printString("Sectors sector available\n\r");
			break;
		}
	}

	// If sectors sector is full...
	if(sectorsRow == 32) {
		printString("Failed to write file, sectors sector limit reached");
		return;
	}

	// Clear files row
	clear(files + (filesRow << 4), 16);
	
	// Set `P` value in files row
	files[filesRow<<4] = parentIndex;

	// Set `S` value in files row
	files[(filesRow<<4) + 1] = sectorsRow;

	// Set filename in files row
	for (i = 0; i < 14; i++) {
		if (path[i] == 0)
			break;
		
		files[(filesRow<<4) + 2 + i] = path[i];
	}


	// Write all buffer contents
	for (i = 0; i < *sectorCount; i++) {
		// Find empty sector to write in
		sectorToWrite = getFirstEmptySector(map, 256);
		
		// Mark in map sector
		map[sectorToWrite] = 0xFF;

		// Store sector number in sectors sector
		sectors[(sectorsRow<<4) + i] = sectorToWrite;

		printString("writing to sector ");
		printInteger(sectorToWrite);
		printString("\n\r");
		// tmp = sectorToWrite;
		// while(tmp!=0)
		// {
		// 	printString("pisng");
		// 	tmp = div(tmp, 10);
		// }
		// printString("\n\r");

		// Write buffer to  sector
		writeSector(buffer + (i<<9), sectorToWrite);
	}

	/// Write to system.img
	writeSector(map, 0x100);
	writeSector(files, 0x101);
	writeSector(files+512, 0x102);
	writeSector(sectors, 0x103);
}

int getEmptySectorCount(char *buffer, int sectors) {
	int count;
	int i;

	count = 0;
	for (i = 0; i < sectors; i++) {
		if (buffer[i] == 0x00) {
			count++;
		}
	}
	return count;
}

int getFirstEmptySector(char *buffer, int sectors) {
	int i;
	for (i = 0; i < sectors; i++) {
		printString("i\n\r");
		if (buffer[i] == 0x00) {
			return i;
		}
	}
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
	char sectors[512];
	int noSector;
	int secPos;
	int fileIdx;
	
	fileIdx = getPathIndex(parentIndex, path);

	if (fileIdx == -1) {
		*result = -1;
		return;
	}

	// Baca sektor dir
	// readSector(dir, 2);

	// Nama file sesuai?
	// for (entry = 0; entry < 16; entry++) {
	// 	if (isStringEqual(dir + entry * 32, filename, 12) == 1) {
	// 		break;
	// 	}
	// }

	// success <- FALSE
	// if (entry == 16) {
	// 	printString("Failed to read file, no file found\n\r");
	// 	*result = -1;
	// 	return;
	// }

	printString("File found, reading file\n\r");
	// Baca sektor
	for (noSector = 0; noSector < 20; noSector++) {
		secPos = fileIdx * 16 + noSector;
		if (sectors[secPos] == 0) {
			printString("End of file..\n\r");
			break;
		}
		printString("Reading sector...\n\r");
		readSector(buffer + noSector * 512, sectors[secPos]);
	}
	
	*result = 1;
}

char isStringEqual(char *a, char *b, int length) {
	int i;

	for (i = 0; i < length; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == 0) {
			return 1;
		}
	}

	return 1;
}

char isStringStartsWith(char *a, char *b, int length) {
	int i;

	for (i = 0; i < length; i++) {
		if (b[i] == 0) {
			return 1;
		}
		if (a[i] != b[i]) {
			return 0;
		}
	}

	return 1;
}

int stringLength(char *string, int max) {
	int length = 0;
	while (string[length] != 0 && length < max) {
		length++;
	}
	return length;
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
	char buffer[512 * 20];
	int i;

	clear(buffer, 512 * 20);

	readFile(buffer, filename, success, 0xFF);

	if (*success == 0) {
		printString("Siao a kenot launch edi a...");
		return;
	}

	for (i = 0; i < 512 * 20; i++) {
		putInMemory(segment, i, buffer[i]);
	}

	launchProgram(segment);
	// printString("Tescok");
}

void returnToKernel(char *string) {
	int one = 1;
	executeProgram(string, 0x3000, &one, 0xFF);
}

void printBootLogo() {
	printString("	               ,@@@@@@@,\r\n"					);
	printString("       ,,,.   ,@@@@@@/@@,  .oo8888o.\r\n"			);
	printString("    ,&%%&%&&%,@@@@@/@@@@@@,8888\\88/8o\r\n"		);
	printString("   ,%&\\%&&%&&%,@@@\\@@@/@@@88\\88888/88'\r\n"		);
	printString("   %&&%&%&/%&&%@@\\@@/ /@@@88888\\88888'\r\n"		);
	printString("   %&&%/ %&%%&&@@\\ V /@@' `88\\8 `/88'\r\n"		);
	printString("   `&%\\ ` /%&'    |.|        \\ '|8'\r\n"			);
	printString("//\\ \\/ ._\\//_/__/  ,\\_//__\\/.  \\_//__/_\r\n"	);
	printString("     _               _                        \r\n");
	printString("    | |__  _   _ ___| |__   ___  ___          \r\n");
	printString("    | '_ \\| | | / __| '_ \\ / _ \\/ __|      \r\n");
	printString("    | |_) | |_| \\__ \\ | | |  __/\\__ \\     \r\n");
	printString("    |_.__/ \\__,_|___/_| |_|\\___||___/       \r\n");
                                  

}

void putchar(int x, int y, char cc, char color){
	putInMemory(0xB000, 0x8000 + (2*(80*y+x)), cc);
	putInMemory(0xB000, 0x8000 + (2*(80*y+x))+1, color);
}

void printStringFormat(int x, int y, char *string, char color) {
	char * pointer = string;
	int startx = x;
	while (*pointer != 0x00) {
		if(*pointer=='\n'){
			x = startx;
			y++;
			pointer++;
		} else {
			putchar(x++, y, *(pointer++), color);
		}
	}
}

int getCurrentFolderIndex(char *currentPath) {
	char lineSize;
	char maxFileCount;
	char files[512 * 2];
	char idx;
	char P;
	char pathReadPos;
	char isFileFound;

	lineSize = 0x10;
	maxFileCount = 0x40;
	idx = 0;
	P = 0xFF;
	pathReadPos = 0;
	isFileFound = 1;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);
	
	// Get index of current path
	while (currentPath[pathReadPos] != 0x00) {
		if (currentPath[pathReadPos] == '/') {
			if (isFileFound == 0) {
				return -1;
			}
			isFileFound = 0;
			pathReadPos++;
		} else {
			if (isStringStartsWith(currentPath + pathReadPos, files + idx * 16 + 2, 14)) {
				pathReadPos += stringLength(idx, 14);
				isFileFound = 1;
				P = idx;
				idx = 0;
			} else {
				idx++;
			}
			if (idx >= maxFileCount) {
				return -1;
			}
		}
	}
	return P;
}

int getPathIndex(char parentIndex, char *filePath) {
	char lineSize;
	char maxFileCount;
	char files[512 * 2];
	char idx;
	char P;
	char pathReadPos;
	char isFileFound;

	lineSize = 0x10;
	maxFileCount = 0x40;
	idx = 0;
	P = 0xFF;
	pathReadPos = 0;
	isFileFound = 0;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	// TODO: Optimize to not check current path
	if (filePath[0] == '/') {	// If Root folder
		pathReadPos ++;
		P = 0xFF;
	} else if (filePath[0] == '.' && filePath[1] == '/') {	// If current folder
		pathReadPos += 2;
	} else {			// If @ $PATH
		// P = $PATH
	}

	// Get index of filePath
	while (filePath[pathReadPos] != 0x00) {
		if (filePath[pathReadPos] == '/') {		// Go inside folder
			if (isFileFound == 0) {
				return -1;
			}
			isFileFound = 0;
			pathReadPos++;
		} else if (filePath[pathReadPos] == '.') {
			pathReadPos++;
			// parent folder
			if (filePath[pathReadPos] == '.' && filePath[pathReadPos + 1] == '.' && filePath[pathReadPos + 2] == '/') {
				if (P == 0xFF) return -1;
				P = files[P * 16];
				pathReadPos += 3;
			} else {
				// Read normally
			}
		} else {
			if (isStringStartsWith(filePath + pathReadPos, files + idx * 16 + 2, 14)) {
				pathReadPos += stringLength(idx, 14);
				isFileFound = 1;
				P = idx;
				idx = 0;
			} else {
				idx++;
			}
			if (idx >= maxFileCount) {
				return -1;
			}
		}
	}

	return P;
}