/*
This code primarily comes from
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>

extern int cpu_idle_time();
extern void update_cpu_statistics();
extern char* initial_http_response(char* response);

int start_server(int PORT_NUMBER) {

  // structs to represent the server and client
  struct sockaddr_in server_addr, client_addr;

  int sock; // socket descriptor

  // 1. socket: creates a socket descriptor that you later use to make other system calls
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }
  int temp;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, & temp, sizeof(int)) == -1) {
    perror("Setsockopt");
    exit(1);
  }

  // configure the server
  server_addr.sin_port = htons(PORT_NUMBER); // specify port number
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero( & (server_addr.sin_zero), 8);

  // 2. bind: use the socket and associate it with the port number
  if (bind(sock, (struct sockaddr * ) & server_addr, sizeof(struct sockaddr)) == -1) {
    perror("Unable to bind");
    exit(1);
  }

  // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
  if (listen(sock, 1) == -1) {
    perror("Listen");
    exit(1);
  }

  // once you get here, the server is set up and about to start listening
  printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
  fflush(stdout);

//   // Branch out thread here
//   pthread_t thread_communication; // Initialize thread struct
//   pthread_create(&thread_communication, NULL, myThreadFun, NULL);

//   // blocks the calling thread until the called thread terminates
//   pthread_join(thread_id, NULL);


  int count = 0; // count the number of pages requested (for debugging purposes)

  while (1) { // keep looping and accept additional incoming connections

    // 4. accept: wait here until we get a connection on that port
    int sin_size = sizeof(struct sockaddr_in);
    int fd = accept(sock, (struct sockaddr * ) & client_addr, (socklen_t * ) & sin_size);
    if (fd != -1) {
      printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

      // buffer to read data into
      char request[1024];

      // 5. recv: read incoming message (request) into buffer
      int bytes_received = recv(fd, request, 1024, 0);
      // null-terminate the string
      request[bytes_received] = '\0';
      // print it to standard out
      printf("REQUEST:\n%s\n", request);

      count++; // increment counter for debugging purposes

      // this is the message that we'll send back
      int LEN_LIMIT = 10000;
      char * response = (char * ) malloc(LEN_LIMIT * sizeof(char));
      initial_http_response(response);

      printf("RESPONSE:\n%s\n", response);

      // 6. send: send the outgoing message (response) over the socket
      // note that the second argument is a char*, and the third is the number of chars
      send(fd, response, strlen(response), 0);

      free(response);

      // 7. close: close the connection
      close(fd);
      printf("Server closed connection\n");
    }
  }

  // 8. close: close the socket
  close(sock);
  printf("Server shutting down\n");

  return 0;
}



/* -------------------------------------------------------------------------  */

int idle_history [3600];
int count = 0;
double usage_max = 0;
double usage_avg = 0;
double usage_latest = 0 ;

int port_number = 3000; // hard-coded for use on Codio
#define MAX(a,b) (((a)>(b))?(a):(b))

int main(int argc, char * argv[]) {
  /*
  // check the number of arguments
  if (argc != 2) {
      printf("\nUsage: %s [port_number]\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }*/


  // Define entry functions
  void* entry_thread_cpu(void* p){

    while(1){
			sleep(1);
			idle_history[count] = cpu_idle_time();
			update_cpu_statistics();
			printf("\n---- CPU Statistics ----\n");
// 			printf("Idle history: ");
// 			for (int i = 0; i <= count; i++){
// 				printf(" %d ", idle_history[count]);
// 			}
// 			printf("\n");
			printf("max usage: %f \n", usage_max);
			printf("avg usage: %f \n", usage_avg);
			printf("lat usage: %f \n", usage_latest);
			count = (count + 1) % 3600;
		}
		return NULL;
  }

  void* entry_thread_http(void* p){
    start_server(port_number);
  }

  // Branch out thread here
  pthread_t thread_cpu; // Initialize thread struct
  pthread_t thread_http; // Initialize thread struct
  pthread_create(&thread_cpu, NULL, entry_thread_cpu, NULL);  // Branch 1: Monitor CPU usage
	pthread_create(&thread_http, NULL, entry_thread_http, NULL);  // Branch 2: Establish HTTP connection with browser

  // Terminates monitoring
	char user_input [100];
  printf("Type 'q' and enter to exit. \n");
  do {
		fgets(user_input, 99, stdin);
	}
  while (strcmp(user_input,"q\n") != 0);

	return 0;

}
