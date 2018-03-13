#include "sha256.h"
#include "sha512.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


bool exec_sha256 = false;
bool exec_sha512 = false;




int main(int argc, char const *argv[]) {

	if (argc == 1) {
        fprintf(stderr, "Usage: %s <options...><input files...>\n", argv[0]);
        return 1;
    }
	
	int c;

	while(1) {

		static struct option long_options[] = {
			{"sha256", no_argument, 0, 'd'},
			{"sha512", no_argument, 0, 'c'}	
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "", long_options, &option_index);

		/* detect end of the options */

		if (c == -1) {
			break;
		}

		switch (c) {
			case 0:
				break;

			case '?':
				break;

			case 'd':
				exec_sha256 = true;
				break;

			case 'c':
				exec_sha512 = true;
				break;	

			default:
				abort();

		}

	}

	for (int i = optind; i < argc; ++i) {
		if (exec_sha512)
			sha512(argv[i]);
	}



	for (int i = optind; i < argc; ++i) {
		if (exec_sha256)
			sha256(argv[i]);
	}



	
	return 0;
}