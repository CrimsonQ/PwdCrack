#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include "configure.h"
#include "options.h"
#include "configure.h"

void help()
{
    printf("Usage:\n    crack [hash] [options]\n");
    printf("or: crack [options]\n\n");
    printf("A multithreaded hashed password cracker.\n\n");
    printf("Options:\n");
    printf("-d, --dictionary=FILENAME\n    add dictionary file with name FILENAME (from the default folder\n"
           "    which can be changed in the config file) and start dictionary attack\n\n");
    printf("-b, --bruteforce=MAX_SYMBOLS_NUM\n    start bruteforcing up to MAX_SYMBOLS_NUM symbols\n\n");
    printf("-v, --verbose=LEVEL\n    set level of output:\n");
    printf("    0 - show only return code; 1 - cracked password;\n"
           "    2 - password and elapsed time; 3 - password, time and progress\n\n");
    printf("-h, --help\n    show help\n\n");
}

void setDefaults()
{
    opt.bruteMode = false;
    opt.dictionaryMode = false;
    opt.verbose = 0;
    opt.dictionaries = malloc(sizeof(char*) * conf.maxDict);
    opt.bruteMaxLen = 3;
    opt.dictCounter = 0;
}

int add(char ** list, char * str, int * counter, const int maxNum)
{
    if (*counter < maxNum)
    {
        list[*counter] = malloc(strlen(str) + 1);
        strcpy(list[*counter], str);
        (*counter)++;
    }
    else
    {
        return -1;
    }
    return 0;
}

void getOptions(int argc, char ** argv)
{
    // Если пользователь не ввел хеш опцией
    if (strcmp(typecheck(argv[1]), "UNKNOWN") == 0)
    {
        getHashStdin();
    }
    else
    {
        strcpy(opt.hash, argv[1]);
    }

    int getopt_rez = 0;
    int longIndex = 0;

    while ( (getopt_rez = getopt_long(argc, argv, ":b:v:hl:d:", longOpts, &longIndex)) != -1)
    {
        switch (getopt_rez){
        case 'b':
            opt.bruteMode = true;
            opt.bruteMaxLen = atoi(optarg);
            break;

        case 'd':
            opt.dictionaryMode = true;
            if (add(opt.dictionaries, optarg, &opt.dictCounter, conf.maxDict) == -1)
            {
                printf("Only %d dictionaries were added.\n", conf.maxDict);
                printf("You can change the max allowed number in the conf file\n");
            }
            break;

        case 'v':
            if (atoi(optarg) >= 0 && atoi(optarg) <= 3)
            {
                opt.verbose = atoi(optarg);
            }
            else
            {
                fprintf(stderr, "Wrong input! Choose 0, 1, 2 or 3 level\n");
            }
            break;

        case 'h':
            help();
            break;

        case ':':
            fprintf(stderr, "Option '-%c' requires an argument\n", optopt);
            break;

        case '?':
            fprintf(stderr, "Invalid option '-%c'\n", optopt);
            break;
        };
    };
    // Если пользователь не выбрал ни один способ
    if (opt.bruteMode == false && opt.dictionaryMode == false)
    {
        opt.bruteMode = true;
    }
}

void getHashStdin()
{
    char buf[EVP_MAX_MD_SIZE  * 2 + 1];

    printf("Input a hash: ");

    while (scanf("%s", buf) == 1)
    {
        if (strcpy(typecheck(buf),"UNKNOWN") == 0)
        {
            fprintf(stderr, "Unknown type of hash: %s. Input again:\n", buf);
        }
        else
        {
            strcpy(opt.hash, buf);
            break;
        }
    }
}

/*int getHashesStdin(char ** hashes)
{
    int counter = 0;
    char buf[EVP_MAX_MD_SIZE  * 2 + 1];

    printf("Input up to %d hashes (Ctrl + D to stop):\n", conf.maxHashes);

    while (counter < conf.maxHashes && scanf("%s", buf) == 1)
    {
        if (typecheck(buf) == "UNKNOWN")
        {
            fprintf(stderr, "Unknown type of hash: %s. Input another:\n", buf);
        }
        else
        {
            hashes[counter] = (char*)malloc(strlen(buf) + 1);
            strcpy(hashes[counter], buf);
            counter++;
        }
    }
    return counter;
}*/

char * typecheck(char * hash)
{
    int len = strlen(hash);

    switch(len)
    {
    case MD5_LENGTH:
        return "MD5";
        break;
    case SHA1_LENGTH:
        return "SHA1";
        break;
    case SHA256_LENGTH:
        return "SHA256";
        break;
    case SHA512_LENGTH:
        return "SHA512";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

void printVerbose(int verbosity, const char * format, ...)
{
    va_list args;
    va_start(args, format);

    if (verbosity == 1)
    {
        vfprintf(stdout, format, args);
    }
}
