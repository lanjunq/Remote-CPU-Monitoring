#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern double usage_max, usage_avg, usage_latest;
extern int idle_history [];
extern int count;

/* A helper function that returns CPU idle time being whenever called */
int cpu_idle_time(){

	int idle_time;
	char buffer [100];
	int target = 4;
	char* start;

	// Read a line file
	FILE* fp = fopen("/proc/stat", "r");
	fread(buffer, sizeof(char), 100, fp);

	// Extract idle time
	start = strtok(buffer, " ");
	for (int i = 0; i < target; i++){
		start = strtok(NULL, " ");
	}
	idle_time = atoi(start);

	fclose(fp);
	return idle_time;
}


/* A helper function to update usage_max, usage_avg, and usage_latest */
void update_cpu_statistics(){

	// Read and analyze
	int latest = idle_history[count]; // Note: not yet updated count when executing this function, i.e. idle_history[count] is the lastest
	int previous = idle_history[count - 1];

	// Compute new CPU status
	double idle_time = (latest - previous) / 4; // idle time in %
	double usage = 100 - idle_time; // CPU work time in %
	if (usage < 0) usage = 0;
	if (usage > 100) usage = 100;

	// Update CPU status
	usage_latest = usage;
	usage_avg = (usage_avg * count + usage) / (count + 1);
	if (usage > usage_max) usage_max = usage;

	return;
}
