all:consume publish
consume: consume.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev
publish: publish.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev
.PHONY:clean
clean:
	rm consume publish