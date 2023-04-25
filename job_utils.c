#include "general.h"

/**
 * get_job_by_id-get the corresponding job struct in current shell session.
 * @id: int the ID of the job to retrieve.
 * Return:pointer to job struct corresponding to given ID or NULL if ID invalid
 */
struct job *get_job_by_id(int id)
{
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS)
	{
		return (NULL);
	}

	return (shell->jobs[id]);
}

/**
 * get_job_id_by_pid-searches for corresponding job ID in current shell session
 * @pid:the ID of the process to search for
 * Return:job id that contains the process ,-1 if process not found in any job
 */
int get_job_id_by_pid(int pid)
{
	int i;
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	for (i = 1; i <= NR_JOBS; i++)
	{
		if (shell->jobs[i] != NULL)
		{
			for (proc = shell->jobs[i]->root; proc != NULL; proc = proc->next)
			{
				if (proc->pid == pid)
				{
					return (i);
				}
			}
		}
	}

	return (-1);
}

/**
 * get_pgid_by_job_id-search for process group ID of the job by job id.
 * @id: int job id to used in search.
 * Return:The pgid of job, or -1 if job is not found.
 */
int get_pgid_by_job_id(int id)
{
	struct job *job = get_job_by_id(id);

	if (job == NULL)
	{
		return (-1);
	}

	return (job->pgid);
}

/**
 * get_next_job_id - search for the id of the next job.
 * Return: the next available job ID,-1 if no next job not found
 */
int get_next_job_id(void)
{
	int i;
	struct shell_info *shell = get_shell_info();

	for (i = 1; i <= NR_JOBS; i++)
	{
		if (shell->jobs[i] == NULL)
		{
			return (i);
		}
	}
	return (-1);
}

/**
 * wait_for_job - Wait for all processes in a job to finish or be suspended.
 * @id: The ID of the job to wait for.
 * Return:status of last process waited on.If error occurs,returns -1.
 */
int wait_for_job(int id)
{
	struct shell_info *shell = get_shell_info();
	int proc_count = get_proc_count(id, PROC_FILTER_REMAINING);
	int wait_pid = -1, wait_count = 0;
	int status = 0;

	if (id > NR_JOBS || shell->jobs[id] == NULL)
	{
		return (-1);
	}

	do {
		wait_pid = waitpid(-shell->jobs[id]->pgid, &status, WUNTRACED);
		wait_count++;

		if (WIFEXITED(status))
		{
			set_process_status(wait_pid, STATUS_DONE);
		}
		else if (WIFSIGNALED(status))
		{
			set_process_status(wait_pid, STATUS_TERMINATED);
		}
		else if (WSTOPSIG(status))
		{
			status = -1;
			set_process_status(wait_pid, STATUS_SUSPENDED);
			if (wait_count == proc_count)
			{
				print_job_status(id);
			}
		}
	} while (wait_count < proc_count);

	return (status);
}
