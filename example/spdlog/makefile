all : sync async main
main : main.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags
sync : sync.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt
async : async.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt