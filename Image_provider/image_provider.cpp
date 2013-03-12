/**
* \file image_provider.cpp
*
* \brief service provider che si registra al service register e mette a disposizione dei client i servizi per la modifica delle
*	 immagini
*/
#include "server2.h"

#define N 20

pthread_t th[N];
int th_arg[N];
int sk[N];
sem_t thread_sem[N];
bool occupati[N];
int thread_disponibili;
pthread_mutex_t thread_mutex;
pthread_cond_t thread_cond;

Service* rotate;
Service* flip;

Register* reg;
int my_port;
in_addr_t my_ind;

Rotate::Rotate(string n,Parametri i,Parametri o) : Service(n,i,o) {}
	
int Rotate::do_service(int sk,int id){
	Parametri input;
	string nomi;
	BYTE img;
	BYTE img2;
	int r;
	int num_char;
	result a;
	Parametro* q;
	int dir;
	bool ris;
	string path;
	FILE* f;
	int ultimi;
	int primi;

	r = receive_request(sk,input);
	if(r != 1)
		return r;
	if(!verifica(input))
		return -3;
		
	q = input.get_testa();
	ris = q->get_int(dir);
	if(!ris)
		return -1;
	if((dir != 1) && (dir != -1))
		return -3;
	q = q->next;
	ris = q->get_byte(img);
	if(!ris)
		return -1;

	ostringstream oss;
	oss << id;
	path = "./Image_provider/img/t";
	path = path + oss.str();
	path = path + ".jpg";

	f = fopen(path.c_str(),"w");
	if(f == NULL)
		return -5;

	ultimi = img.lun%512;
	primi = img.lun-ultimi;
	primi = primi/512;		
	for(int j = 0; j < primi; j++)
		fwrite(&img.b[j*512],sizeof(byte),512,f);
	fwrite(&img.b[primi*512],sizeof(byte),ultimi,f);
	fflush(f);
	fclose(f);
	
	CImg<unsigned char> image(path.c_str());
		
	//+ orario -antiorario
	image.rotate(dir*90,0,1);
	image.save(path.c_str(),-1);

	f = fopen(path.c_str(),"r");
	if(f == NULL)
		return -5;
	
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

	img2 = BYTE(buffer,num_char);
	Param_Byte* p1 = new Param_Byte(img2,b);

	Parametri output = Parametri();
	output.insert(p1);

	a = ok;
	write(sk,&a,sizeof(a));		
	resp.send_response(sk,output);

	pid_t child = fork();
	if(child == 0)
		execl("/bin/rm","rm",path.c_str(),NULL);
	waitpid(child,NULL,0);
	return 1;
}

Flip::Flip(string n,Parametri i,Parametri o) : Service(n,i,o) {}

int Flip::do_service(int sk,int id){
	Parametri input;
	string nomi;
	BYTE img;
	BYTE img2;
	int r;
	int num_char;
	result a;
	Parametro* q;
	bool ris;
	string path;
	FILE* f;
	int ultimi;
	int primi;

	r = receive_request(sk,input);
	if(r != 1)
		return r;
	if(!verifica(input))
		return -3;
		
	q = input.get_testa();
	ris = q->get_byte(img);
	if(!ris)
		return -1;

	ostringstream oss;
	oss << id;
	path = "./Image_provider/img/t";
	path = path + oss.str();
	path = path + ".jpg";

	f = fopen(path.c_str(),"w");
	if(f == NULL)
		return -5;
	
	ultimi = img.lun%512;
	primi = img.lun-ultimi;
	primi = primi/512;		
	for(int j = 0; j < primi; j++)
		fwrite(&img.b[j*512],sizeof(byte),512,f);
	fwrite(&img.b[primi*512],sizeof(byte),ultimi,f);
	fflush(f);
	fclose(f);

	CImg<unsigned char> image(path.c_str());
		
	image.mirror('x');
	image.save(path.c_str(),-1);

	f = fopen(path.c_str(),"r");
	if(f == NULL)
		return -5;
		
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

	img2 = BYTE(buffer,num_char);
	Param_Byte* p1 = new Param_Byte(img2,b);

	Parametri output = Parametri();
	output.insert(p1);
	a = ok;
	write(sk,&a,sizeof(a));		
	resp.send_response(sk,output);
		
	pid_t child = fork();
	if(child == 0)
		execl("/bin/rm","rm",path.c_str(),NULL);
	waitpid(child,NULL,0);
	return 1;
}

void libera(int id){
	pthread_mutex_lock(&thread_mutex);
	close(sk[id]);
	occupati[id] = false;
	thread_disponibili++;
	pthread_cond_signal(&thread_cond);
	pthread_mutex_unlock(&thread_mutex);
	cout<<"IMAGE_THREAD[" <<id <<"]: liberato" <<endl;
}

