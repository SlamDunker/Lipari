-----------------------------------------------------------------------------------------------------------------------------
Autori: Alessio Seghetti, Fabio Bindi
Email: {alessio.seghetti, f.bindi}@gmail.com
-----------------------------------------------------------------------------------------------------------------------------
Per procedere all'esecuzione:
	- compilare l'intero progetto tramite il comando make
	- eseguire lo script run.sh
-----------------------------------------------------------------------------------------------------------------------------
Per terminare i vari processi si possono inviare i seguenti segnali:
	- SIGINT
	- SIGTERM
-----------------------------------------------------------------------------------------------------------------------------
Se si desidera mandare in esecuzione singolarmente le varie componenti:
	- ./Service_register/service_register register_port
	- ./Storage_provider/storage_provider store_addr store_port register_addr register_port
	- ./Image_provider/image_provider image_addr image_port register_addr register_port
	- ./Client/Client1/client1 register_addr register_port
	- ./Client/Client2/client2 register_addr register_port
	- ./Client/Client3/client3 register_addr register_port
	- ./Client/Client4/client4 register_addr register_port
	- ./Client/Client5/client5 register_addr register_port
-----------------------------------------------------------------------------------------------------------------------------
Per una completa descrizione dell'applicazione consultare la documentazione Doxygen presente nella subdirectory Doxygen
-----------------------------------------------------------------------------------------------------------------------------
