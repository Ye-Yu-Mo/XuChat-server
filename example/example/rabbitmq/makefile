all : publish consume
publish : publish.cc
	g++ -g -std=c++17 $^ -o $@ -lamqpcpp -lev -lfmt -lspdlog -lgflags
consume : consume.cc
	g++ -g -std=c++17 $^ -o $@ -lamqpcpp -lev -lfmt -lspdlog -lgflags