/**
* \file client3.cpp 
*
* \brief Il client ciclicamente legge casualmente un file .jpg o da disco o dal provider poi casualmente ne chiede o la rotazione
*        oppure il flip al provider competente. Infine chiede la memorizzazione dell'immagine ottenuta al provider che fornisce
*        il servizio Store image.
*/
#include "libreria.h"
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctime>
/**
* \brief Funzione che crea il socket e connette il client al server specificato.
*
* \param server_port Numero porta del server
* \param server_addr Indirizzio ip del server
*
* \return descrittore del socket in caso di successo
*	  -4 errore socket
*         -3 errore connect
*/
int crea_connetti(in_port_t server_port,in_addr_t server_addr){
	int sk;
	int r;
	struct sockaddr_in srv_addr;

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk < 0){
		cout<<"CLIENT3: Errore socket" <<endl;
		return -4;	
	}

	bzero(&server_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = server_port;
	srv_addr.sin_addr.s_addr = server_addr;

	r = connect(sk, (struct sockaddr*)&srv_addr, sizeof(struct sockaddr_in));
	if(r == -1){
		close(sk);
		cout<<"CLIENT3: Errore connect" <<endl;
		return -3;	
	}
	return sk;
}

static void handler(int signo){
	if(signo == 2)
		cout<<"CLIENT3: Catturato segnale SIGINT" <<endl;
	if(signo == 15)
		cout<<"CLIENT3: Catturato segnale SIGTERM" <<endl;	
	exit(0);
}

