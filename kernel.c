#include <stdio.h>
void printString(char[]);
void printChar(char);
void readString(char[]);
void readSector(char*, int);
//void handleInterrupt21(int, int, int, int);


//interrupt(interNum, AX, BX, CX, DX)

int main() {
//    char line[80];
//    makeInterrupt21();
//    interrupt(0x21,1,line,0,0);
//    printString("\n");
//    interrupt(0x21,0,line,0,0);

    char buffer[13312];   /*this is the maximum size of a file*/
    int sectorsRead;
//    printString("sectorsrad\n"); -Not getting to this line
    printChar("x")
    makeInterrupt21();
    interrupt(0x21, 3, "messag", buffer, &sectorsRead);   /*read the file into buffer*/
    if (sectorsRead>0)
        interrupt(0x21, 0, buffer, 0, 0);   /*print out the file*/
    else
        interrupt(0x21, 0, "messag not found\r\n", 0, 0);  /*no sectors read? then print an error*/
    while(1);

}

void printString(char chars[]){
    char selectedChar = chars[0];
    int i=0;

    while(selectedChar != 0x0){
        char al = selectedChar;
        char ah = 0xe;
        int ax = ah * 256 + al;
        interrupt(0x10, ax, 0, 0, 0);
        i++;
        selectedChar = chars[i];

    }
}

void printChar(char c){
    char al = c;
    char ah = 0xe;
    int ax = ah * 256 + al;
    interrupt(0x10, ax, 0, 0, 0);

}

void readString(char input[]){
    char key;
    int i=0;
    do {
        key = interrupt(0x16, 0, 0, 0, 0);
        if(key != 0x8){
            input[i] = key;
            interrupt(0x10, 0xe*256+key, 0, 0, 0);
            i++;
        } else if (key == 0x8){
            if (i>0){
                i--;
                interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0); //backspace
                interrupt(0x10, 0xe * 256 + 0x20, 0, 0, 0); //space
                interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0); //backspace
            }
        }

    } while(key != 0xd);
    interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0); //final carriage return
}

void readSector(char *buffer, int sector){
    int AH = 2; //(this number tells the BIOS to read a sector as opposed to write)
    int AL = 1; //number of sectors to read (use 1)
    char *BX = buffer; //address where the data should be stored to (pass your char* array here)
    int CH = 0; //track number
    int CL = sector + 1;//relative sector number (sector number plus one)
    int DH = 0; //head number
    int DL = 0x80; //device number (for the hard disk, use 0x80)

    int AX = AH*256+AL;
    int CX = CH*256+CL;
    int DX = DH*256+DL;

    interrupt(0x13, AX, BX, CX, DX);
}

readFile(char *fileName, char *buffer, int *sectorsRead ){
/*
 * 2. Go through the directory trying to match the file name.
 If you do not find it, set the number of sectors read to 0 and return.

3. Using the sector numbers in the directory, load the file, sector by sector, into the buffer array.
 You should add 512 to the buffer address every time you call readSector.
 Make sure to increment the number of sectors read as you go.
 */
    char directory[512];
    int entrySize = 32;
    int fileEntry = 0; //current entry position
    printString("readFile\n");

    readSector(directory, 2); //map at sector 1, directory at sector 2, kernel at sector 3

    
    for (fileEntry = 0; fileEntry<512; fileEntry+=32){

        int match = 0;

        int j;
        for (j=0; j<6; j++){ //loop through six characters of entry
            if (fileName[j] == directory[fileEntry + j]){ //filename char, entry offset plus current char
                match = 1;
            } else {
                match = 0;
                *sectorsRead = 0; //if not found, set the number of sectors read to 0 and return.
                break;
            }
        }

        if (match){
            int k = 6;
            int sector = directory[fileEntry + k];

//            *sectorsRead = 0;
            while(sector != 0){
                readSector(buffer, sector);
                buffer+=512;
                k++;
                sector = directory[fileEntry + k];
//                *sectorsRead+=1;
//                sector = directory[fileEntry + 7];
            }
            *sectorsRead += k - 6;
            break;
        } else *sectorsRead = 0;

    }
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    printString("handle\n");
    if (ax==0){
        printString(bx);
    }
    if (ax==1){
        readString(bx);
    }
    if (ax==2){
        readSector(bx, cx);
    }
    if (ax==3){
        readFile(bx, cx, dx);
    }
    if (ax > 3){
        printString("error");
    }
}
