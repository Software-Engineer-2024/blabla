#include "general.h"
/**
 * mysh_init - initialization of shell
 * Return: void
 */
void mysh_init(void)
{
	int pid = 0;
	int i;
	struct shell_info *shell = get_shell_info();
	struct passwd *pw;
	struct sigaction sigint_action;



	sigint_action.sa_handler = sigint_handler;
	sigemptyset(&sigint_action.sa_mask);
	sigint_action.sa_flags = 0;
	sigaction(SIGINT, &sigint_action, NULL);


	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);

	pid = getpid();
	setpgid(pid, pid);
	tcsetpgrp(0, pid);

	shell = (struct shell_info *)malloc(sizeof(struct shell_info));
	getlogin_r(shell->cur_user, sizeof(shell->cur_user));

	pw = getpwuid(getuid());

	strcpy(shell->pw_dir, pw->pw_dir);
	for (i = 0; i < NR_JOBS; i++)
	{
		shell->jobs[i] = NULL;
	}

	mysh_update_cwd_info();
}
/**
 * mysh_update_cwd_info - updates current working directory info
 * Return: 0 if successful , -1 otherwise
 */
void mysh_update_cwd_info(void)
{
	struct shell_info *shell = get_shell_info();

	getcwd(shell->cur_dir, sizeof(shell->cur_dir));
}

/**
 * mysh_read_line - reads input turning it to char array
 * Return: buffer
 */
char *mysh_read_line(void)
{
	int bufsize = COMMAND_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer)
	{
		fprintf(stderr, "./hsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		c = getchar();

		if (c == EOF || c == '\n')
		{
			buffer[position] = '\0';
			return (buffer);
		}
		else
		{
			buffer[position] = c;
		}
		position++;

		if (position >= bufsize)
		{
			bufsize += COMMAND_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer)
			{
				fprintf(stderr, "./hsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}
/**
 * mysh_print_promt - prints promt before a command
 * Return: void
 */
void mysh_print_promt(void)
{
	printf("$ ");
}
