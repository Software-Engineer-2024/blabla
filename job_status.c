#include "general.h"
#include "status_strings.h"

/**
 * insert_job - add a job to the shell jobs to be executed
 * @job: struct of type job
 * Return: job id if successfully added , -1 otherwise
 */

int insert_job(struct job *job)
{
	struct shell_info *shell = get_shell_info();
	int id = get_next_job_id();

	if (id < 0)
		return (-1);

	job->id = id;
	shell->jobs[id] = job;
	return (id);
}

/**
 * remove_job - releases job from exectution
 * @id: job id
 * Return: 0 if successful , -1 otherwise
 */

int remove_job(int id)
{
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS || shell->jobs[id] == NULL)
		return (-1);

	release_job(id);
	shell->jobs[id] = NULL;

	return (0);
}

/**
 * is_job_completed - checks whether a job is completed or not
 * @id: job id
 * Return: 1 if completed , 0 otherwise
 */

int is_job_completed(int id)
{
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS || shell->jobs[id] == NULL)
		return (0);

	for (proc = shell->jobs[id]->root; proc != NULL; proc = proc->next)
	{
		if (proc->status != STATUS_DONE)
			return (0);
	}

	return (1);
}

/**
 * set_job_status - add status to job
 * @id: job id
 * @status: status to be added
 * Return: 0 if job exists , -1 otherwise
 */

int set_job_status(int id, int status)
{
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS || shell->jobs[id] == NULL)
		return (-1);

	for (proc = shell->jobs[id]->root; proc != NULL; proc = proc->next)
	{
		if (proc->status != STATUS_DONE)
			proc->status = status;
	}

	return (0);
}

/**
 * print_job_status - prints job status
 * @id: job id
 * Return: 0 if exists , -1 if not
 */

int print_job_status(int id)
{
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS || shell->jobs[id] == NULL)
		return (-1);

	printf("[%d]", id);
	for (proc = shell->jobs[id]->root; proc != NULL; proc = proc->next)
	{
		printf("\t%d\t%s\t%s", proc->pid,
				STATUS_STRING[proc->status], proc->command);
		if (proc->next != NULL)
			printf("|\n");
		else
			printf("\n");
	}

	return (0);
}
