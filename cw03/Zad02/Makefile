BIN = search_files dir* file*
FLAGS = -Wall
CC = gcc

all:
	$(CC) $(FLAGS) search_files.c -o search_files

test:
	@mkdir dir1 dir2 dir3
	@touch dir1/file1.txt dir2/file2.txt dir3/file3.txt
	@echo "nUISHfidsbsdBUYsfusdbckjsdbf" > dir1/file1.txt
	@echo "ndbbdfbyuguyeuebv" > dir2/file2.txt
	@echo "nUISHfidsbsdB jkvUVDUAVYcyCXBCHJVAFYEDlncakbdcvjaUYsfusdbckjsdbf" > dir3/file3.txt
	@echo "lnbiddfnvkdfn" > file4.txt
	@echo "uiUSDVgvygvsdcvgsdbcgsjdvv sdvjsb dfusgjbv jds" > file5.txt
	./search_files

clean:
	rm -rf $(BIN)