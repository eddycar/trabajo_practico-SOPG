#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_NAME "named_fifo"

const char *logFilePath = "logs.txt";
const char *signalFilePath = "sign.txt";

int main()
{

    char buffer[256];
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1)
    {
        perror("Reader");
        return 1;
    }

    FILE *logFile = fopen(logFilePath, "a");
    FILE *signalFile = fopen(signalFilePath, "a");
    if (!logFile || !signalFile)
    {
        perror("Reader fopen error");
        return 1;
    }

    while (1)
    {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            if (strstr(buffer, "Signal") != NULL)
            {
                // Identify the signal and write it to the corresponding file
                if (strstr(buffer, "Signal 10") != NULL)
                { // SIGUSR1
                    fprintf(signalFile, "SIGN:1\n");
                }
                else if (strstr(buffer, "Signal 12") != NULL)
                { // SIGUSR2
                    fprintf(signalFile, "SIGN:2\n");
                }
                fflush(signalFile);
            }
            else
            {
                fprintf(logFile, "%s", buffer);
                fflush(logFile);
            }
        }
        else if (bytesRead == 0)
        {
            // EOF received, no writers
            fprintf(stderr, "No writers detected for the FIFO. Exiting...\n");
            break;
        }
        else
        {
            // Read error
            perror("read");
            break;
        }
    }

    fclose(logFile);
    fclose(signalFile);
    close(fd);
    return 0;
}