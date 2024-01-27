compiler = g++
flags = -g -Wall -std=c++11
compile = $(compiler) $(flags)

scheduling: scheduling.cpp avlbst.h
	$(compile) $< -o $@

.PHONY: clean
clean:
	rm -rf *.o scheduling
	