int main(int quanti, char* quali[]){
	in_port_t server_port;
	in_port_t register_port;
	in_addr_t server_addr;
	in_addr_t register_addr;
	struct in_addr indirizzo;
	int sk;
	FILE *f;
	int num_char;
	char* percorso;
	Service* servizio;
	Parametri in;
	Parametri o;
	result a;
	int r;
	bool ris;
	int ultimi,primi;
	BYTE img;
	bool disco;
	int casuale;
	int casuale2;
	int casuale3;
	string nome_img[5];
	string dsc;
	int dim;
	int dir;
	int quante;
	Parametro* p;
	Parametri input;
	BYTE img2;
	Parametri output;
	string nome;
	string name_service;
	struct sigaction nuova, vecchia;

	if(quanti != 3){
		cout<<"use: " <<quali[0] <<" Register_addr Register_port" <<endl;
		exit(0);
	}	

	register_port = htons(atoi(quali[2]));
	inet_aton(quali[1],&indirizzo);
	register_addr = indirizzo.s_addr;

	nuova.sa_handler = handler;
	sigemptyset(&nuova.sa_mask);
	nuova.sa_flags = 0;
	sigaction(SIGINT,&nuova,&vecchia);
	nuova.sa_handler = handler;
	sigemptyset(&nuova.sa_mask);
	nuova.sa_flags = 0;
	sigaction(SIGTERM,&nuova,&vecchia);
	nuova.sa_handler = SIG_IGN;
	sigemptyset(&nuova.sa_mask);
	nuova.sa_flags = 0;
	sigaction(SIGPIPE,&nuova,&vecchia);

	Register reg = Register(register_port,register_addr);
	
	nome_img[0] = "big_ban.jpg";
	nome_img[1] = "Colosseo.jpg";
	nome_img[2] = "sole.jpg";
	nome_img[3] = "Stemma_unipi.jpg";
	nome_img[4] = "torre_eiffel.jpg";
	
	srand(time(NULL));
	while(1){
		//0 disco; 1 service_provider
		casuale = 0 + rand() % 2;
		if(casuale == 0)
			disco = true;
		else
			disco = false;
		if(disco){
			casuale = 0 + rand() % 5;
			percorso = new char[nome_img[casuale].size()+21];
			nome = nome_img[casuale];
			strcpy(percorso,"./Client/Client3/img/");
			strcat(percorso,nome_img[casuale].data());
			cout<<"CLIENT3: scelto file " <<nome <<" da disco" <<endl;
		}
		else{
			name_service = "Get list";
			r = reg.request_service(name_service,dsc,server_port,server_addr);
			if(r != 0){
				cout<<"CLIENT3: Errore comunicazione con server_register" <<endl;
				if(r == -5)
					continue;
				exit(0);
			}
			sk = crea_connetti(server_port,server_addr);
			if(sk < 0 ){
				cout<<"CLIENT3: Impossibile connettersi con il server" <<endl;
				continue;
			}
			cout<<"CLIENT3: richiesta comando Get list" <<endl;

			servizio = new Service("Get list",in,o);	
			dim = 8;
			write(sk, &dim, sizeof(dim));			
			write(sk, "Get list", dim);
			input = Parametri();
			ris = servizio->send_request(sk,input);
			if(!ris){
				cout<<"CLIENT3: Errore invio parametri" <<endl;
				close(sk);
				continue;
			}

			r = read(sk,&a,sizeof(a));
			if((r == -1)||(a != ok)){
				cout<<"CLIENT3: Errore lettura esito" <<endl;
				close(sk);
				continue;
			}
	
			output = Parametri();
			ris = servizio->resp.receive_response(sk,output);
			if(!ris){
				cout<<"CLIENT3: Errore ricezione lista" <<endl;
				close(sk);
				continue;
			}
			close(sk);
			p = output.get_testa();
			BYTE img_list;
			ris = p->get_byte(img_list);
			if(!ris){
				cout<<"CLIENT3: Errore recupero lista" <<endl;
				continue;
			}
			quante = 0;
			cout<<"CLIENT3: Lista: ";
			char* lista = new char[img_list.lun];
			for(int j = 0; j < img_list.lun; j++){
				if(img_list.b[j] == ';'){
					quante++;
					cout<<" ";
				}
				else{	
					cout<<img_list.b[j];
				}
				lista[j] = img_list.b[j];
			}	
			cout<<endl;
			if(quante == 0)
				continue;
			
			char* name;
			casuale3 = 0 + rand() % (quante);
			name = strtok(lista,";");
			for(int j = 0; j < casuale3; j++)
				name = strtok(NULL,";");
			nome = string(name,strlen(name));

			cout<<"CLIENT3: scelto il file " <<nome <<endl;
			
			sk = crea_connetti(server_port,server_addr);
			if(sk < 0 ){
				cout<<"CLIENT3: Impossibile connettersi con il server" <<endl;
				continue;
			}
			cout<<"CLIENT3: richiesta comando Get image" <<endl;

			servizio = new Service("Get image",in,o);	
			dim = 9;
			write(sk, &dim, sizeof(dim));			
			write(sk, "Get image", dim);
			input = Parametri();
			Param_String* p1 = new Param_String(nome,s);
			input.insert(p1);
			ris = servizio->send_request(sk,input);
			if(!ris){
				cout<<"CLIENT3: Errore invio parametri" <<endl;
				close(sk);
				continue;
			}
	
			r = read(sk,&a,sizeof(a));
			if((r == -1)||(a != ok)){
				cout<<"CLIENT3: Errore lettura esito" <<endl;
				close(sk);
				continue;
			}
	
			output = Parametri();
			ris = servizio->resp.receive_response(sk,output);
			if(!ris){
				cout<<"CLIENT3: Errore ricezione immagine" <<endl;
				close(sk);
				continue;
			}
			close(sk);

			p = output.get_testa();
			ris = p->get_byte(img);
			if(!ris){
				cout<<"CLIENT3: Errore recupero immagine" <<endl;
				continue;
			}
			percorso = new char[nome.size()+21];
			strcpy(percorso,"./Client/Client3/img/");
			strcat(percorso,nome.data());
			f=fopen(percorso,"w");
			if(f == NULL){
				cout<<"CLIENT3: errore nell'aperture del file" <<endl;
				continue;
			}
	
			ultimi=img.lun%512;
			primi=img.lun-ultimi;
			primi=primi/512;		

			for(int j = 0; j < primi; j++){
				fwrite(&img.b[j*512],sizeof(byte),512,f);
			}
			fwrite(&img.b[primi*512],sizeof(byte),ultimi,f);
			fclose(f);
		}
		//0 Rotate; 1 Flip
		casuale2 = 0 + rand() % 2;
		if(casuale2 == 0){
			name_service = "Rotate image";
			r = reg.request_service(name_service,dsc,server_port,server_addr);
			if(r != 0){
				cout<<"CLIENT3: Errore comunicazione con server_register" <<endl;
				if(r == -5)
					continue;
				exit(0);
			}
			sk = crea_connetti(server_port,server_addr);
			if(sk < 0 ){
				cout<<"CLIENT3: Impossibile connettersi con il server" <<endl;
				continue;
			}
			cout<<"CLIENT3: richiesta comando Rotate image" <<endl;

			servizio = new Service("Rotate image",in,o);
			dim = 12;
			write(sk, &dim, sizeof(dim));			
			write(sk, "Rotate image", dim);
			//scelta direzione
			casuale2 = 0 + rand() % 2;
			if(casuale2 == 0)
				dir = -1;
			else
				dir = 1;
			Param_Int* p2 = new Param_Int(dir,i);
			f = fopen(percorso,"r");
			if(f == NULL){
				cout<<"CLIENT3: errore nell'aperture del file";
				continue;
			}
			fseek(f,0,SEEK_END);				
			num_char = ftell(f);			
			fseek(f,0,SEEK_SET);
	
			byte* buffer = new byte[num_char];
	
			ultimi = num_char%512;
			primi = num_char-ultimi;
			primi = primi/512;

			for(int i = 0; i < primi; i++)
				fread(&buffer[i*512],sizeof(byte),512,f);	
			fread(&buffer[primi*512],sizeof(byte),ultimi,f);
			fclose(f);

			img = BYTE(buffer,num_char);	
			Param_Byte* p3 = new Param_Byte(img,b);
			input = Parametri();
			input.insert(p2);
			input.insert(p3);
			ris = servizio->send_request(sk,input);
			if(!ris){
				cout<<"CLIENT3: Errore invio parametri" <<endl;
				close(sk);
				continue;
			}
	
			r = read(sk,&a,sizeof(a));
			if((r == -1)||(a != ok)){
				cout<<"CLIENT3: Errore lettura esito" <<endl;
				close(sk);
				continue;
			}
			
			output = Parametri();
			ris = servizio->resp.receive_response(sk,output);
			if(!ris){
				cout<<"CLIENT3: Errore ricezione immagini" <<endl;
				close(sk);
				continue;
			}
	
			close(sk);

			p = output.get_testa();			
			ris = p->get_byte(img2);
			if(!ris){
				cout<<"CLIENT3: Errore recupero immagine" <<endl;
				continue;
			}

			f=fopen(percorso,"w");
			if(f == NULL){
				cout<<"CLIENT3: errore nell'aperture del file" <<endl;
				continue;
			}
	
			ultimi=img2.lun%512;
			primi=img2.lun-ultimi;
			primi=primi/512;		

			for(int j = 0; j < primi; j++){
				fwrite(&img2.b[j*512],sizeof(byte),512,f);
			}
			fwrite(&img2.b[primi*512],sizeof(byte),ultimi,f);
			fclose(f);	
		}
		else{
			name_service = "Flip image";
			r = reg.request_service(name_service,dsc,server_port,server_addr);
			if(r != 0){
				cout<<"CLIENT3: Errore comunicazione con server_register" <<endl;
				if(r == -5)
					continue;
				exit(0);
			}
			sk = crea_connetti(server_port,server_addr);
			if(sk < 0 ){
				cout<<"CLIENT3: Impossibile connettersi con il server" <<endl;
				continue;
			}
			cout<<"CLIENT3: richiesta comando Flip image" <<endl;

			servizio = new Service("Flip image",in,o);
			dim = 10;
			write(sk, &dim, sizeof(dim));			
			write(sk, "Flip image", dim);
			f = fopen(percorso,"r");
			if(f == NULL){
				cout<<"CLIENT3: errore nell'aperture del file";
				continue;
			}
			fseek(f,0,SEEK_END);				
			num_char = ftell(f);			
			fseek(f,0,SEEK_SET);
	
			byte* buffer = new byte[num_char];
	
			ultimi = num_char%512;
			primi = num_char-ultimi;
			primi = primi/512;

			for(int i = 0; i < primi; i++)
				fread(&buffer[i*512],sizeof(byte),512,f);	
			fread(&buffer[primi*512],sizeof(byte),ultimi,f);
			fclose(f);

			img = BYTE(buffer,num_char);	
			Param_Byte* p4 = new Param_Byte(img,b);
			input = Parametri();
			input.insert(p4);
			ris = servizio->send_request(sk,input);
			if(!ris){
				cout<<"CLIENT3: Errore invio parametri" <<endl;
				close(sk);
				continue;
			}
	
			r = read(sk,&a,sizeof(a));
			if((r == -1)||(a != ok)){
				cout<<"CLIENT3: Errore lettura esito" <<endl;
				close(sk);
				continue;
			}

			output = Parametri();
			ris = servizio->resp.receive_response(sk,output);
			if(!ris){
				cout<<"CLIENT3: Errore ricezione immagini" <<endl;
				close(sk);
				continue;
			}
	
			close(sk);

			p = output.get_testa();
			ris = p->get_byte(img2);
			if(!ris){
				cout<<"CLIENT3: Errore recupero immagine" <<endl;
				continue;
			}

			f=fopen(percorso,"w");
			if(f == NULL){
				cout<<"CLIENT3: errore nell'aperture del file" <<endl;
				continue;
			}
	
			ultimi=img2.lun%512;
			primi=img2.lun-ultimi;
			primi=primi/512;		

			for(int j = 0; j < primi; j++){
				fwrite(&img2.b[j*512],sizeof(byte),512,f);
			}
			fwrite(&img2.b[primi*512],sizeof(byte),ultimi,f);
			fclose(f);	
		}
		name_service = "Store image";
		r = reg.request_service(name_service,dsc,server_port,server_addr);
		if(r != 0){
			cout<<"CLIENT3: Errore comunicazione con server_register" <<endl;
			if(r == -5)
					continue;
			exit(0);
		}
		sk = crea_connetti(server_port,server_addr);
		if(sk < 0 ){
				cout<<"CLIENT3: Impossibile connettersi con il server" <<endl;
				continue;
		}
		cout<<"CLIENT3: richiesta comando Store image" <<endl;

		servizio = new Service("Store image",in,o);
		dim = 11;
		write(sk, &dim, sizeof(dim));			
		write(sk, "Store image", dim);	

		Param_String* p5 = new Param_String(nome,s);
		input = Parametri();
		input.insert(p5);
		f=fopen(percorso,"r");
		if(f == NULL){
			cout<<"CLIENT3: errore nell'aperture del file" <<endl;
			continue;
		}
			
		fseek(f,0,SEEK_END);				
		num_char=ftell(f);			
		fseek(f,0,SEEK_SET);
	
		byte* buffer = new byte[num_char];

		ultimi = num_char%512;
		primi = num_char-ultimi;
		primi = primi/512;

		for(int i = 0; i < primi; i++)
			fread(&buffer[i*512],sizeof(byte),512,f);								
		fread(&buffer[primi*512],sizeof(byte),ultimi,f);
		fclose(f);

		BYTE img = BYTE(buffer,num_char);
		Param_Byte* p6 = new Param_Byte(img,b);
		input.insert(p6);
		ris = servizio->send_request(sk,input);
		if(!ris){
			cout<<"CLIENT3: Errore invio parametri" <<endl;
			continue;
		}

		r = read(sk,&a,sizeof(a));
		if(r == -1){
			cout<<"CLIENT3:Errore lettura risultato" <<endl;
			continue;
		}
		cout<<"CLIENT3: Immagine " <<nome <<" salvata sul server"<<endl;
	}
}
