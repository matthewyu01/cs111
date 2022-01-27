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
            if (dup2(fd[1], STDOUT_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[1]) == -1 || close(fd[0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[1], argv[1], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            close(fd[1]);
            if (wait(&wstatus) == -1)
            {
                return errno;
            }
        }
        else
        {
            return errno;
        }
        //2nd commnad
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
    else if (argc == 10)
    {
        int fd[2][2]; // fd[0] for read, fd[1] for write
        pipe(fd[0]);

        int wstatus;
        int pid = fork();

        if (pid == 0) //child process
        {
            if (dup2(fd[0][1], STDOUT_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[0][1]) == -1 || close(fd[0][0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[1], argv[1], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            if (close(fd[0][1])){
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }
        }
        else
        {
            return errno;
        }
        //2nd command

        pipe(fd[1]);
        pid = fork();
        if (pid == 0) //child process
        {
            if (dup2(fd[0][0], STDIN_FILENO) == -1 || dup2(fd[1][1], STDOUT_FILENO) == -1)
            {
                return errno;
            }

            if (close(fd[0][0]) == -1 || close(fd[0][1]) == -1 || close(fd[1][1]) == -1)
            {
                return errno;
            }

            if (execlp(argv[2], argv[2], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            close(fd[0][0]);
            close(fd[1][1]);
            wait(&wstatus);
        }
        else
        {
            return errno;
        }

        //3rd command
        pid = fork();
        if (pid == 0)
        {
            if (dup2(fd[1][0], STDIN_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[1][0]) == -1)
            {
                return errno;
            }
            if (execlp(argv[3], argv[3], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            close(fd[1][0]);
            wait(&wstatus);
        }
        else
        {
            return errno;
        }
    }

    else
    {
        printf("%d", argc-2);
        int fd[argc - 2][2]; // fd[0] for read, fd[1] for write
        pipe(fd[0]);

        int wstatus;
        int pid = fork();

        if (pid == 0) //child process
        {
            if (dup2(fd[0][1], STDOUT_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[0][1]) == -1 || close(fd[0][0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[1], argv[1], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            if (close(fd[0][1]) == -1)
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }
        }
        else
        {
            return errno;
        }
        //middle commands
        int i = 1;
        while (i < argc - 2)
        {
            printf("%d\n", i);
            pipe(fd[i]);
            pid = fork();
            if (pid == 0) //child process
            {
                if (dup2(fd[i - 1][0], STDIN_FILENO) == -1 || dup2(fd[i][1], STDOUT_FILENO) == -1)
                {
                    return errno;
                }

                if (close(fd[i - 1][0]) == -1 || close(fd[i - 1][1]) == -1 || close(fd[i][1]) == -1)
                {
                    return errno;
                }

                if (execlp(argv[i + 1], argv[i + 1], (char *)NULL) == -1)
                {
                    return errno;
                }
            }
            else if (pid > 0) //parent process
            {
                if (close(fd[i - 1][0]) == -1 || close(fd[i][1]) == -1)
                {
                    return errno;
                }
                wait(&wstatus);
                i++;
            }
            else
            {
                return errno;
            }
        }

        //last command
        pid = fork();
        if (pid == 0)
        {
            if (dup2(fd[2][0], STDIN_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[2][0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[4], argv[4], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) //parent process
        {
            if (close(fd[2][0]) == -1)
            {
                perror("");
                return errno;
            }
            wait(&wstatus);
        }
        else
        {
            return errno;
        }
    }
}
