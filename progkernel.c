#include <stdio.h>
void printString(char[]);
void printChar(char);
void readString(char[]);
void readSector(char*, int);
//void handleInterrupt21(int, int, int, int);


//interrupt(interNum, AX, BX, CX, DX)

int main() {
    char line[80];
    //printString("Hello\n");

    //printString("Enter a line: ");
    //readString(line);
    //printString(line);

//    char buffer[512];
//    readSector(buffer, 30);
//    printString(buffer);

//    makeInterrupt21();
//    interrupt(0x21,0,0,0,0);

    makeInterrupt21();
    interrupt(0x21,1,line,0,0);
    interrupt(0x21,0,line,0,0);


    return 0;
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
}

void readSector(char* buffer, int sector){
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

void handleInterrupt21(int ax, int bx, int cx, int dx){
    if (ax==0){
        char buffer[512];
        readSector(buffer, bx);
        printString(buffer);
    }
    if (ax==1){
        readString(bx);
    }
    if (ax==2){
        readSector(bx, cx);
    }
}
