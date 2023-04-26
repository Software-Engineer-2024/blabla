#include "general.h"
/**
 * launch_job - helper for the my_sh_launch function
 * @job: job
 * @proc: process
 * @in_fd: input file directory
 * @out_fd: output file directory
 * Return: status if successful , -1 if failed
 */
int launch_job(struct job *job, struct process *proc,
		int in_fd, int out_fd)
{
	int status = 0;

	for (proc = job->root; proc != NULL; proc = proc->next)
	{
		if (proc == job->root && proc->input_path != NULL)
		{
			in_fd = open(proc->input_path, O_RDONLY);
			if (in_fd < 0)
			{
				printf("./hsh:%d : no such file or directory: %s \n",
						count, proc->input_path);
				remove_job(job_id);
				return (-1);
			}
		}
		if (proc->next != NULL)
		{
			pipe(fd);
			status = mysh_launch_process(job, proc, in_fd, fd[1], PIPELINE_EXECUTION);
			close(fd[1]);
			in_fd = fd[0];
		} else
		{
			int out_fd = 1;

			if (proc->output_path != NULL)
			{
				out_fd = open(proc->output_path, O_CREAT | O_WRONLY,
						S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				if (out_fd < 0)
					out_fd = 1;
			} status = mysh_launch_process(job, proc, in_fd, out_fd, job->mode);
		}
	}
	return (status);
}
/**
 * mysh_launch_job - launches job
 * @job: job
 * Return: status
 */
int mysh_launch_job(struct job *job)
{
	struct process *proc;
	int status = 0, in_fd = 0, fd[2], job_id = -1;
	int count = 0;

	check_zombie();
	if (job->root->type == COMMAND_EXTERNAL)
		job_id = insert_job(job);
	count = job->command_count;
	status = launch_job(job, proc, in_fd, out_fd);

	if (job->root->type == COMMAND_EXTERNAL)
	{
		if (status >= 0 && job->mode == FOREGROUND_EXECUTION)
			remove_job(job_id);
		else if (job->mode == BACKGROUND_EXECUTION)
			print_processes_of_job(job_id);
	}
	return (status);
}

/**
 * release_job -  removes a job from list of those known by shell
 * @id: job id
 * Return: 0 if successful , -1 otherwise
 */
int release_job(int id)
{
	struct shell_info *shell = get_shell_info();
	struct job *job = shell->jobs[id];
	struct process *proc, *tmp;

	if (id > NR_JOBS || shell->jobs[id] == NULL)
		return (-1);
	for (proc = job->root; proc != NULL;)
	{
		tmp = proc->next;
		free(proc->command);
		free(proc->argv);
		free(proc->input_path);
		free(proc->output_path);
		free(proc);
		proc = tmp;
	}

	free(job->command);
	free(job);

	return (0);
}
