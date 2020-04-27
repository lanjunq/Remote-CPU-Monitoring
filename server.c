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
extern char* initial_http_update(char* response);
int running = 1;
int sock; // socket descriptor
int fd;

int idle_history [3600];
int count = 0;
double usage_max = 0;
double usage_avg = 0;
double usage_latest = 0 ;

int port_number = 3000; // hard-coded for use on Codio
#define MAX(a,b) (((a)>(b))?(a):(b))

pthread_mutex_t lock; // for CPU statistics
pthread_mutex_t lock_2; // for global variable running, sock, and fd
pthread_mutex_t lock_3; // for printf()


/* Helper function to start_server and establish socket */
int start_server(int PORT_NUMBER) {

  // structs to represent the server and client
  struct sockaddr_in server_addr, client_addr;

  // 1. socket: creates a socket descriptor that you later use to make other system calls
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }
// 	int temp;
  int temp = 1;
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
	pthread_mutex_lock( &lock_3 );
//   printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
	pthread_mutex_unlock( &lock_3 );
  fflush(stdout);

  // int count = 0; // count the number of pages requested (for debugging purposes)
	pthread_mutex_lock( &lock_2 );
  while (running == 1) { // keep looping and accept additional incoming connections
		pthread_mutex_unlock( &lock_2 );

    // 4. accept: wait here until we get a connection on that port
    int sin_size = sizeof(struct sockaddr_in);
// 		alarm(2);
		pthread_mutex_lock( &lock_2 );
		int fd_copy = fd;
		int sock_copy = sock;
		pthread_mutex_unlock( &lock_2 );

    fd_copy = accept(sock_copy, (struct sockaddr * ) & client_addr, (socklen_t * ) & sin_size);

		pthread_mutex_lock( &lock_2 );
		fd = fd_copy;
		sock = sock_copy;
// 		pthread_mutex_unlock( &lock_2 );

		if (fd != -1) {
			pthread_mutex_unlock( &lock_2 );
			pthread_mutex_lock( &lock_3 );
// 			printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			pthread_mutex_unlock( &lock_3 );

			// 5. recv: read and echo HTTP request
			char request[1024]; // buffer to read data into
			pthread_mutex_lock( &lock_2 );
			int bytes_received = recv(fd, request, 1024, 0);
			pthread_mutex_unlock( &lock_2 );
			request[bytes_received] = '\0';
			pthread_mutex_lock( &lock_3 );
// 			printf("REQUEST:\n%s\n", request);
			pthread_mutex_unlock( &lock_3 );

			// HTTP response
			char* get = strtok(request, " ");
			get = strtok(NULL, " ");

			int LEN_LIMIT = 10000;
			char * response = (char * ) malloc(LEN_LIMIT * sizeof(char));
			if (strcmp(get, "/data") == 0){
				initial_http_update(response);
			} else {
				initial_http_response(response);
			}
			pthread_mutex_lock( &lock_3 );
// 			printf("RESPONSE:\n%s\n", response); // echo HTTP response
			pthread_mutex_unlock( &lock_3 );

			// 6. send: send the outgoing message (response) over the socket
			// note that the second argument is a char*, and the third is the number of chars
			pthread_mutex_lock( &lock_2 );
			send(fd, response, strlen(response), 0);
			pthread_mutex_unlock( &lock_2 );

			// 7. close: close the connection
			free(response);
			pthread_mutex_lock( &lock_2 );
			close(fd);
			pthread_mutex_unlock( &lock_2 );
			pthread_mutex_lock( &lock_3 );
// 			printf("Server closed connection\n");
			pthread_mutex_unlock( &lock_3 );
		} else {
			pthread_mutex_unlock( &lock_2 );
		}
		pthread_mutex_lock( &lock_2 );
  }
	pthread_mutex_unlock( &lock_2 );

  // 8. close: close the socket
  pthread_mutex_lock( &lock_2 );
  close(sock);
	pthread_mutex_unlock( &lock_2 );
	pthread_mutex_lock( &lock_3 );
  printf("Server shutting down\n");
	pthread_mutex_unlock( &lock_3 );
  return 0;
}

/* Helper function to print CPU statistics to stdout */
void print_cpu_statistics(){
	pthread_mutex_lock( &lock_3 );
  printf("\n---- CPU Statistics ----\n");
  printf("max usage: %f \n", usage_max);
  printf("avg usage: %f \n", usage_avg);
  printf("lat usage: %f \n", usage_latest);
	pthread_mutex_unlock( &lock_3 );
}

/* Define entry function to CPU thread */
void* entry_thread_cpu(void* p){
	pthread_mutex_lock( &lock_2 );
  while(running == 1){
		pthread_mutex_unlock( &lock_2 );
			pthread_mutex_lock( &lock );
    idle_history[count] = cpu_idle_time();  // get latest idle_time
			pthread_mutex_unlock( &lock );
    update_cpu_statistics();  // update global variables
//     print_cpu_statistics();   // show to stdout
    	pthread_mutex_lock( &lock );
    count = (count + 1) % 3600;
			pthread_mutex_unlock( &lock );
		sleep(1);
		pthread_mutex_lock( &lock_2 );
  }
	pthread_mutex_unlock( &lock_2 );
  return NULL;
}

/* Define entry function to HTTP thread */
void* entry_thread_http(void* p){
  start_server(port_number);
}


/* main function */
int main(int argc, char * argv[]) {

  // Create threads
  pthread_t thread_cpu; // Initialize thread struct
  pthread_t thread_http; // Initialize thread struct

	int ret = pthread_mutex_init( &lock, NULL );
	int ret_2 = pthread_mutex_init( &lock_2, NULL );
	int ret_3 = pthread_mutex_init( &lock_3, NULL );
	if ( ret == -1 || ret_2 == -1 ) {
		printf("ERROR: Mutex");
		exit(-1);
	}

// int pthread_mutex_lock(pthread_mutex_t *mutex)

// int pthread_mutex_unlock(pthread_mutex_t *mutex)
// int pthread_mutex_destroy(pthread_mutex_t *mutex)

  pthread_create(&thread_cpu, NULL, entry_thread_cpu, NULL);  // Branch 1: Monitor CPU usage
	pthread_create(&thread_http, NULL, entry_thread_http, NULL);  // Branch 2: Establish HTTP connection with browser

  // Main thread monitors user input
	pthread_mutex_lock( &lock_3 );
  printf("Type 'q' and enter to exit. \n");
	pthread_mutex_unlock( &lock_3 );
	char user_input [100];
  do {
		fgets(user_input, 99, stdin);
	}
  while (strcmp(user_input,"q\n") != 0);

	pthread_mutex_lock( &lock_2 );
	running = 0;
	shutdown(sock, SHUT_RDWR);
  close(fd);
	close(sock);
	pthread_mutex_unlock( &lock_2 );

	pthread_join(thread_cpu, NULL);
	pthread_join(thread_http, NULL);
	pthread_mutex_lock( &lock_3 );
  printf("Server shutting down\n");
	pthread_mutex_unlock( &lock_3 );
	return 0;
}
