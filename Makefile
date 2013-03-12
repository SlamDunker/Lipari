CC = g++
CFLAGS = -Wall
LIBS = -lpthread

all : ./Service_register/service_register ./Storage_provider/storage_provider ./Image_provider/image_provider ./Client/Client1/client1 ./Client/Client2/client2 ./Client/Client3/client3 ./Client/Client4/client4 ./Client/Client5/client5

./Service_register/service_register : ./Service_register/service_register.cpp ./Service_register/libreria.h ./Service_register/registro.h ./Service_register/libreria.cpp ./Service_register/registro.cpp
	$(CC) $(CFLAGS) -o $@ ./Service_register/service_register.cpp ./Service_register/libreria.cpp ./Service_register/registro.cpp $(LIBS)

./Storage_provider/storage_provider : ./Storage_provider/storage_provider.cpp ./Storage_provider/libreria.h ./Storage_provider/libreria.cpp ./Storage_provider/server1.h
	$(CC) $(CFLAGS) -o $@ ./Storage_provider/storage_provider.cpp ./Storage_provider/libreria.cpp $(LIBS)

./Image_provider/image_provider : ./Image_provider/image_provider.cpp ./Image_provider/libreria.h ./Image_provider/libreria.cpp ./Image_provider/server2.h
	$(CC) $(CFLAGS) -o $@ ./Image_provider/image_provider.cpp ./Image_provider/libreria.cpp $(LIBS)

./Client/Client1/client1 : ./Client/Client1/client1.cpp ./Client/Client1/libreria.h ./Client/Client1/libreria.cpp
	$(CC) $(CFLAGS) -o $@ ./Client/Client1/client1.cpp ./Client/Client1/libreria.cpp

./Client/Client2/client2 : ./Client/Client2/client2.cpp ./Client/Client2/libreria.h ./Client/Client2/libreria.cpp
	$(CC) $(CFLAGS) -o $@ ./Client/Client2/client2.cpp ./Client/Client2/libreria.cpp

./Client/Client3/client3 : ./Client/Client3/client3.cpp ./Client/Client3/libreria.h ./Client/Client3/libreria.cpp
	$(CC) $(CFLAGS) -o $@ ./Client/Client3/client3.cpp ./Client/Client3/libreria.cpp

./Client/Client4/client4 : ./Client/Client4/client4.cpp ./Client/Client4/libreria.h ./Client/Client4/libreria.cpp
	$(CC) $(CFLAGS) -o $@ ./Client/Client4/client4.cpp ./Client/Client4/libreria.cpp

./Client/Client5/client5 : ./Client/Client5/client5.cpp ./Client/Client5/libreria.h ./Client/Client5/libreria.cpp
	$(CC) $(CFLAGS) -o $@ ./Client/Client5/client5.cpp ./Client/Client5/libreria.cpp



