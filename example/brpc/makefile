all : server client
server : server.cc main.pb.cc
	g++ -std=c++17 $^ -o $@ -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb
client : client.cc main.pb.cc
	g++ -std=c++17 $^ -o $@ -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb