/* Define the helper functions to generate http_response in this file */

/*
Parts of a HTTP response:
  * header
  * html body
  * JavaScript code

Functions to be defined:
  1.
 */

#include <stdlib.h>
#include <stdio.h>

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
