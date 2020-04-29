
yourProgram: yourProgram.o
	gcc yourProgram.o -o yourProgram
yourProgram.o: yourProgram.c Makefile
	gcc yourProgram.c -clean
clean:
	rm yourProgram.o yourProgram
run:
	sudo ./yourProgram
