/**
* \file storage_provider.cpp
*
* \brief service provider che si registra al service register e mette a disposizione dei client i servizi per lo storage delle
*        immagini. Per realizzare tali servizi il service provider usa la politica di lock lettori/scrittori per gestire le 
*        immagini
*/
#include "server1.h"

#define N 20

Reader_Writer rw;

pthread_t th[N];
int th_arg[N];
int sk[N];
sem_t thread_sem[N];
bool occupati[N];
int thread_disponibili;
pthread_mutex_t thread_mutex;
pthread_cond_t thread_cond;

Immagini* lista;

Service* store;
Service* geti;
Service* getl;

Register* reg;

int my_port;
in_addr_t my_ind;

Immagine::Immagine(string n){
	nome = n;
	next = NULL;
}

Immagini::Immagini(string p){
	testa = NULL;
	path = p;
}

void Immagini::insert(string n){
	Immagine* i = new Immagine(n);
	i->next = testa;
	testa = i;
}

bool Immagini::find(string n){
	Immagine* p = testa;
	while(p != NULL){
		if(p->nome == n)
			return true;
		p = p->next;
	}
	return false;
}
	
bool Immagini::list(string &l){
	if(testa == NULL)
		return false;
	l = "";
	Immagine* p = testa;
	while(p != NULL){
		l =  l+p->nome+";";
		p = p->next;
	}
	return true;
}

GetL::GetL(string n,Parametri i,Parametri o) : Service(n,i,o) {}

int GetL::do_service(int sk,int id){
	Parametri input;
	string nomi;
	BYTE img_list;
	int r;
	int dim_nomi;
	result a;
	bool ris;

	r = receive_request(sk,input);
	if(r != 1)
		return r;
	if(!verifica(input))
		return -3;
	
	rw.richiesta_lettura();
	ris = lista->list(nomi);
	rw.fine_lettura();
	if(ris){
		dim_nomi = nomi.size();	
		byte* buffer = new byte[dim_nomi];
		for(unsigned int j = 0; j < nomi.length(); j++)
			buffer[j] = nomi[j];		
		img_list = BYTE(buffer,dim_nomi);
	}
	else{
		img_list = BYTE();
	}

	Param_Byte* p1 = new Param_Byte(img_list,b);
	Parametri output = Parametri();
	output.insert(p1);
	a = ok;
	write(sk,&a,sizeof(a));		
	resp.send_response(sk,output);
	return 1;
}

GetI::GetI(string n,Parametri i,Parametri o) : Service(n,i,o) {}

