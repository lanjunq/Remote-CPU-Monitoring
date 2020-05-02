compile: server.c http_response.c cpu_monitoring.c
	cc -o server server.c http_response.c cpu_monitoring.c -pthread

run: compile
	./server

valgrind: compile
	valgrind ./server

helgrind: compile
	valgrind --tool=helgrind ./server

clean:
	rm test_interface server http_response -f


# Install valgrind package
install_helgrind:
	sudo apt-get install valgrind