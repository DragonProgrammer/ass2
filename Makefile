CC = gcc

# .bin (binary) means it is an executable

all: Assign2.bin    

# to make anything (%) .bin requires the same anything .o (object)
# take CC ... (more later)

%.bin: %.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c $< -o $@
	
clean:
	rm -rf *.bin
