all:	cli	ser	../Librairie/socket.o

../Librairie/socket.o:	../Librairie/socket.h	../Librairie/socket.cpp
	g++ -c ../Librairie/socket.cpp -o ../Librairie/socket.o

cli:	clientTest.cpp	../Librairie/socket.o	../Librairie/socket.h
	g++ -o cli clientTest.cpp ../Librairie/socket.o ../Librairie/socket.h

ser:	serveurTest.cpp	../Librairie/socket.o	../Librairie/socket.h
	g++ -o ser serveurTest.cpp ../Librairie/socket.o ../Librairie/socket.h

clean:
	rm *o