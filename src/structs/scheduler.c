//File scheduler.c

#include <pthread.h>

JobScheduler *initialize_scheduler(int execution_threads);
void submit_job(JobScheduler *sch, Job *j);
void execute_all_jobs(JobScheduler *sch);
void wait_all_tasks_finish(JobScheduler *sch);  //waits all submitted tasks to finish
OK_SUCCESS destroy_scheduler(JobScheduler *sch);
