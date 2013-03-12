/**
* \file service_register.cpp
*
* \brief service register che permette di registrare la corrispondenza tra servizio e service provider. Inoltre fornisce ai
*	 client tali corrispondenze. Per gestire le strutture dati interne utilizza una politica di locking lettori/scrittori
*/
#include "registro.h"

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

Servizi service;

void libera(int id){
	pthread_mutex_lock(&thread_mutex);
	close(sk[id]);
	occupati[id] = false;
	thread_disponibili++;
	pthread_cond_signal(&thread_cond);
	pthread_mutex_unlock(&thread_mutex);
	cout<<"REGISTER_THREAD[" <<id <<"]: liberato" <<endl;
}

void *th_body(void *arg){
	int id = *((int *)arg);
	command c;
	int r;
	result a;
	in_port_t server_port;
	in_addr_t server_addr;
	while(1){	
		sem_wait(&thread_sem[id]);
		cout<<"REGISTER_THREAD[" <<id <<"]: si risveglia" <<endl;
		
		r = read(sk[id], &c, sizeof(c));
		if(r == -1){
			libera(id);
			continue;
		}
		switch(c){
			case rec:{
					cout<<"REGISTER_THREAD[" <<id <<"]: ricevuta richiesta di registrazione servizio" <<endl;
					int dim;
					r = read(sk[id], &dim, sizeof(dim));
					if(r == -1)
						break;
					char* v = new char[dim];
					r = read(sk[id], v, dim);
					if(r == -1)
						break;
					string dsc(v,dim);
					r = read(sk[id], &server_port, sizeof(in_port_t));
					if(r == -1)
						break;
					r = read(sk[id],&server_addr,sizeof(in_addr_t));
					if(r == -1){
						a = error;
						write(sk[id],&a,sizeof(a));
						break;
					}
					int pos = dsc.find_first_of(';');
					string name;
					name = dsc.substr(0,pos);
					rw.richiesta_scrittura();
					service.insert(name,dsc);
					DescService* p = NULL;
					bool ris = service.retrive(name,p);
					if(!ris){
						a = error;
						write(sk[id],&a,sizeof(a));
						rw.fine_scrittura();
						break;
					}
					p->insert(server_port,server_addr);
					rw.fine_scrittura();
					a = ok;
					write(sk[id],&a,sizeof(a));
					break;					
				}
			case derec:{	cout<<"REGISTER_THREAD[" <<id <<"]: ricevuta richiesta di deregistrazione servizio"<<endl;
					int dim;
					r = read(sk[id], &dim, sizeof(dim));
					if(r == -1)
						break;
					char* v = new char[dim];
					r = read(sk[id], v, dim);
					if(r == -1)
						break;
					string name(v,dim);
					r = read(sk[id], &server_port, sizeof(in_port_t));
					if(r == -1)
						break;
					r = read(sk[id],&server_addr,sizeof(in_addr_t));
					if(r == -1){
						a = error;
						write(sk[id],&a,sizeof(a));
						break;
					}
					rw.richiesta_scrittura();
					DescService* p = NULL;
					bool ris = service.retrive(name,p);
					if(!ris){
						a = error;
						write(sk[id],&a,sizeof(a));
						rw.fine_scrittura();
						break;
					}
					ris = p->erase(server_port,server_addr);				
					rw.fine_scrittura();
					if(ris)
						a = ok;
					else
						a = error;
					write(sk[id],&a,sizeof(a));
					break;
				}
			case del:{	cout<<"REGISTER_THREAD[" <<id <<"]: ricevuta richiesta di deregistrazione server" <<endl;
					r = read(sk[id], &server_port, sizeof(in_port_t));
					if(r == -1)
						break;
					r = read(sk[id],&server_addr,sizeof(in_addr_t));
					if(r == -1){
						a = error;
						write(sk[id],&a,sizeof(a));
						break;
					}
					rw.richiesta_scrittura();
					DescService* p = service.testa;
					while(p != NULL){
						p->erase(server_port,server_addr);
						p = p->next;					
					}				
					rw.fine_scrittura();
					a = ok;
					write(sk[id],&a,sizeof(a));
					break;
				}
			case req:{	cout<<"REGISTER_THREAD[" <<id <<"]: ricevuta richiesta di servizio" <<endl;
					int dim;
					r = read(sk[id], &dim, sizeof(dim));
					if(r == -1)
						break;
					char* v = new char[dim];
					r = read(sk[id], v, dim);
					if(r == -1){
						a = error;
						write(sk[id],&a,sizeof(a));
						break;
					}
					string name(v,dim);
					rw.richiesta_lettura();
					DescService* p = NULL;
					bool ris = service.retrive(name,p);
					if(!ris){
						a = noservice;
						write(sk[id],&a,sizeof(a));
						rw.fine_lettura();
						break;
					}
					string dsc = p->desc;
					in_port_t porta;
					in_addr_t indirizzo;
					ris = p->scegli(porta,indirizzo);
					rw.fine_lettura();
					if(!ris){
						a = noservice;
						write(sk[id],&a,sizeof(a));
						break;
					}
					a = ok;
					write(sk[id],&a,sizeof(a));
					write(sk[id],&porta,sizeof(porta));					
					write(sk[id],&indirizzo,sizeof(indirizzo));
					dim = dsc.size();
					write(sk[id], &dim, sizeof(dim));				
					write(sk[id], dsc.data(), dim);
					break;
				}
			default:
					break;
		}
		libera(id);
	}
	return 0;
}

static void handler(int signo){
	if(signo == 2)
		cout<<"SERVICE_REGISTER: Catturato segnale SIGINT" <<endl;
	if(signo == 15)
		cout<<"SERVICE_REGISTER: Catturato segnale SIGTERM" <<endl;	
	exit(0);
}

int main(int quanti, char* quali[]){
	int port;
	int ascolto;
	int sock;
	size_t length=sizeof(struct sock_addr*);
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	struct sigaction nuova, vecchia;
	
	if(quanti != 2){
		cout<<"use: " <<quali[0] <<" port" <<endl;
		exit(0);
	}	

	port = atoi(quali[1]);	
	rw = Reader_Writer();
	service = Servizi();

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
	for(int i = 0; i < N; i++){
		occupati[i] = false;
		th_arg[i] = i;
		sem_init(&thread_sem[i],0,0);
	}

	
	pthread_mutex_lock(&thread_mutex);
	for(int i = 0; i < N; i++){
		pthread_create(&th[i], 0, th_body, &th_arg[i]);
		cout<<"REGISTER_THREAD[" <<i <<"]: attivato" <<endl;
	}
	pthread_mutex_unlock(&thread_mutex);
	bzero(&my_addr,sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	
	ascolto = socket(AF_INET,SOCK_STREAM,0);
	if(ascolto < 0){
		cout<<"SERVICE_REGISTER: Errore socket" <<endl;
		exit(0);
	}

	if(bind(ascolto,(struct sockaddr*)&my_addr,sizeof(my_addr)) < 0){
		cout<<"SERVICE_REGISTER: Errore bind" <<endl;
		close(ascolto);
		exit(0);
	}	

	if(listen(ascolto,5) < 0){
		cout<<"SERVICE_REGISTER: Errore listen" <<endl;
		close(ascolto);
		exit(0);
	}	

	while(1){
		int i;
		bzero(&client_addr,sizeof(struct sockaddr_in));
		sock = accept(ascolto,(struct sockaddr*)&client_addr,&length);
		if(sock < 0){
			cout<<"SERVICE_REGISTER: Errore accept" <<endl;
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
	return 0;
}
