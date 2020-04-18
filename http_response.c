/* Define the helper functions to generate http_response in this file */

/*
Parts of a HTTP response:
  * header
  * html body
  * JavaScript code

Functions to be defined:
  1.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
extern int idle_history [3600];
extern int count;
extern double usage_max;
extern double usage_avg;
extern double usage_latest;

char* initial_http_update(char* response){
  char response_header [] = "HTTP/1.1 200 OK\nContent-Type: application/JSON\n\n";
  strcpy(response, response_header);
  // example JSON: {\"name\" : \"snoopy\", \"id\" : 12345}"

  char response_json [100];

	strcat(response, "{");
	sprintf(response_json, "\"%s\" : %d", "count", count);
  strcat(response, response_json);
	strcat(response, " , ");

  sprintf(response_json, "\"%s\" : %f", "usage_max", usage_max);
  strcat(response, response_json);
	strcat(response, " , ");

  sprintf(response_json, "\"%s\" : %f", "usage_avg", usage_avg);
  strcat(response, response_json);
	strcat(response, " , ");

	sprintf(response_json, "\"%s\" : %f", "usage_latest", usage_latest);
  strcat(response_json, "}");
  strcat(response, response_json);

	return response;
}

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

  fread(content_js, sizeof(char), LIMIT-1, fp);
	int len = sprintf(response_javascript, "<html>\n<body>\n<script>\n%s\n</script>\n</body>\n</html>", content_js);
  response_javascript[len] = '\0';
  fclose(fp);
	free(content_js);

  // Combine three components together
  sprintf(response, "%s%s%s", response_header, response_html, response_javascript);
// 	sprintf(response, "%s%s", response_header, response_html);
  free(response_html);
	free(response_javascript);

  // Sanity check
//   printf("response \n%s \n", response);

  return response;
}

/*
int main (){
  char* response = (char *) malloc(10000 * sizeof(char));
  initial_http_response(response);
  free(response);
}
*/
