.SILENT:



all:	./ClientQt/mainclient.o ./ClientQt/windowclient.o ./ClientQt/moc_windowclient.o ./Librairie/socket.o
		g++ -Wno-unused-parameter -o ./ClientQt/Client ./ClientQt/mainclient.o ./ClientQt/windowclient.o ./Librairie/socket.o ./ClientQt/moc_windowclient.o /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread
		g++ -o ./Serveur/Serveur ./Serveur/Serveur.cpp ./Librairie/socket.o -I/usr/include/mysql -lpthread -L/usr/lib64/mysql -lmysqlclient

./Librairie/socket.o:	./Librairie/socket.h	./Librairie/socket.cpp
	g++ -c ./Librairie/socket.cpp -o ./Librairie/socket.o

./ClientQt/mainclient.o:	./ClientQt/mainclient.cpp
		g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o ./ClientQt/mainclient.o ./ClientQt/mainclient.cpp

./ClientQt/windowclient.o:	./ClientQt/windowclient.cpp
		g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o ./ClientQt/windowclient.o ./ClientQt/windowclient.cpp

./ClientQt/moc_windowclient.o:	./ClientQt/moc_windowclient.cpp
		g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o ./ClientQt/moc_windowclient.o ./ClientQt/moc_windowclient.cpp


clean:
	rm ./ClientQt/*o
	rm ./Librairie/*o

clobber:
	rm ./ClientQt/Client
	rm ./Serveur/Serveur
