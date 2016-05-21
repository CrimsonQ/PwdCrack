#include <openssl/evp.h>
#include <sys/stat.h>
#include <math.h>
#include "dictionary.h"
#include "bruteforce.h"
#include "options.h"
#include "configure.h"
#include "queue.h"
#include "crack.h"

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
    //printVerbose(opt.verbose, "crack: Dictionary attack started\n");

    for (int j = 0; j < opt.dictCounter; j++)
    {
        //printVerbose(opt.verbose, "crack: Processing dictionary \"%s\"...\n", opt.dictionaries[j]);

        int size = strlen(conf.dictionaryPath) + strlen(opt.dictionaries[j]) + 2;

        char buf[size];
        snprintf(buf, sizeof buf, "%s%s%s", conf.dictionaryPath, "/", opt.dictionaries[j]);
    \
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
    //printVerbose(opt.verbose, "crack: Bruteforce attack started\n");

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

void measureRuntime(pthread_t * threads, int ncores)
{
    int len = opt.bruteMaxLen;

    opt.bruteMaxLen = conf.testIterationsNumber;

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

    if (runtime < 1)
    {
        printf("Approximate runtime: %.02f sec\n", runtime);
    }

    if (runtime > 1 && runtime < 60)
    {
        printf("Approximate runtime: %.0f sec\n", runtime);
    }

    if (runtime > 59 && runtime < 3600)
    {
       printf("Approximate runtime: %.1f min\n", runtime / 60);
    }

    if (runtime > 3599 && runtime < 86400)
    {
        printf("Approximate runtime: %.2f hour(s)\n", runtime / 3600);
    }

    if (runtime > 86399 && runtime < 604800)
    {
        printf("Approximate runtime: %.0f day(s)\n", runtime / 86400);
    }

    opt.bruteMaxLen = len;
}

double convertTime(double seconds)
{

}

/*void crack(pthread_t * threads, int ncores)
{
    if (opt.dictionaryMode)
    {
        int size = strlen(conf.dictionaryPath) + strlen(opt.dictionaries[0]) + 2;

        char buf[size];
        snprintf(buf, sizeof buf, "%s%s%s", conf.dictionaryPath, "/", opt.dictionaries[0]);
\
        FILE * p1_file = fopen(opt.dictionaries[0], "r");

        long int partFile = getFileSize(p1_file) / ncores;

        FILE * fileArr[ncores];
        long int partBounds[ncores];
        dictionaryThreadArg args[ncores];

        fileArr[0] = p1_file;

        determineBounds(fileArr, partBounds, ncores, partFile);

        for (int i = 0; i < ncores; i++){
            args[i].file = fileArr[i];
            args[i].bound = partBounds[i];
        }

        for (int i = 0; i < ncores; i++){
            pthread_create(&threads[i], NULL, dictionary_thread, (void*)&args[i]);
        }
    }

    for (int i = 0; i < ncores; i++)
    {
       pthread_join(threads[i], NULL);
    }

    if (opt.bruteMode && found == 0)
    {
        printf("starting brute mode...\n");

        // создадим очередь:
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
        //puts("Fin2");
    }
}*/
