test_interface: server_send_html.c
	cc -o test_interface server_send_html.c
	./test_interface

test_architecture: server_architecture.c cpu_idle_time.c
	cc -o test_architecture server_architecture.c cpu_idle_time.c -pthread
	./test_architecture

clean:
	rm test_interface server
