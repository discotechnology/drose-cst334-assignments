//
// Created by Sam Ogden on 8/8/23.
//

#include "student_code.h"
#include "stdio.h"

void mark_start(PROCESS* p, SCHEDULER_STATS* stats, float time_started) {
	stats->num_processes_started = stats->num_processes_started + 1;
	stats->sum_of_response_time = stats->sum_of_response_time + (time_started - p->entry_time);
}
void mark_end(PROCESS* p, SCHEDULER_STATS* stats, float time_completed) {
	stats->num_processes_completed = stats->num_processes_completed + 1;
	stats->sum_of_turnaround_times = stats->sum_of_turnaround_times + (time_completed - p->entry_time);
}

void finalize_stats(SCHEDULER_STATS* stats) {
	stats->average_turnaround_time = (stats->sum_of_turnaround_times / (float)stats->num_processes_completed);
	stats->average_response_time = (stats->sum_of_response_time / (float)stats->num_processes_completed);
}

int compare_noop(PROCESS p1, PROCESS p2) {
  return 0;
}

int compare_priority(PROCESS p1, PROCESS p2) {
	if (p1.priority < p2.priority) {
		return 0;
	}
	else {
		return 1;
	}
}

int compare_arrival_time(PROCESS p1, PROCESS p2) {
	if (p1.entry_time <= p2.entry_time) {
		return 0;
	}
	else {
		return 1;
	}
}

int compare_arrival_time_invert(PROCESS p1, PROCESS p2) {
	if (p1.entry_time < p2.entry_time) {
		return 1;
	}
	else {
		return 0;
	}
}

int compare_job_duration(PROCESS p1, PROCESS p2) {
	if (p1.duration < p2.duration) {
		return 0;
	}
	else {
		return 1;
	}
}

int compare_time_remaining(PROCESS p1, PROCESS p2) {
	if (p1.time_remaining < p2.time_remaining) {
		return 0;
	}
	else {
		return 1;
	}
}

int compare_last_run(PROCESS p1, PROCESS p2) {
	if (p1.last_run <= p2.last_run) {
		return 0;
	}
	else {
		return 1;
	}
}

PROCESS* fifo_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_arrival_time);
  return p;
}

PROCESS* rr_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_last_run);
  return p;
}

PROCESS* sjf_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_job_duration);
  return p;
}

PROCESS* priority_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_priority);
  return p;
}

PROCESS* lifo_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_arrival_time_invert);
  return p;
}

PROCESS* stcf_process_selector(PROCESS_LIST* pl) {
  PROCESS *p;
  p = get_minimum(pl, compare_time_remaining);
  return p;
}
