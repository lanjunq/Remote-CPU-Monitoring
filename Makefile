main: server.c http_response.c
	cc -o server server.c http_response.c
	./server

test_interface: server_send_html.c
	cc -o test_interface server_send_html.c
	./test_interface

test_architecture: server_architecture.c cpu_idle_time.c
	cc -o test_architecture server_architecture.c cpu_idle_time.c -pthread
	./test_architecture

test_http_response: http_response.c
	cc -o http_response http_response.c
	./http_response

clean:
	rm test_interface server
