/*
 * PROGRAM:   cookie-converter
 *
 * DESCRIPTION:
 *     Convert cookies from Chromium based browser into Netscape format.
 *     Application -> Storage -> Cookies
 *
 * USAGE:
 *     cc -o cookie-converter main.c
 *     ./cookie-converter input.txt
 *     or
 *     cat input.txt | ./cookie-converter
 */

#define MAX_FILE_SIZE 4096
#define MAX_COOKIE_LENGTH 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void die(const char *msg)
{
    printf("%s (%d)\n", msg, errno);
    exit(1);
}

void usage(void)
{
    printf("USAGE: ./cookie-converter input.txt\n");
    exit(1);
}

void parse(const char *src)
{
    /* j = index of all cookies*/
    /* l = index of a char in a cookie */
    int i=0, j=0, l=0;
    char result[13][MAX_COOKIE_LENGTH] = {0};
    int year, month, day, hour, min, sec;
    long unix_time;

    char *host,*path,*expiry,*name,*value;

    for (i = 0; src[i]!='\0'; i++) {
        if (l >= MAX_COOKIE_LENGTH-1) die("field too long!");

        if (src[i] == '\t') {
            if (j == 12) die("cookie is too bad!");

            j++;
            result[j][l] = '\0';
            l = 0;
            continue;
        }

        result[j][l] = src[i];
        l++;
    }

    name = result[0];
    value = result[1];
    host = result[2];
    path = result[3];
    expiry = result[4];
    sscanf(expiry, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &min, &sec);
    unix_time = (year - 1970) * 365 * 24 * 3600;
    unix_time += (year - 1970) / 4 * 24 * 3600;
    unix_time += (month - 1) * 30 * 24 * 3600;
    unix_time += (day - 1) * 24 * 3600;
    unix_time += hour * 3600 + min * 60 + sec;

    printf("%s\tTRUE\t%s\tTRUE\t%ld\t%s\t%s\n", host, path, unix_time, name, value);
}

int main(int argc, char **argv)
{
    FILE *fptr;
    char buf[MAX_FILE_SIZE] = {0};

    if (argc>1) {
        fptr = fopen(argv[1], "r");
        if (fptr == NULL) {
            die("The file could not be opened.");
        }
    } else {
        fptr = stdin;
    }

    printf("# Netscape HTTP Cookie File\n");
    while(fgets(buf, MAX_FILE_SIZE, fptr)) {
        parse(buf);
    }

    if (fptr != stdin)
        fclose(fptr);
    return 0;
}
