/*
* @Author : Stefanos Theodosiou
* @Date   : 20/04/2021
* @Description: The following program, gets a folder that contains emails files inside of it
*               (must be included in the same directory as the program) and creates a "mailbox" directory, with
*               directories of every user mentioned in the email files, and every directory containing all the
*               emails that were sent to the certain user.
*/
#include <conio.h>
#include <dirent.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable : 4996)

/*
 * This function copies a file from as4-supplementary, to the persons mailbox (name) and
 * sets the name of the file, according to how many mails that person has.
 *
 * @param num : number of file (name) to copy
 * @param name: email of the person to copy the file into his/her mailbox
 * @param count: how many mails he/she has in mailbox currently +1
 * @param currentdir: the path of the directory our program runs on.
 */
void copyFile(int num, char* name,  char* currentdir) {
    char* s = (char*)malloc(1000 * sizeof(char));
    char* number = (char*)malloc(5 * sizeof(char));
    strcpy(s, "cp as4-supplementary\\");
    itoa(num, number, 10);
    strcat(s, number);
    strcat(s, " ");
    strcat(s, currentdir);
    strcat(s, "\\mailbox\\");
    strcat(s, name);
    system(s);
    free(s);
    free(number);


}

/*
 *   This function , checks how many "mails" a person has in his/her mailbox, by trying to open the file
 *   and when it files the file that can't open , returns the name of that (number) as a result.
 *
 *   @param name: the name of the person's mailbox.
 *   @return int: how many mails the mailbox has +1 (for the next file's name to be set)
*/
int getCount(char* name) {
    int i = 0;
    int count=0;
    for (i = 1; i < 145; i++) {
        char* s = (char*)malloc(1000 * sizeof(char));
        strcpy(s, "mailbox\\");
        strcat(s, name);
        strcat(s, "\\");
        char* num = (char*)malloc(5 * sizeof(char));
        itoa(i, num, 10);
        strcat(s, num);
        FILE* f = fopen(s, "rt");
        if (f == NULL) {
        }else{
            count++;
            fclose(f);
        }
        free(s);
    }
    return count;

}

/*
 *   This function, creates a new directory under the mailbox directory. If the directory already exsists
 *   it does nothing .
 *
 *   @param name: the person's email to create the directory.
*/
void makeDirectory(char* name) {
    char* s = (char*)malloc(1000 * sizeof(char));
    strcpy(s, "mailbox\\");
    strcat(s, name);
    DIR* dir = opendir(s);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    }
    else { mkdir(s); }

}

/*
*   This function , creates the mailbox directory and every directory , with it's according files in it.
*   It goes over all the files in the supplementary folder, checks the "To:" field of the files,
*   and gets the emails of the receiver of emails.After it creates the directories of each user found in
*   every file (if needed) and copies every file that had each person as a receiver ("To:") in it.
*
*/
void createMailbox() {
    mkdir("mailbox");
    char* currentdir = (char*)malloc(1000 * sizeof(char));
    getcwd(currentdir, 1000);
    printf("%s", currentdir);
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
                    if (c == '@') {
                        check = 1;
                    }
                    temp[i] = c;
                    i++;
                    c = fgetc(f);
                    if (c == ':') break;
                }
                if (c == ':')break;
                if (check == 1) {
                    temp[i] = '\0';
                    makeDirectory(temp);
                    copyFile(i, temp, currentdir);
                }

            }

            free(line);
            fclose(f);
        }
        free(s);
        free(file);

    }
    free(currentdir);
}


int main() {
    createMailbox();


}
