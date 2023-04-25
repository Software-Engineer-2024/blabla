#include "general.h"
/**
 * main - driver for shell
 * @argc: number of arguments entered on the command line when program started
 * @argv: array of pointers to arrays of character objects
 * Return: EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	int interactive_mode = isatty(STDIN_FILENO);

	mysh_init();

	if (interactive_mode)
	{
		mysh_loop();
	} else
	{
		char *command = NULL;
		size_t bufsize = 0;
		ssize_t nread;

		if (argc < 2)
		{
			fprintf(stderr, "Usage: %s <command>\n", argv[0]);
			exit(EXIT_FAILURE);
		}

		command = argv[1];

		while ((nread = getline(&command, &bufsize, stdin)) != -1)
		{

			mysh_parse_command(command);
		}

		free(command);
	}

	return (EXIT_SUCCESS);
}
