#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int cpu_idle_time();

int main(){

	cpu_idle_time();
	
	return 0;
}

/* A helper function that returns CPU idle time being whenever called */
int cpu_idle_time(){
	
	// Declare variables
	int idle_time;
	char buffer [100];
	
	int target = 4; 
	char* start;
	
	// Read a line file
	FILE* fp = fopen("/proc/stat", "r");
	fread(buffer, sizeof(char), 100, fp);
	printf("line: %s \n", buffer);
	
	// Extract idle time
	start = strtok(buffer, " ");
	for (int i = 0; i < target; i++){
		start = strtok(NULL, " ");
	}
	
	// Convert idle time to int
	idle_time = atoi(start);
	
	fclose(fp);
	
	return idle_time;
}
