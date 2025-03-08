all:discovery registry
discovery : discovery.cc main.pb.cc
	g++ -g -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb
registry : registry.cc main.pb.cc
	g++ -g -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb