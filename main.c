#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "configure.h"
#include "bruteforce.h"
#include "dictionary.h"
#include "options.h"
#include <crack.h>
#include <openssl/evp.h>
#include <sys/stat.h>
#include <time.h>

int found = 0;
char * crackedPswd = NULL;

int main(int argc, char *argv[])
{
    if (getConfig(&conf, CONF_FILENAME) == 1)
    {
        printf("Failed to open config file\n");
        return 1;
    }

    if (argc == 1) //если без аргументов - выводим справку
    {
        help();
        return 0;
    }

    setDefaults();
    getOptions(argc, argv);

    int ncores = 1;
    getNumCores(&ncores);

    OpenSSL_add_all_digests();

    pthread_t * threads = (pthread_t*)malloc(sizeof(pthread_t)*ncores);

    if (opt.verbose == 2 || opt.verbose == 3)
    {
        measureRuntime(threads, ncores);
    }

    struct timespec startTimer, finishTimer;
    double elapsed;

    clock_gettime(CLOCK_REALTIME, &startTimer);

    if (opt.dictionaryMode)
    {
        dictionaryAttack(threads, ncores);
    }

    if (opt.bruteMode && found == 0)
    {
        bruteforceAttack(threads, ncores);
    }

    clock_gettime(CLOCK_REALTIME, &finishTimer);

    if (opt.verbose == 0)
    {
        printf("Return code: %d\n", found);
    }
    else
    {
        if (found == 1)
        {
            printf("Found: %s\n", crackedPswd);
            free(crackedPswd);
        }
        else
        {
            printf("Nothing found\n");
        }

        if (opt.verbose == 2)
        {
            elapsed = (finishTimer.tv_sec - startTimer.tv_sec);
            elapsed += (finishTimer.tv_nsec - startTimer.tv_nsec) / 1000000000.0;
            printf("Elapsed time: %f\n", elapsed);
        }

    }
    return 0;
}
