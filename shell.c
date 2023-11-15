void typeCommand(char[]);

int main(){
    char input[80];
    while(1){
        syscall(0, "SH>");
        input[80];
        syscall(1, *input);
        //try to match
        if (input[0]=="t" && input[1]=="y" && input[2]=="p" && input[3]=="e")
            typeCommand(input);
            

        //syscall(0, "bad command\r\n");
    }
}

/*
0 printString
1 readString
2 readSector
3 readFile
4 executeProgram
5 terminate
*/

void typeCommand(char command[]){
    char buffer[13312];
    char filename[7];
    int startname = 5;
    int i = 0;
    
    for(i=0; i<6; i++){
        filename[i] = command[i+startname];    
    }
    filename[6] = "\0";

    syscall(3, filename, buffer, 0);
    syscall(0, command, 0, 0);
}