int GetI::do_service(int sk,int id){
	Parametri input;
	Parametro* p;
	string nome;
	BYTE img;
	FILE* f;
	int r;
	bool ris;
	int dim_path;
	int dim_nome;
	int primi;
	int ultimi;
	int num_char;
	result a;
		
	r = receive_request(sk,input);
	if(r != 1)
		return r;
	if(!verifica(input))
		return -3;

	p = input.get_testa();
	ris = p->get_string(nome);
	if(!ris)
		return -1;

	rw.richiesta_lettura();	
	ris = lista->find(nome);
	if(!ris){
		rw.fine_lettura();
		return -4;
	}	
		
	dim_nome = nome.size();
	dim_path = lista->path.size();
	char* percorso = new char[dim_nome+dim_path+1];
	bzero(percorso,dim_nome+dim_path+1);
	strcpy(percorso,lista->path.data());
	strcat(percorso,"/");
	strcat(percorso,nome.data());

	f = fopen(percorso,"r");
	if(f == NULL){
		rw.fine_lettura();
		return -5;
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

	rw.fine_lettura();

	img = BYTE(buffer,num_char);
	Param_Byte* p1 = new Param_Byte(img,b);

	Parametri output = Parametri();
	output.insert(p1);
	a = ok;
	write(sk,&a,sizeof(a));		
	resp.send_response(sk,output);
	return 1;
}

Store::Store(string n,Parametri i,Parametri o) : Service(n,i,o) {}

int Store::do_service(int sk,int id){
	Parametri input;
	Parametro* p;
	string nome;
	BYTE img;
	FILE* f;
	int r;
	bool ris;
	int dim_path;
	int dim_nome;
	int primi;
	int ultimi;

	r = receive_request(sk,input);
	if(r != 1)
		return r;
	if(!verifica(input))
		return -3;
	
	p = input.get_testa();
	ris = p->get_string(nome);
	if(!ris)
		return -1;
	p = p->next;
	ris = p->get_byte(img);
	if(!ris)
		return -1;
	
	rw.richiesta_scrittura();
	dim_nome = nome.size();
	dim_path = lista->path.size();
	char* percorso = new char[dim_nome+dim_path+1];
	bzero(percorso,dim_nome+dim_path+1);
	strcpy(percorso,lista->path.c_str());
	strcat(percorso,"/");
	strcat(percorso,nome.c_str());

	f = fopen(percorso,"w");
	if(f == NULL){
		rw.fine_scrittura();
		return -5;
	}
	
	ultimi=img.lun%512;
	primi=img.lun-ultimi;
	primi=primi/512;		

	for(int j = 0; j < primi; j++)
		fwrite(&img.b[j*512],sizeof(byte),512,f);
	fwrite(&img.b[primi*512],sizeof(byte),ultimi,f);
	fclose(f);
	if(!(lista->find(nome)))
		lista->insert(nome);
	rw.fine_scrittura();
	return 1;
}

void libera(int id){
	pthread_mutex_lock(&thread_mutex);
	close(sk[id]);
	occupati[id] = false;
	thread_disponibili++;
	pthread_cond_signal(&thread_cond);
	pthread_mutex_unlock(&thread_mutex);
	cout<<"STORAGE_THREAD[" <<id <<"]: liberato" <<endl;
}

void my_store(int id){
	int r;
	result a;		
	r = store->do_service(sk[id],id);	
	if(r != 1){
		a = error;
		write(sk[id],&a,sizeof(a));
		libera(id);
		return;
	}	
	a = ok;
	write(sk[id],&a,sizeof(a));
	libera(id);
}

void my_geti(int id){
	int r;
	result a;	
	r = geti->do_service(sk[id],id);	
	if(r != 1){
		a = error;
		write(sk[id],&a,sizeof(a));
		libera(id);
		return;
	}	
	libera(id);
}

void my_getl(int id){
	int r;
	result a;
	r = getl->do_service(sk[id],id);	
	if(r != 1){
		a = error;
		write(sk[id],&a,sizeof(a));
		libera(id);
		return;
	}	
	libera(id);
}

void *th_body(void *arg){
	int id = *((int *)arg);
	int r;
	int dim;
	
	while(1){
		sem_wait(&thread_sem[id]);
		cout<<"STORAGE_THREAD[" <<id <<"]: si risveglia" <<endl;

		r = read(sk[id], &dim, sizeof(dim));
		if(r == -1){
			libera(id);
			continue;
		}
		char* c = new char[dim];
		r = read(sk[id], c, dim);
		if(r == -1){
			libera(id);
			continue;
		}
		
		string com = string(c,dim);
		cout<<"STORAGE_THREAD[" <<id <<"]: ricevuto comando: " <<com <<endl;
		if(com == store->get_nome()){
			my_store(id);
		}
		else{
			if(com == geti->get_nome()){
				my_geti(id);	
			}
			else{
				if(com == getl->get_nome()){
					my_getl(id);
				}
				else{
					cout<<"STORAGE_THREAD[" <<id <<"]: comando inesistente"<<endl;
					libera(id);
				}
			}
		}	
	}
	return 0;
}


void myexit(){
	pid_t child;
	int figli = 0;
	cout<<"STORAGE_PROVIDER: Uscita e eliminazione servizi" <<endl;
	reg->del_server(my_port,my_ind);
	Immagine* p = lista->testa;
	while(p != NULL){
		child = fork();
		if(child == 0){
			string percorso = lista->path + "/" + p->nome;
			execl("/bin/rm","rm",percorso.c_str(),NULL);
		}
		figli++;
		p = p->next;
	}
	for(int j = 0; j < figli; j++)
		wait(NULL);
}

static void handler(int signo){
	if(signo == 2)
		cout<<"STORAGE_PROVIDER: Catturato segnale SIGINT" <<endl;
	if(signo == 15)
		cout<<"STORAGE_PROVIDER: Catturato segnale SIGTERM" <<endl;
	exit(0);
}

int main(int quanti, char* quali[]){
	int register_port;
	int ascolto;
	int sock;
	int r;
	size_t length=sizeof(struct sock_addr*);
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	in_addr_t register_addr;
	struct in_addr mio_indirizzo;
	struct in_addr register_indirizzo;
	struct sigaction nuova, vecchia;
	string dsc;
	
	if(quanti != 5){
		cout<<"use: " <<quali[0] <<" Server_addr Server_port Register_addr Register_port" <<endl;
		exit(0);
	}	

	my_port = htons(atoi(quali[2]));	
	register_port = atoi(quali[4]);
	inet_aton(quali[1],&mio_indirizzo);
	inet_aton(quali[3],&register_indirizzo);
	my_ind = mio_indirizzo.s_addr;
	register_addr = register_indirizzo.s_addr;

	reg = new Register(htons(register_port),register_addr);

	atexit(myexit);
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

	rw = Reader_Writer();
	
	lista = new Immagini("./Storage_provider/img");

	pthread_mutex_init(&thread_mutex,0);
	pthread_cond_init(&thread_cond,0);
	thread_disponibili = N;
	for(int i = 0; i < N; i++){
		occupati[i] = false;
		th_arg[i] = i;
		sem_init(&thread_sem[i],0,0);
	}

	pthread_mutex_lock(&thread_mutex);
	for(int i = 0; i < N; i++){
		pthread_create(&th[i], 0, th_body, &th_arg[i]);
		cout<<"STORAGE_THREAD[" <<i <<"]: attivato" <<endl;
	}
	pthread_mutex_unlock(&thread_mutex);
	
	Parametri out1 = Parametri();
	Parametri in1 = Parametri();
	Parametro* nome1 = new Parametro(s);
	Parametro* img1 = new Parametro(b);
	in1.insert(nome1);
	in1.insert(img1);
	store = new Store("Store image",in1,out1);

	Parametri out2 = Parametri();
	Parametri in2 = Parametri();
	Parametro* nome2 = new Parametro(s);
	Parametro* img2 = new Parametro(b);
	in2.insert(nome2);
	out2.insert(img2);
	geti = new GetI("Get image",in2,out2);

	Parametri out3 = Parametri();
	Parametri in3 = Parametri();
	Parametro* img3 = new Parametro(b);
	out3.insert(img3);
	getl = new GetL("Get list",in3,out3);
	
	dsc = store->descrizione();
	r = reg->rec_service(dsc,my_port,my_ind);
	if(r == 0){
		cout<<"STORAGE_PROVIDER: servizio Store registrato"<<endl;
	}
	else{
		cout<<"STORAGE_PROVIDER: servizio Store non registrato:"<<r <<endl;
		exit(0);
	}
	dsc = geti->descrizione();
	r = reg->rec_service(dsc,my_port,my_ind);
	if(r == 0){
		cout<<"STORAGE_PROVIDER: servizio Get image registrato"<<endl;
	}
	else{
		cout<<"STORAGE_PROVIDER: servizio Get image non registrato:"<<r <<endl;
		exit(0);
	}
	dsc = getl->descrizione();
	r = reg->rec_service(dsc,my_port,my_ind);
	if(r == 0){
		cout<<"STORAGE_PROVIDER: servizio Get list registrato"<<endl;
	}
	else{
		cout<<"STORAGE_PROVIDER: servizio Get list non registrato:"<<r <<endl;
		exit(0);
	}

	bzero(&my_addr,sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = my_port;
	my_addr.sin_addr.s_addr = INADDR_ANY;

	ascolto = socket(AF_INET,SOCK_STREAM,0);
	if(ascolto < 0){
		cout<<"STORAGE_PROVIDER: Errore socket" <<endl;
		exit(0);
	}

	if(bind(ascolto,(struct sockaddr*)&my_addr,sizeof(my_addr)) < 0){
		cout<<"STORAGE_PROVIDER: Errore bind" <<endl;
		close(ascolto);
		exit(0);
	}	

	if(listen(ascolto,5) < 0){
		cout<<"STORAGE_PROVIDER: Errore listen" <<endl;
		close(ascolto);
		exit(0);
	}	

	while(1){
		int i;
		bzero(&client_addr,sizeof(struct sockaddr_in));
		sock = accept(ascolto,(struct sockaddr*)&client_addr,&length);
		if(sock < 0){
			cout<<"STORAGE_PROVIDER: Errore accept" <<endl;
			close(ascolto);
			exit(0);
		}

		pthread_mutex_lock(&thread_mutex);
		if(thread_disponibili == 0)
			pthread_cond_wait(&thread_cond,&thread_mutex);
		i = 0;
		while(occupati[i]) i++;
		occupati[i] = true;
		thread_disponibili--;
		sk[i] = sock;
		sem_post(&thread_sem[i]);
		pthread_mutex_unlock(&thread_mutex);
	}
	exit(0);
}
