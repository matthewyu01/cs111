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

    else if (argc == 2)
    {
        int wstatus;
        int pid = fork();
        if (pid == 0)
        {
            if (execlp(argv[1], argv[1], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0)
        {
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
    }

    else if (argc == 3)
    {
        int fd[2]; // fd[0] for read, fd[1] for write
        if (pipe(fd) == -1)
        {
            return errno;
        }
        int wstatus;

        int pid = fork();

        if (pid == 0) // child process
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
        else if (pid > 0) // parent process
        {
            if (close(fd[1]) == -1)
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }

        // 2nd commnad
        pid = fork();
        if (pid == 0) // child process
        {
            if (dup2(fd[0], STDIN_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[2], argv[2], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) // parent process
        {
            if (close(fd[0]) == -1)
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
    }
    else if (argc == 4)
    {
        int fd[2][2]; // fd[r][0] for read, fd[r][1] for write
        if (pipe(fd[0]) == -1)
        {
            return errno;
        }

        int wstatus;
        int pid = fork();

        if (pid == 0) // child process
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
        else if (pid > 0) // parent process
        {
            if (close(fd[0][1]))
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
        // 2nd command

        if (pipe(fd[1]) == -1)
        {
            return errno;
        }
        pid = fork();
        if (pid == 0) // child process
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
        else if (pid > 0) // parent process
        {
            if (close(fd[0][0]) == -1 || close(fd[1][1]) == -1)
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }

        // 3rd command
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
        else if (pid > 0) // parent process
        {
            if (close(fd[1][0]))
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
    }

    else
    {
        int fd[argc - 2][2]; // fd[r][0] for read, fd[r][1] for write

        // first command
        if (pipe(fd[0]) == -1)
        {
            return errno;
        }

        int wstatus;
        int pid = fork();

        if (pid == 0) // child process
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
        else if (pid > 0) // parent process
        {
            if (close(fd[0][1]) == -1)
            {
                return errno;
            }
            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
        // inner commands
        int i = 1;
        while (i < argc - 2)
        {
            if (pipe(fd[i]))
            {
                return errno;
            }
            pid = fork();
            if (pid == 0) // child process
            {
                if (dup2(fd[i - 1][0], STDIN_FILENO) == -1 || dup2(fd[i][1], STDOUT_FILENO) == -1)
                {
                    return errno;
                }

                if (close(fd[i - 1][0]) == -1 || close(fd[i][0]) == -1 || close(fd[i][1]) == -1)
                {
                    return errno;
                }

                if (execlp(argv[i + 1], argv[i + 1], (char *)NULL) == -1)
                {
                    return errno;
                }
            }
            else if (pid > 0) // parent process
            {
                if (close(fd[i - 1][0]) == -1 || close(fd[i][1]) == -1)
                {
                    return errno;
                }
                if (wait(&wstatus) == -1)
                {
                    return errno;
                }

                if (!WIFEXITED(wstatus))
                {
                    return ECHILD;
                }

                int exit_status = WEXITSTATUS(wstatus);
                if (exit_status)
                {
                    return exit_status;
                }
                i++;
            }
            else
            {
                return errno;
            }
        }

        // last command
        pid = fork();
        if (pid == 0)
        {
            if (dup2(fd[i - 1][0], STDIN_FILENO) == -1)
            {
                return errno;
            }
            if (close(fd[i - 1][0]) == -1)
            {
                return errno;
            }

            if (execlp(argv[i + 1], argv[i + 1], (char *)NULL) == -1)
            {
                return errno;
            }
        }
        else if (pid > 0) // parent process
        {
            if (close(fd[i - 1][0]) == -1)
            {
                return errno;
            }

            if (wait(&wstatus) == -1)
            {
                return errno;
            }

            if (!WIFEXITED(wstatus))
            {
                return ECHILD;
            }

            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status)
            {
                return exit_status;
            }
        }
        else
        {
            return errno;
        }
    }
}
