#include "general.h"
void sigint_handler(int sig)
{
    printf("\n");
	if (sig == 0)
	{
	}
	exit(EXIT_SUCCESS);
}
/**
 * get_shell_info - created and initialized only once
 * Return:a pointer to the same global shell_info struct
 */
struct shell_info *get_shell_info(void)
{
	static struct shell_info shell;

	return (&shell);
}
/**
 * mysh_execute_builtin_command - checks process type
 * to execute correct command
 * @proc: incoming process
 * Return: status
 */

int mysh_execute_builtin_command(struct process *proc)
{
	int status = 1;

	switch (proc->type)
	{
		case COMMAND_EXIT:
			mysh_exit();
			break;
		case COMMAND_CD:
			mysh_cd(proc->argc, proc->argv);
			break;
		case COMMAND_JOBS:
			mysh_jobs();
			break;
		case COMMAND_FG:
			mysh_fg(proc->argc, proc->argv);
			break;
		case COMMAND_BG:
			mysh_bg(proc->argc, proc->argv);
			break;
		case COMMAND_KILL:
			mysh_kill(proc->argc, proc->argv);
			break;
		case COMMAND_EXPORT:
			mysh_export(proc->argc, proc->argv);
			break;
		case COMMAND_UNSET:
			mysh_unset(proc->argc, proc->argv);
			break;
		default:
			status = 0;
			break;
	}

	return (status);
}

/**
 * get_command_type - checks the type of incoming command
 * to perform the right operation
 * @command: pointer to the incomming command
 * Return: type of command
 */

int get_command_type(char *command)
{
	if (strcmp(command, "exit") == 0)
	{
		return (COMMAND_EXIT);
	}
	else if (strcmp(command, "cd") == 0)
	{
		return (COMMAND_CD);
	}
	else if (strcmp(command, "jobs") == 0)
	{
		return (COMMAND_JOBS);
	}
	else if (strcmp(command, "fg") == 0)
	{
		return (COMMAND_FG);
	}
	else if (strcmp(command, "bg") == 0)
	{
		return (COMMAND_BG);
	}
	else if (strcmp(command, "kill") == 0)
	{
		return (COMMAND_KILL);
	}
	else if (strcmp(command, "export") == 0)
	{
		return (COMMAND_EXPORT);
	}
	else if (strcmp(command, "unset") == 0)
	{
		return (COMMAND_UNSET);
	}
	else
	{
		return (COMMAND_EXTERNAL);
	}
}

/**
 * mysh_exit - close the shell
 * Return: always 0
 */

int mysh_exit(void)
{
	exit(0);
}
