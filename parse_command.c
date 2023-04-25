#include "general.h"

/**
 * myshe_parse_command_segment -parses a single command segment 
 * @segmant: pointer to command line.
 * Return:pointer to struct process 
 */
struct process* mysh_parse_command_segment(char *segment) {
	struct process *new_proc = (struct process*) malloc(sizeof(struct process));
	int i = 0, argc = 0;
	int bufsize = TOKEN_BUFSIZE;
	int position = 0;
	char *command = strdup(segment);
	char *token;
	char *input_path;
	char *output_path;
	char **tokens = (char**) malloc(bufsize * sizeof(char*));

	if (!tokens) {
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(segment, TOKEN_DELIMITERS);
	while (token != NULL) {
		glob_t glob_buffer;
		int glob_count = 0;

		if (strchr(token, '*') != NULL || strchr(token, '?') != NULL) {
			glob(token, 0, NULL, &glob_buffer);
			glob_count = glob_buffer.gl_pathc;
		}

		if (position + glob_count >= bufsize) {
			bufsize += TOKEN_BUFSIZE;
			bufsize += glob_count;
			tokens = (char**) realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "mysh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		if (glob_count > 0) {
			int i;
			for (i = 0; i < glob_count; i++) {
				tokens[position++] = strdup(glob_buffer.gl_pathv[i]);
			}
			globfree(&glob_buffer);
		} else {
			tokens[position] = token;
			position++;
		}

		token = strtok(NULL, TOKEN_DELIMITERS);
	}
	input_path = NULL, output_path = NULL;
	while (i < position) {
		if (tokens[i][0] == '<' || tokens[i][0] == '>') {
			break;
		}
		i++;
	}
	argc = i;

	for (; i < position; i++) {
		if (tokens[i][0] == '<') {
			if (strlen(tokens[i]) == 1) {
				input_path = (char *) malloc((strlen(tokens[i + 1]) + 1) * sizeof(char));
				strcpy(input_path, tokens[i + 1]);
				i++;
			} else {
				input_path = (char *) malloc(strlen(tokens[i]) * sizeof(char));
				strcpy(input_path, tokens[i] + 1);
			}
		} else if (tokens[i][0] == '>') {
			if (strlen(tokens[i]) == 1) {
				output_path = (char *) malloc((strlen(tokens[i + 1]) + 1) * sizeof(char));
				strcpy(output_path, tokens[i + 1]);
				i++;
			} else {
				output_path = (char *) malloc(strlen(tokens[i]) * sizeof(char));
				strcpy(output_path, tokens[i] + 1);
			}
		} else {
			break;
		}
	}

	for (i = argc; i <= position; i++) {
		tokens[i] = NULL;
	}
	new_proc->command = command;
	new_proc->argv = tokens;
	new_proc->argc = argc;
	new_proc->input_path = input_path;
	new_proc->output_path = output_path;
	new_proc->pid = -1;
	new_proc->type = get_command_type(tokens[0]);
	new_proc->next = NULL;
	return new_proc;
}

/**
 * mysh_parse_command - parses a command line into a job structure,
 * 		where each process is a segment separated by a '|' symbol.
 * @line:pointer to a character array represents user input command string
 * Returen:pointer to struct job represents the parsed command with all details
 */
struct job* mysh_parse_command(char *line) {
	char *line_cursor;
	char *c;
	char *seg;
	struct job *new_job;
	int seg_len;
	int mode;
	struct process *root_proc;
	struct process *proc;
	char *command = strdup(line);
	struct process* new_proc;
	line = helper_strtrim(line);

	root_proc = NULL, proc = NULL;
	line_cursor = line, c = line;
	seg_len = 0, mode = FOREGROUND_EXECUTION;

	if (line[strlen(line) - 1] == '&') {
		mode = BACKGROUND_EXECUTION;
		line[strlen(line) - 1] = '\0';
	}

	while (1) {
		if (*c == '\0' || *c == '|') {
			seg = (char*) malloc((seg_len + 1) * sizeof(char));
			strncpy(seg, line_cursor, seg_len);
			seg[seg_len] = '\0';

			new_proc = mysh_parse_command_segment(seg);
			if (!root_proc) {
				root_proc = new_proc;
				proc = root_proc;
			} else {
				proc->next = new_proc;
				proc = new_proc;
			}

			if (*c != '\0') {
				line_cursor = c;
				while (*(++line_cursor) == ' ');
				c = line_cursor;
				seg_len = 0;
				continue;
			} else {
				break;
			}
		} else {
			seg_len++;
			c++;
		}
	}

	new_job = (struct job*) malloc(sizeof(struct job));
	new_job->root = root_proc;
	new_job->command = command;
	new_job->pgid = -1;
	new_job->mode = mode;
	new_job->command_count+=1;
	return new_job;
}
