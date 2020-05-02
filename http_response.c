/* Define the helper functions to generate http_response in this file */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

extern int idle_history [3600];
extern int count;
extern double usage_max;
extern double usage_avg;
extern double usage_latest;

extern pthread_mutex_t lock;
extern pthread_mutex_t lock_2;
extern pthread_mutex_t lock_3;


/* Helper function to generate JSON update HTTP response */
char* initial_http_update(char* response){
  char response_header [] = "HTTP/1.1 200 OK\nContent-Type: application/JSON\n\n";
  strcpy(response, response_header);

  char response_json [100];

	strcat(response, "{");
	pthread_mutex_lock( &lock );
	sprintf(response_json, "\"%s\" : %d", "count", count);
	pthread_mutex_unlock( &lock );
  strcat(response, response_json);
	strcat(response, " , ");

	pthread_mutex_lock( &lock );
		
  sprintf(response_json, "\"%s\" : %f", "usage_max", usage_max);
  strcat(response, response_json);
	strcat(response, " , ");

  sprintf(response_json, "\"%s\" : %f", "usage_avg", usage_avg);
  strcat(response, response_json);
	strcat(response, " , ");

	sprintf(response_json, "\"%s\" : %f", "usage_latest", usage_latest);
  strcat(response_json, "}");
  strcat(response, response_json);
	
	pthread_mutex_unlock( &lock );

	return response;
}


/* Helper function to generate initial full HTTP response */
char* initial_http_response(char* response){

  // Generate header
  char response_header [] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

  // Include interface.html
  int LIMIT = 10000;
  char* response_html = (char *) malloc(LIMIT * sizeof(char));
  FILE* fp = fopen("interface.html", "r");
  int html_length = fread(response_html, sizeof(char), LIMIT-1, fp);
  response_html[html_length] = '\0';
  fclose(fp);

	// Include webpage.js
  char* response_javascript = (char *) malloc(LIMIT * sizeof(char));
	char* content_js = (char *) malloc(LIMIT * sizeof(char));
  fp = fopen("webpage.js", "r");
  int len = fread(content_js, sizeof(char), LIMIT-1, fp);
	content_js[len] = '\0';
	// int len = sprintf(response_javascript, "<html>\n<body>\n<script>\n%s\n</script>\n</body>\n</html>", content_js);
  len = sprintf(response_javascript, "<script>\n%s\n</script>\n</body>\n</html>", content_js);
  response_javascript[len] = '\0';
  fclose(fp);

  // Combine three components together
  len = sprintf(response, "%s%s%s", response_header, response_html, response_javascript);
  response[len] = '\0';
	// sprintf(response, "%s%s", response_header, response_html);
  free(response_html);
	free(response_javascript);
  free(content_js);

  return response;
}
