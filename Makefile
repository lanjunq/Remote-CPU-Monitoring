test_interface: server_send_html.c
	cc -o test_interface server_send_html.c
	./test_interface

clean:
	rm test_interface server