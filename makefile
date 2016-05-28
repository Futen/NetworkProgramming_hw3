all: server client
server:	Header.h Header.cpp DataBaseHeader.h UserClass.cpp ArticalClass.cpp UserFriend.cpp
	g++ UserClass.cpp Header.cpp ArticalClass.cpp Server.cpp UserFriend.cpp -o server -pthread
client: Header.h Header.cpp  Client.cpp
	g++ Client.cpp Header.cpp  -o client -pthread
clean:
	rm server client
