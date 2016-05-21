#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include "dictionary.h"
#include "configure.h"
#include "crack.h"

int getFileSize(FILE * file)
{
    struct stat fileStat;
    unsigned long long int fileSize = 0;

    fstat(fileno(file), &fileStat);
    fileSize = fileStat.st_size;

    return fileSize;
}

void determineBounds(FILE * fileArr[], int index, long int * partBounds, int ncores, unsigned long partFile)
{
    fseek(fileArr[0], 0, SEEK_END);
    partBounds[ncores - 1] = ftell(fileArr[0]);
    fseek(fileArr[0], 0, SEEK_SET);

    for (int i = 1; i < ncores; i++)
    {
        fileArr[i] = fopen(opt.dictionaries[index], "r");

        long int pos = partFile * i;

        fseek(fileArr[i], pos, SEEK_SET);

        char c = getc(fileArr[i]);

        while(c != '\r' && c != '\n' && c != '\0')
        {
            --pos;
            fseek(fileArr[i], pos, SEEK_SET);
            c = getc(fileArr[i]);
        }

        while(c == '\r' || c == '\n' || c == '\0')
        {
           ++pos;
           fseek(fileArr[i], pos, SEEK_SET);
           c = getc(fileArr[i]);
        }

      fseek(fileArr[i], pos, SEEK_SET);

      partBounds[i - 1] = ftell(fileArr[i]);
    }
}

void * dictionary_thread(void * Arg)
{
    dictionaryThreadArg * args = (dictionaryThreadArg*)Arg;
    char str[conf.maxPswdLenDict];
    int endStr = 0;
    char currentHash[EVP_MAX_MD_SIZE * 2 + 1];

    EVP_MD_CTX * mdctx = NULL;
    const EVP_MD * md = NULL;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    md = EVP_get_digestbyname(typecheck(opt.hash));

    while(ftell(args->file) != args->bound)
    {

        if (found == 1)
        {
            pthread_exit(0);
        }

        fgets(str, conf.maxPswdLenDict, args->file);

        endStr = strlen(str) - 1;

        while(str[endStr] == '\n' || str[endStr] == '\r')
        {
            str[endStr] = '\0';
            --endStr;
        }

        doHash(&mdctx, md, &md_len, &md_value, str);

        for (unsigned int i = 0; i < md_len; i++)
        {
             sprintf(&currentHash[i * 2], "%02x", md_value[i]);
        }

        if (strcmp(currentHash, opt.hash) == 0)
        {
            crackedPswd = malloc(strlen(str) + 1);
            strcpy(crackedPswd, str);
            found = 1;
            pthread_exit(0);
        }
    }
}
