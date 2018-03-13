main: sha256.c sha512.c main.c
	gcc -Wall -o shautil sha512.c sha256.c main.c


clean:
		@echo "Cleaning..."
		rm shautil


debug: sha256.c sha256.h sha512.c sha512-h main.c 
	gcc -Wall -g -o shautil sha512.c sha256.c main.c		