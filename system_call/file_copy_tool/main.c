#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#define BUFFER_SIZE 65536

int main(int argc, char *argv[]) {
	// later
	// char file_you_want_to_copy[101];
	int file_source = open("./bigfile.txt", O_RDONLY);
	int file = open("./text.txt", O_WRONLY | O_CREAT, 0644);

	struct stat file_info;

	if (file == -1 || file_source == -1) {
		perror("open");
		if (file != -1) close(file);
		if (file_source != -1) close(file_source);
		return 1;
	}

	// copy file info from file_source
	if (fstat(file_source, &file_info) == -1) {
		perror("error executing fstat");
		close(file_source);
		close(file);
		return 1;
	}

	// change mod destination file
	if (fchmod(file, file_info.st_mode) == -1) {
		perror("error to change mod");
		close(file_source);
		close(file);
		return 1;
	}

	char buffer[BUFFER_SIZE];
	while (1) {
		ssize_t bytes_read = read(file_source, buffer, BUFFER_SIZE);
		if (bytes_read == -1) {
			perror("read");
			close(file_source);
			close(file);
			return 1;
		}

		if (bytes_read == 0) {
			break;	// EOF
		}

		// printf("%.*s", (int)bytes_read, buffer);

		ssize_t bytes_write = write(file, buffer, bytes_read);
		if (bytes_write == -1) {
			perror("write");
			close(file_source);
			close(file);
			return 1;
		}
	}
	
	close(file);
	close(file_source);

	return 0;
}
