#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#define NUM_LEN "8"
#define PREFIX_LEN 20
#define OPEN 0
#define EDITOR "subl"

int main(int argc, char *argv[]) {
    FILE *csv, *vcf;
    if (argc > 1) {
        if(!strcmp(argv[1], "-help")) {
            puts("Usage (from command line):\n\tvcf [filename] [prefix]\n\nOptional arguments:\n\t[1] filename -- name of .csv file to read (including extension), eg. my_contacts.csv\n\t\t\t- file must be in the same folder as the program\n\t\t\t- file should not contain any column headers\n\t\t\t- each name,number pair must appear on a new line\n\t\t\t- names should not contain any commas\n\t[2] prefix   -- string to be prefixed to all contact names, eg. office\n\t\t\t- not more than 20 characters \n\nResult\n\tcontacts.vcf will be created/overwritten at the same location as contacts.csv\n\nExample:\n\tvcf my_contacts.csv office --> prefix 'office' to all contacts read from my_contacts.csv\n\nNotes:\n\t[1] When vcf is run without any arguments, it will look for contacts.csv by default\n\t[2] Arguments must be supplied in the correct order\n\t[3] filename can be supplied without prefix, but filename must be provided first if prefix is to be given");
            return 0;
        }
        printf("Reading %s...\n", argv[1]);
        csv = fopen(argv[1], "r");
    }
    else {
        puts("Looking for contacts.csv...");
        csv  = fopen("contacts.csv", "r");
    }
    if (!csv) {
        if (!errno) {
            errno = 2;
        }           
        perror("Error");
        exit(EXIT_FAILURE);
    }
    if (vcf = fopen("contacts.vcf", "r")) {
        puts("contacts.vcf will be overwritten. ENTER to continue...");
        for (int c = 0; (c = getchar()) != EOF && c != '\n';);      
    }
    vcf = fopen("contacts.vcf", "w");
    for (int c = 0; (c = getc(csv)) != EOF;) {
        if (isalpha(c)) {
            int buffer = 50, i = 0;
            char *name = malloc(buffer * sizeof(char));
            if (argc == 3) {
                for (i = 0; argv[2][i] != '\0' && i < PREFIX_LEN; i++) {
                    name[i] = argv[2][i];
                }           
            } 
            name[i++] = ' ';
            name[i++] = c;
            for (int ch = 0; ch = getc(csv);) {
                if (ch == ',') {
                    name[i] = '\0';
                    break;
                }
                name[i] = ch;
                if (++i == buffer-2) {
                    buffer *= 2;
                    char *name_new = realloc(name, buffer);
                    if (name_new) {
                        name = name_new;
                    }
                    else {
                        perror("Error");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            fprintf(vcf, "BEGIN:VCARD\nVERSION:3.0\nFN: %s\nN:; %s\n", name, name);
            goto num;
        }
        else if (isdigit(c)) {
            num:;
            char number[9] = {0};
            fscanf(csv, "%"NUM_LEN"[0-9]", number);
            fprintf(vcf, "TEL;TYPE=CELL:%s\nEND:VCARD\n", number);
        }
        else if (c == '\r' || c == '\n') {
            continue;
        }
    }
    fclose(csv);
    fclose(vcf);
    puts("contacts.vcf has been successfully created.");
    #if OPEN
        system(EDITOR " contacts.vcf");
    #endif
}