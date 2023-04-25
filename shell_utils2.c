#include "general.h"

/**
 * mysh_fg -  removes job's process ID from list of those known by shell
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: 0 if successful , -1 otherwise
 */
int mysh_fg(int argc, char **argv)
{
	pid_t pid;
	int job_id = -1;

	if (argc < 2)
	{
		printf("usage: fg <pid>\n");
		return (-1);
	}

	if (argv[1][0] == '%')
	{
		job_id = atoi(argv[1] + 1);
		pid = get_pgid_by_job_id(job_id);
		if (pid < 0)
		{
			printf("./hsh: fg %s: no such job\n", argv[1]);
			return (-1);
		}
	}
	else
		pid = atoi(argv[1]);
	if (kill(-pid, SIGCONT) < 0)
	{
		printf("./hsh: fg %d: job not found\n", pid);
		return (-1);
	}
	tcsetpgrp(0, pid);
	if (job_id > 0)
	{
		set_job_status(job_id, STATUS_CONTINUED);
		print_job_status(job_id);
		if (wait_for_job(job_id) >= 0)
			remove_job(job_id);
	}
	else
		wait_for_pid(pid);
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(0, getpid());
	signal(SIGTTOU, SIG_DFL);
	return (0);
}

/**
 * mysh_jobs - shell job status printer
 * Return: 0
 */
int mysh_jobs(void)
{
	int i;
	struct shell_info *shell = get_shell_info();

	for (i = 0; i < NR_JOBS; i++)
	{
		if (shell->jobs[i] != NULL)
		{
			print_job_status(i);
		}
	}

	return (0);
}
/**
 * helper_strtrim - trims white spaces from front and end of input
 * @line: input to shell
 * Return: head
 */

char *helper_strtrim(char *line)
{
	char *head = line, *tail = line + strlen(line);

	while (*head == ' ')
	{
		head++;
	}
	while (*tail == ' ')
	{
		tail--;
	}
	*(tail + 1) = '\0';

	return (head);
}
/**
 * mysh_loop - main loop for shell
 * Return: void
 */
void mysh_loop(void)
{
	char *line;
	struct job *job;
	int status;


	while (1)
	{
		mysh_print_promt();
		line = mysh_read_line();
		if (strlen(line) == 0)
		{
			check_zombie();
			continue;
		}

		job = mysh_parse_command(line);
		status = mysh_launch_job(job);
	}
	if (status == 0)
	{
	};
}
