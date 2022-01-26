#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return EINVAL;
    }
    // int pipe(int fds[2]);

    // dup2(fd,oldfd);
    // char* cmd;

    else if (argc == 2)
    {
        int pid = fork();
        if (pid == 0)
        {
            execlp(argv[1], argv[1], (char *)NULL);
        }
        else if (pid > 0)
        {
            wait(NULL);
        }
        else
        {
            return errno;
        }
    }

    else if (argc == 3)
    {
        int fd[2]; // fd[0] for read, fd[1] for write
        pipe(fd);
        int wstatus;

        int pid = fork();

        if (pid == 0) //child process
        {
            if (dup2(fd[1], STDOUT_FILENO) == -1){
                return errno;
            }
            if (close(fd[1]) == -1 || close(fd[0]) == -1){
                return errno;
            }

            if (execlp(argv[1], argv[1], (char *)NULL) == -1){
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            close(fd[1]);
            if (wait(&wstatus) == -1){
                return errno;
            }
        }
        else
        {
            return errno;
        }

        pid = fork();
        if (pid == 0) //child process
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);

            execlp(argv[2], argv[2], (char *)NULL);
        }
        else if (pid > 0) //parent process
        {
            close(fd[0]);
            wait(&wstatus);
        }
        else
        {
            return errno;
        }
    }
    else if (argc == 4)
    {
    }
}
