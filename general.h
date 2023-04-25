#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <glob.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


#define NR_JOBS 20
#define PATH_BUFSIZE 1024
#define COMMAND_BUFSIZE 1024
#define TOKEN_BUFSIZE 64
#define TOKEN_DELIMITERS " \t\r\n\a"

#define BACKGROUND_EXECUTION 0
#define FOREGROUND_EXECUTION 1
#define PIPELINE_EXECUTION 2

#define COMMAND_EXTERNAL 0
#define COMMAND_EXIT 1
#define COMMAND_CD 2
#define COMMAND_JOBS 3
#define COMMAND_FG 4
#define COMMAND_BG 5
#define COMMAND_KILL 6
#define COMMAND_EXPORT 7
#define COMMAND_UNSET 8

#define STATUS_RUNNING 0
#define STATUS_DONE 1
#define STATUS_SUSPENDED 2
#define STATUS_CONTINUED 3
#define STATUS_TERMINATED 4

#define PROC_FILTER_ALL 0
#define PROC_FILTER_DONE 1
#define PROC_FILTER_REMAINING 2

/**
 * struct process - the process object.
 * @command:The command to be executed
 * @argc:The number of arguments in argv (including the command)
 * @argv:An array of strings representing the arguments to the command
 * @input_path:The path to the file that will be used as input for the command
 * @output_path:path to the file that will be used as output for the command
 * @pid:The process ID (PID) of the process that executes the command
 * @type:The type of the process (e.g., foreground or background)
 * @status:The status of the process (e.g., running, stopped, or terminated)
 * @next:A pointer to the next process in a linked list of processes
 * description:represents info about a process will be executed by program.
 */
struct process
{
	char *command;
	int argc;
	char **argv;
	char *input_path;
	char *output_path;
	pid_t pid;
	int type;
	int status;
	struct process *next;
};

/**
 * struct job - job object
 * @id:The ID of the job
 * @root:pointer to first process in the linked list of processes for the job
 * @command:The command to be executed by the job
 * @command_count: Number of commands executed.
 * @pgid:The process group ID (PGID) of the job
 * @mode:The execution mode of the job (e.g., foreground or background)
 * description:represents job contains one or more processes to be executed
 */
struct job
{
	int id;
	struct process *root;
	char *command;
	pid_t pgid;
	int command_count;
	int mode;
};

/**
 * struct shell_info - stores information about the current shell session
 * @cur_user:The name of the current user
 * @cur_dir:The path to the current directory
 * @pw_dir:The path to the present working directory
 * @jobs:An array of pointers to job structures, including a null terminator
 * Description: including the current user, current
 * directory, present working directory, and an array of job structures.
 */
struct shell_info
{
	char cur_user[TOKEN_BUFSIZE];
	char cur_dir[PATH_BUFSIZE];
	char pw_dir[PATH_BUFSIZE];
	struct job *jobs[NR_JOBS + 1];
};
struct shell_info *get_shell_info(void);
/* process_utils.c */
int get_proc_count(int id, int filter);
int print_processes_of_job(int id);
int set_process_status(int pid, int status);
int wait_for_pid(int pid);
void check_zombie(void);

/* job_utils.c */
int get_job_id_by_pid(int pid);
struct job *get_job_by_id(int id);
int get_pgid_by_job_id(int id);
int get_next_job_id(void);
int wait_for_job(int id);

/* job_status.c */
int print_job_status(int id);
int insert_job(struct job *job);
int is_job_completed(int id);
int set_job_status(int id, int status);
int remove_job(int id);

/* parse_command.c */
struct job *mysh_parse_command(char *line);
struct process *mysh_parse_command_segment(char *segment);

/* shell_launch.c */
int mysh_launch_job(struct job *job);
int mysh_launch_process(struct job *job,
		struct process *proc, int in_fd, int out_fd, int mode);
int release_job(int id);

/* execute.c */
int mysh_execute_builtin_command(struct process *proc);
int get_command_type(char *command);
int mysh_exit(void);
struct shell_info *get_shell_info(void);
void sigint_handler(int sig);

/* shell.c */
void mysh_init(void);
void mysh_update_cwd_info(void);
char *mysh_read_line(void);
void mysh_print_promt(void);

/* shell_utils.c */
int mysh_bg(int argc, char **argv);
int mysh_kill(int argc, char **argv);
int mysh_export(int argc, char **argv);
int mysh_unset(int argc, char **argv);
int mysh_cd(int argc, char **argv);

/* shell_utils2.c */
int mysh_fg(int argc, char **argv);
int mysh_jobs(void);
char *helper_strtrim(char *line);
void mysh_loop(void);

#endif
