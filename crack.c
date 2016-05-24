#include <openssl/evp.h>
#include <sys/stat.h>
#include <math.h>
#include "dictionary.h"
#include "bruteforce.h"
#include "options.h"
#include "configure.h"
#include "queue.h"
#include "crack.h"

int test = 0;

void doHash(EVP_MD_CTX ** mdctx, const EVP_MD * md, unsigned int * md_len,
            unsigned char ** md_value, const char * pswd)
{
    *mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(*mdctx, md, NULL);
    EVP_DigestUpdate(*mdctx, pswd, strlen(pswd));
    EVP_DigestFinal_ex(*mdctx, md_value, md_len);
    EVP_MD_CTX_destroy(*mdctx);
}

void dictionaryAttack(pthread_t * threads, int ncores)
{
    for (int j = 0; j < opt.dictCounter; j++)
    {
        //int size = strlen(conf.dictionaryPath) + strlen(opt.dictionaries[j]) + 2;
        //char buf[size];
        //snprintf(buf, sizeof buf, "%s%s%s", conf.dictionaryPath, "/", opt.dictionaries[j]);

        FILE * p1_file = fopen(opt.dictionaries[j], "r");

        unsigned long int partFile = getFileSize(p1_file) / ncores;

        FILE * fileArr[ncores];
        long int partBounds[ncores];
        dictionaryThreadArg args[ncores];

        fileArr[0] = p1_file;

        determineBounds(fileArr, j, partBounds, ncores, partFile);

        for (int i = 0; i < ncores; i++)
        {
            args[i].file = fileArr[i];
            args[i].bound = partBounds[i];
        }

        for (int i = 0; i < ncores; i++)
        {
            pthread_create(&threads[i], NULL, dictionary_thread, (void*)&args[i]);
        }

        for (int i = 0; i < ncores; i++)
        {
           pthread_join(threads[i], NULL);
        }
    }
}

void bruteforceAttack(pthread_t * threads, int ncores)
{
    struct producer_consumer_queue * q = producer_consumer_queue_create();

    for (int i = 0; i < ncores; i++)
    {
        pthread_create(&threads[i], 0, brute_thread, (void *)q);
    }

    generateWords(&q);

    producer_consumer_queue_stop(q);

    for (int i = 0; i < ncores; i++)
    {
       pthread_join(threads[i], NULL);
    }

}

void measureBruteRuntime(pthread_t * threads, int ncores)
{
    int len = opt.bruteMaxLen;
    int verbosity = opt.verbose;

    opt.bruteMaxLen = conf.testIterationsNumber;
    opt.verbose = 0;

    test = 1;

    struct timespec startTimer, finishTimer;
    long double elapsed;

    clock_gettime(CLOCK_REALTIME, &startTimer);

    bruteforceAttack(threads, ncores);

    clock_gettime(CLOCK_REALTIME, &finishTimer);

    elapsed = (finishTimer.tv_sec - startTimer.tv_sec);
    elapsed += (finishTimer.tv_nsec - startTimer.tv_nsec) / 1000000000.0;

    double count = pow(strlen(conf.alphabet), conf.testIterationsNumber);
    double speed = count /  elapsed;
    double runtime = pow(strlen(conf.alphabet), len) / speed;

    printf("Cracking may take up to ");
    printConvertedTime(runtime);

    test = 0;

    opt.bruteMaxLen = len;
    opt.verbose = verbosity;
    progress = 0;
}

void measureDictRuntime(pthread_t * threads, int ncores)
{
    char dictionary[strlen(opt.dictionaries[0])];
    strcpy(dictionary, opt.dictionaries[0]);
    int counter = opt.dictCounter;
    int verbosity = opt.verbose;

    strcpy(opt.dictionaries[0], "test_dictionary.txt");
    opt.dictCounter = 1;
    opt.verbose = 0;

    test = 1;

    FILE * pfile = fopen(opt.dictionaries[0], "r");

    unsigned long int size = getFileSize(pfile);

    struct timespec startTimer, finishTimer;
    long double elapsed;

    clock_gettime(CLOCK_REALTIME, &startTimer);

    dictionaryAttack(threads, ncores);

    clock_gettime(CLOCK_REALTIME, &finishTimer);

    elapsed = (finishTimer.tv_sec - startTimer.tv_sec);
    elapsed += (finishTimer.tv_nsec - startTimer.tv_nsec) / 1000000000.0;

    fclose(pfile);
\
    double speed = size /  elapsed;

    if(chdir(conf.dictionaryPath) != 0)
    {
       printf("Unable to open directory %s\n", conf.dictionaryPath);
    }

    pfile = fopen(dictionary, "r");

    size = getFileSize(pfile);

    double runtime = size / speed;

    printf("Cracking may take up to ");
    printConvertedTime(runtime);

    test = 0;
    strcpy(opt.dictionaries[0] , dictionary);
    opt.dictCounter = counter;
    opt.verbose = verbosity;
    progress = 0;
}

void printConvertedTime(double seconds)
{
    if (seconds < 1)
    {
        printf("%.02f sec\n", seconds);
    }

    if (seconds > 1 && seconds < 60)
    {
        printf("%.0f sec\n", seconds);
    }

    if (seconds > 59 && seconds < 3600)
    {
       printf("%.1f min\n", seconds / 60);
    }

    if (seconds > 3599 && seconds < 86400)
    {
        printf("%.2f hour(s)\n", seconds / 3600);
    }

    if (seconds > 86399 && seconds < 604800)
    {
        printf("%.0f day(s)\n", seconds / 86400);
    }

    if (seconds > 604799)
    {
        printf("%.0f weeks(s)\n", seconds / 604800);
    }

}

