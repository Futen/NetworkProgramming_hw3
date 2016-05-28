all: server client
server:	Header.h Header.cpp DataBaseHeader.h UserClass.cpp ArticalClass.cpp UserFriend.cpp
	g++ UserClass.cpp Header.cpp ArticalClass.cpp Server.cpp UserFriend.cpp -o server
client: Header.h Header.cpp ClientHeader.h Client.cpp ClientFunction.cpp
	g++ Client.cpp Header.cpp ClientFunction.cpp  -o client
clean:
	rm server client
