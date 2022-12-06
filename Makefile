CC := g++ -Wno-pointer-arith -Wwrite-strings
SRC := main.cpp mylang.cpp Onegin-sort\text-sort.cpp Differentiator\diftree.cpp Differentiator\recursivedescent.cpp
DIR := C:\Users\USER\Documents\My-language

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $(DIR)\$@

rec: $(REC)
	$(CC) $^ -o $(DIR)\$@

clean:
	rm -rf *.png *.dot
