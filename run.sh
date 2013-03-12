#!/bin/bash
REGPORT=12345
ADDR=127.0.0.1
IMGPORT=12347
STOREPORT=12346

./Service_register/service_register $REGPORT &
sleep 2
./Storage_provider/storage_provider $ADDR $STOREPORT $ADDR $REGPORT &
sleep 2
./Image_provider/image_provider $ADDR $IMGPORT $ADDR $REGPORT &
sleep 2
./Client/Client1/client1 $ADDR $REGPORT &
./Client/Client2/client2 $ADDR $REGPORT &
./Client/Client3/client3 $ADDR $REGPORT &
./Client/Client4/client4 $ADDR $REGPORT &
./Client/Client5/client5 $ADDR $REGPORT &
