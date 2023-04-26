#include "general.h"
/**
 * child_pid_eq_zero - helper for the mysh_launch_process function
 * Return: void
 */
void child_pid_eq_zero(void)
{
	signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);

		proc->pid = getpid();
		if (job->pgid > 0)
		{
			setpgid(0, job->pgid);
		}
		else
		{
			job->pgid = proc->pid;
			setpgid(0, job->pgid);
		}

		if (in_fd != 0)
		{
			dup2(in_fd, 0);
			close(in_fd);
		}

		if (out_fd != 1)
		{
			dup2(out_fd, 1);
			close(out_fd);
		}

		if (execvp(proc->argv[0], proc->argv) < 0)
		{
			printf("./hsh: %d %s: not found \n", job->command_count, proc->argv[0]);
			exit(0);
		}

		exit(0);
}
/**
 * mysh_launch_process - launches process
 * @job: job
 * @proc: process
 * @in_fd: input file directory
 * @out_fd: output file directory
 * @mode: mode
 * Return: status if successful , -1 if failed
 */
int mysh_launch_process(struct job *job, struct process *proc,
		int in_fd, int out_fd, int mode)
{
	pid_t childpid;
	int status = 0;

	proc->status = STATUS_RUNNING;
	if (proc->type != COMMAND_EXTERNAL && mysh_execute_builtin_command(proc))
	{
		return (0);
	}
	childpid = fork();

	if (childpid < 0)
		return (-1);
	else if (childpid == 0)
		child_pid_eq_zero();
	else
	{
		proc->pid = childpid;
		if (job->pgid > 0)
			setpgid(childpid, job->pgid);
		else
		{
			job->pgid = proc->pid;
			setpgid(childpid, job->pgid);
		}

		if (mode == FOREGROUND_EXECUTION)
		{
			tcsetpgrp(0, job->pgid);
			status = wait_for_job(job->id);
			signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(0, getpid());
			signal(SIGTTOU, SIG_DFL);
		}
	}

	return (status);
}
