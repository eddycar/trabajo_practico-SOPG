#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_NAME "named_fifo"

//  SIGPIPE signal handler
void sigpipe_handler(int signum) {
    if (signum == SIGPIPE) {
        fprintf(stderr, "No readers detected for the FIFO. Exiting...\n");
        exit(EXIT_FAILURE);
    }
}

// SIGUSR1 and SIGUSR2 signal handler
void signal_handler(int signum)
{
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1)
    {
        perror("Writer");
        exit(EXIT_FAILURE);
    }
    char signal_msg[256];
    snprintf(signal_msg, sizeof(signal_msg), "Signal %d received\n", signum);
    write(fd, signal_msg, strlen(signal_msg) + 1);
    close(fd);
}



int main()
{

    pid_t pid = getpid();
    printf("PID: %d\n", pid);
    // create FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1)
    {
        if (errno == EEXIST)
        {
            printf("%s already exists, using existing fifo \n", FIFO_NAME);
        }
        else
        {
            perror(FIFO_NAME);
            return 1;
        }
    }
    else
    {
        printf("%s created successfully\n", FIFO_NAME);
    }

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
     signal(SIGPIPE, sigpipe_handler);

    char input[256];
    ssize_t bytesWritten;

    printf("waiting for readers...\n");
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1)
    {
        perror("Writer");
        return 1;
    }

    while (1)
    {
        printf("Enter text: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            if (feof(stdin))
            { // Final file validation
                break;
            }
            else
            { // Error validation 
                printf("Error in fgets\n");
                return 1;
            }
        }
        
        if ((bytesWritten =  write(fd, input, strlen(input) + 1)) == -1) // strlen(input) + 1)) to include \0
        {
           perror("Write");
        }
        else
        {
            printf("writer: wrote %zd bytes\n", bytesWritten);
        }
    }
    close(fd);
    return 0;
}