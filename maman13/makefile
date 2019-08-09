# specify filenames in the following lists

EXEC_FILE = my_format
C_FILES = my_format.c 
H_FILES = fat12.h 

# ==> to flush stdout after each line, insert in main() the statement: FLUSH_STDOUT; 
# ==> to flush stderr after each line, insert in main() the statement: FLUSH_STDERR;

#--------------------------------------------
# no changes need to be made beyond this line

# definitions for flushing after earch line
D_OFLUSH = -DFLUSH_STDOUT='setvbuf(stdout,NULL,_IOLBF,BUFSIZ);'
D_EFLUSH = -DFLUSH_STDERR='setvbuf(stderr,NULL,_IOLBF,BUFSIZ);'
D_FLUSH = $(D_OFLUSH) $(D_EFLUSH)

O_FILES = $(C_FILES:.c=.o)

all: $(EXEC_FILE)

$(EXEC_FILE): $(O_FILES) 
	gcc -g -Wall  $(O_FILES) -o $(EXEC_FILE) -lm

%.o: %.c $(H_FILES)
	gcc -g -Wall  -c $(D_FLUSH) -o $@ $<
	
clean:
	rm -f *.o $(EXEC_FILE)
	
