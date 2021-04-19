#include <conio.h>
#include <dirent.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable : 4996)

void copyFile(int num, char* name,int count) {
    char* s = (char*)malloc(1000 * sizeof(char));
    char* number = (char*)malloc(5 * sizeof(char));
    char* rename = (char*)malloc(5 * sizeof(char));
    strcpy(s, "copy as4-supplementary\\");
    itoa(num, number, 10);
    itoa(count, rename, 10);
    strcat(s, number);
    strcat(s, " C:\\Users\\stheo\\source\\repos\\ask4\\ask4\\mailbox\\");
    strcat(s, name);
    strcat(s, "\\");
    strcat(s, rename);
    system(s);
    free(s);
    free(number);
    free(rename);
    
}

int getCount(char* name) {
    int i = 0;
    for (i = 1; i < 145; i++) {
        char* s = (char*)malloc(1000 * sizeof(char));
        strcpy(s, "mailbox\\");
        strcat(s, name);
        strcat(s, "\\");
        char *num = (char*)malloc(5 * sizeof(char));
        itoa(i, num, 10);
        strcat(s, num);
        FILE* f = fopen(s, "rt");
        if (f == NULL) {
            
            return i;
        }
        fclose(f);
    }

}
void makeDirectory(char* name) {
    char* s = (char*)malloc(1000 * sizeof(char));
    strcpy(s, "mailbox\\");
    strcat(s, name);
    DIR* dir = opendir(s);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    }
    else { _mkdir(s); }
    
}
void createMailbox() {
    _mkdir("mailbox");
    int i;
    for (i = 1; i < 145; i++) {
        char* file = (char*)malloc(5 * sizeof(char));
        itoa(i, file, 10);
        char* s = (char*)malloc(1000 * sizeof(char));
        strcpy(s, "as4-supplementary\\");
        strcat(s, file);
        FILE* f = fopen(s, "rt");
        if (f == NULL) {

        }
        else {
            int count = 0;
            char* line = (char*)malloc(1000 * sizeof(char));
            while (count < 3) {
                fgets(line, "%*[ \n]", f);
                count++;
            }
            while (fgetc(f) != ' ') {

            }


            char c = 'a';
            while (c != ':') {
                int check = 0;
                int i = 0;
                char  temp[1000];
                char c = fgetc(f);
                while (c != ' ' && c != '\n' && c != ',' && c != '\t') {
                    check = 1;
                    temp[i] = c;
                    i++;
                    c = fgetc(f);
                    if (c == ':') break;
                }
                if (c == ':')break;
                if (check == 1) {
                    temp[i] = '\0';
                    makeDirectory(temp);
                    int count = getCount(temp);
                    copyFile(i, temp, count);
                }
                
            }

            free(line);
            fclose(f);
        }
        free(s);
        free(file);
        
    }
}


int main() {
    createMailbox();
   
}