void my_rotate(int id){
	int r;
	result a;	
	r = rotate->do_service(sk[id],id);	
	if(r != 1){
		a = error;
		write(sk[id],&a,sizeof(a));
		libera(id);
		return;
	}
	libera(id);
}

void my_flip(int id){
	int r;
	result a;	
	r = flip->do_service(sk[id],id);		
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
		cout<<"IMAGE_THREAD[" <<id <<"]: si risveglia" <<endl;
		
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
		cout<<"IMAGE_THREAD[" <<id <<"]: ricevuto comando: " <<com <<endl;
		if(com == rotate->get_nome()){
			my_rotate(id);
		}
		else{
			if(com == flip->get_nome()){
				my_flip(id);	
			}
			else{
				cout<<"IMAGE_THREAD[" <<id <<"]: comando inesistente"<<endl;
				libera(id);
			}	
		}	
	}
	return 0;
}

void myexit(){
	pid_t child1,child2;
	cout<<"IMAGE_PROVIDER: Uscita e eliminazione servizi" <<endl;
	reg->del_server(my_port,my_ind);
	child1 = fork();
	if(child1 == 0)
		execl("/bin/rm","rm","-rf","./Image_provider/img",NULL);
	waitpid(child1,NULL,0);
	child2 = fork();
	if(child2 == 0)
		execl("/bin/mkdir","mkdir","./Image_provider/img",NULL);
	waitpid(child2,NULL,0);
}

static void handler(int signo){
	if(signo == 2)
		cout<<"IMAGE_PROVIDER: Catturato segnale SIGINT" <<endl;
	if(signo == 15)
		cout<<"IMAGE_PROVIDER: Catturato segnale SIGTERM" <<endl;	
	exit(0);
}

int main(int quanti, char* quali[]){
	int my_port;
	int register_port;
	int ascolto;
	int sock;
	int r;
	size_t length=sizeof(struct sock_addr*);
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	in_addr_t my_ind;
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

	pthread_mutex_init(&thread_mutex,0);
	pthread_cond_init(&thread_cond,0);
	thread_disponibili = N;
	for(int j = 0; j < N; j++){
		occupati[j] = false;
		th_arg[j] = j;
		sem_init(&thread_sem[j],0,0);
	}

	pthread_mutex_lock(&thread_mutex);
	for(int j = 0; j < N; j++){
		pthread_create(&th[j], 0, th_body, &th_arg[j]);
		cout<<"IMAGE_THREAD[" <<j <<"]: attivato" <<endl;
	}
	pthread_mutex_unlock(&thread_mutex);
	
	Parametri out1 = Parametri();
	Parametri in1 = Parametri();
	Parametro* dir1 = new Parametro(i);
	Parametro* img1 = new Parametro(b);
	Parametro* img2 = new Parametro(b);
	in1.insert(dir1);
	in1.insert(img1);
	out1.insert(img2);
	rotate = new Rotate("Rotate image",in1,out1);

	Parametri out2 = Parametri();
	Parametri in2 = Parametri();
	Parametro* img3 = new Parametro(b);
	Parametro* img4 = new Parametro(b);
	in2.insert(img3);
	out2.insert(img4);
	flip = new Flip("Flip image",in2,out2);

	dsc = rotate->descrizione();
	r = reg->rec_service(dsc,my_port,my_ind);
	if(r == 0){
		cout<<"IMAGE_PROVIDER: servizio Rotate image registrato"<<endl;
	}
	else{
		cout<<"IMAGE_PROVIDER: servizio Rotate image non registrato:"<<r <<endl;
		exit(0);
	}
	dsc = flip->descrizione();
	r = reg->rec_service(dsc,my_port,my_ind);
	if(r == 0){
		cout<<"IMAGE_PROVIDER: servizio Flip image registrato"<<endl;
	}
	else{
		cout<<"IMAGE_PROVIDER: servizio Flip image non registrato:"<<r <<endl;
		exit(0);
	}
	bzero(&my_addr,sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = my_port;
	my_addr.sin_addr.s_addr = INADDR_ANY;

	ascolto = socket(AF_INET,SOCK_STREAM,0);
	if(ascolto < 0){
		cout<<"IMAGE_PROVIDER: Errore socket" <<endl;
		exit(0);
	}

	if(bind(ascolto,(struct sockaddr*)&my_addr,sizeof(my_addr)) < 0){
		cout<<"IMAGE_PROVIDER: Errore bind" <<endl;
		close(ascolto);
		exit(0);
	}	

	if(listen(ascolto,5) < 0){
		cout<<"IMAGE_PROVIDER: Errore listen" <<endl;
		close(ascolto);
		exit(0);
	}	

	while(1){
		int i;
		bzero(&client_addr,sizeof(struct sockaddr_in));
		sock = accept(ascolto,(struct sockaddr*)&client_addr,&length);
		if(sock < 0){
			cout<<"IMAGE_PROVIDER: Errore accept" <<endl;
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
