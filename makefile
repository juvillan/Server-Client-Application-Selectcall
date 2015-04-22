##this is a make file

##******************************************************
##client.h:
	##All the actions will be down in the client
##server.h:
	##Creates the FIFO and the POSIX Message Queue
##substring.h:
	##This is where i will just call the applications of the server and client to run it
##******************************************************

main: client3.c server3.c
	gcc -std=c99 -o client client3.c
	gcc -std=c99 -o server server3.c

clean:
	rm client
	rm server

tar:
	tar -cf Villani-Justin-PA3.tar client3.c server3.c movie.txt makefile