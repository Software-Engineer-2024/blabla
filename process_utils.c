#include "general.h"


int get_proc_count(int id, int filter)
{
	struct process *proc;
	struct shell_info *shell = get_shell_info();
	int count = 0;

	if (id > NR_JOBS || shell->jobs[id] == NULL)
	{
		return (-1);
	}

	for (proc = shell->jobs[id]->root; proc != NULL; proc = proc->next)
	{
		if (filter == PROC_FILTER_ALL ||
				(filter == PROC_FILTER_DONE && proc->status == STATUS_DONE) ||
				(filter == PROC_FILTER_REMAINING && proc->status != STATUS_DONE))
		{
			count++;
		}
	}

	return (count);
}


int print_processes_of_job(int id)
{
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	if (id > NR_JOBS || shell->jobs[id] == NULL)
	{
		return (-1);
	}

	printf("[%d]", id);

	for (proc = shell->jobs[id]->root; proc != NULL; proc = proc->next)
	{
		printf(" %d", proc->pid);
	}
	printf("\n");

	return (0);
}


int set_process_status(int pid, int status)
{
	int i;
	struct process *proc;
	struct shell_info *shell = get_shell_info();

	for (i = 1; i <= NR_JOBS; i++)
	{
		if (shell->jobs[i] == NULL)
		{
			continue;
		}
		for (proc = shell->jobs[i]->root; proc != NULL; proc = proc->next)
		{
			if (proc->pid == pid)
			{
				proc->status = status;
				return (0);
			}
		}
	}

	return (-1);
}


int wait_for_pid(int pid)
{
	int status = 0;

	waitpid(pid, &status, WUNTRACED);
	if (WIFEXITED(status))
	{
		set_process_status(pid, STATUS_DONE);
	}
	else if (WIFSIGNALED(status))
	{
		set_process_status(pid, STATUS_TERMINATED);
	}
	else if (WSTOPSIG(status))
	{
		status = -1;
		set_process_status(pid, STATUS_SUSPENDED);
	}

	return (status);
}


void check_zombie()
{
	int status, pid = 0;
	int job_id = get_job_id_by_pid(pid);

	while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0)
	{
		if (WIFEXITED(status))
		{
			set_process_status(pid, STATUS_DONE);
		}
		else if (WIFSTOPPED(status))
		{
			set_process_status(pid, STATUS_SUSPENDED);
		}
		else if (WIFCONTINUED(status))
		{
			set_process_status(pid, STATUS_CONTINUED);
		}

		if (job_id > 0 && is_job_completed(job_id))
		{
			print_job_status(job_id);
			remove_job(job_id);
		}
	}
}
