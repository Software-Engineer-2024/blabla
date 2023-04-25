#include "general.h"

/**
 * mysh_bg - shell job control
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: 0 if successful , -1 otherwise
 */

int mysh_bg(int argc, char **argv)
{
	int job_id = -1;
	pid_t pid;

	if (argc < 2)
	{
		printf("usage: bg <pid>\n");
		return (-1);
	}
	if (argv[1][0] == '%')
	{
		job_id = atoi(argv[1] + 1);
		pid = get_pgid_by_job_id(job_id);
		if (pid < 0)
		{
			printf("./hsh: bg %s: no such job\n", argv[1]);
			return (-1);
		}
	}
	else
	{
		pid = atoi(argv[1]);
	}

	if (kill(-pid, SIGCONT) < 0)
	{
		printf("./hsh: bg %d: job not found\n", pid);
		return (-1);
	}

	if (job_id > 0)
	{
		set_job_status(job_id, STATUS_CONTINUED);
		print_job_status(job_id);
	}

	return (0);
}

/**
 * mysh_kill - kills process
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: 0 if job not found , 1 if successful , -1 otherwise
 */

int mysh_kill(int argc, char **argv)
{
	pid_t pid;
	pid_t job_id;

	if (argc < 2)
	{
		printf("usage: kill <pid>\n");
		return (-1);
	}
	if (argv[1][0] == '%')
	{
		job_id = atoi(argv[1] + 1);
		pid = get_pgid_by_job_id(job_id);
		if (pid < 0)
		{
			printf("./hsh: kill %s: no such job\n", argv[1]);
			return (-1);
		}
		pid = -pid;
	}
	else
		pid = atoi(argv[1]);

	if (kill(pid, SIGKILL) < 0)
	{
		printf("./hsh: kill %d: job not found\n", pid);
		return (0);
	}

	if (job_id > 0)
	{
		set_job_status(job_id, STATUS_TERMINATED);
		print_job_status(job_id);
		if (wait_for_job(job_id) >= 0)
		{
			remove_job(job_id);
		}
	}

	return (1);
}

/**
 * mysh_export - mark variables and functions to be passed to child processes
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: -1 if less than 2 arguments entered , export otherwise
 */

int mysh_export(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: export KEY=VALUE\n");
		return (-1);
	}

	return (putenv(argv[1]));
}

/**
 * mysh_unset - shell removes the variable from list of variables it tracks
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: -1 if less than 2 arguments entered , unset otherwise
 */

int mysh_unset(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: unset KEY\n");
		return (-1);
	}

	return (unsetenv(argv[1]));
}

/**
 * mysh_cd - change directory
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: always 0
 */

int mysh_cd(int argc, char **argv)
{
	struct shell_info *shell = get_shell_info();

	if (argc == 1)
	{
		chdir(shell->pw_dir);
		mysh_update_cwd_info();
		return (0);
	}

	if (chdir(argv[1]) == 0)
	{
		mysh_update_cwd_info();
		return (0);
	}
	else
	{
		printf("./hsh: cd %s: No such file or directory\n", argv[1]);
		return (0);
	}
}
