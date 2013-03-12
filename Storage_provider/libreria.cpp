/**
* \file libreria.cpp
*
* \brief implementazione del file libreria.h
*/
#include "libreria.h"

int secure_read(int sk, byte* buf, int quanti){
	int tot=0;
	int letti;
	do{		
		letti = read(sk,&buf[tot],quanti-tot);
		if(letti == -1)
			return (letti);
		tot = tot+letti;
	}while(tot < quanti);
	return 1;
}

BYTE::BYTE(){
	lun = 0;
	b = NULL;
}

BYTE::BYTE(byte* p, int l){
	lun = l;
	b = new byte[l];
	for(int i = 0;i < l;i++)
		b[i] = p[i];
}

BYTE::BYTE(const BYTE &b1){
	lun = b1.lun;
	b = new byte[lun];
	for(int i = 0;i < lun;i++)
		b[i] = b1.b[i];
}

Parametro::Parametro(tipo t1){
	t = t1;
	next = NULL;
}

Parametro::Parametro(const Parametro &p1){
	t = p1.t;
	next = p1.next;
}

tipo Parametro::get_tipo(){
	return t;
}

Parametro* Parametro::get_next(){
	return next;
}

void Parametro::set_next(Parametro* p){
	next = p;
}

bool Parametro::get_int(int &val){
	return false;
}

bool Parametro::get_double(double &val){
	return false;
}

bool Parametro::get_string(string &val){
	return false;
}

bool Parametro::get_byte(BYTE &val){
	return false;
}

Param_Int::Param_Int(int v, tipo t1) : Parametro(t1) {
	valore = v;
}

Param_Int::Param_Int(const Param_Int &p1) : Parametro(p1.t) {
	valore = p1.valore;
}

bool Param_Int::get_int(int &val){
	val = valore;
	return true;
}

Param_Double::Param_Double(double v, tipo t1) : Parametro(t1) {
	valore = v;
}

Param_Double::Param_Double(const Param_Double &p1) : Parametro(p1.t) {
	valore = p1.valore;
}

bool Param_Double::get_double(double &val){
	val = valore;
	return true;
}

Param_String::Param_String(string v, tipo t1) : Parametro(t1) {
	valore = v;
}

Param_String::Param_String(const Param_String &p1) : Parametro(p1.t) {
	valore = p1.valore;
}

bool Param_String::get_string(string &val){
	val = valore;
	return true;
}

Param_Byte::Param_Byte(BYTE v, tipo t1) : Parametro(t1) {
	valore = BYTE(v);
}

Param_Byte::Param_Byte(const Param_Byte &p1) : Parametro(p1.t) {
	valore = BYTE(p1.valore);
}

bool Param_Byte::get_byte(BYTE &val) {
	val = BYTE(valore);
	return true;
}

Parametri::Parametri() {
	testa = NULL;
	coda = NULL;
	quanti = 0; 
}

Parametro* Parametri::copia(Parametro* p){
	Parametro* q = NULL;
	tipo t = p->get_tipo();
	switch(t){
		case i: {	
			int vi;
			if(!(p->get_int(vi))){
				q = new Parametro(t);			
			}else{	
				q = new Param_Int (vi,t);
			}
			break;
		}
		case d:	{
			double vd;
			if(!(p->get_double(vd))){	
				q = new Parametro(t);
			}else{
				q = new Param_Double(vd,t);
			}
			break;
		}
		case s:	{
			string vs;
			if(!(p->get_string(vs))){	
				q = new Parametro(t);
			}else{
				q = new Param_String(vs,t);
			}
			break;
		}
		case b:	{
			BYTE vb;
			if(!(p->get_byte(vb))){	
				q = new Parametro(t);
			}else{
				q = new Param_Byte(vb,t);
			}
			break;
		}
	}
	return q;
}

void Parametri::insert(Parametro* p) {
	Parametro* q;
	q = copia(p);
	q->next = NULL;
	if(testa == NULL){
		testa = q;
		coda = testa;
		quanti++;
	}else{
		coda->next = q;
		coda = q;
		quanti++;
	}
}

int Parametri::get_quanti(){
	return quanti;
}

Parametro* Parametri::extract() {
	if(testa == NULL){
		return NULL;
	}
	Parametro* q;
	q = copia(testa);	
	if(testa != coda){
		coda->next = testa;
		coda = testa;
		testa = testa->next;
		coda->next = NULL;
	}
	q->next = NULL;
	return q;
}

Parametri::Parametri(const Parametri &p1) {
	quanti = 0;
	if(p1.testa == NULL){
		testa = NULL;
		coda = NULL;	
	}else{
		Parametro* p;
		Parametro* q;
		p = p1.testa;
		q = copia(p);
		q->next = NULL;
		testa = q;
		coda = testa;
		quanti++;
		p = p->next;
		for(int i = 1; i < p1.quanti; i++){
			q = copia(p);
			q->next = NULL;
			coda->next = q;
			coda = q;
			quanti++;
			p = p->next;			
		}
	}
}

Parametro* Parametri::get_testa(){
	return testa;
}

Service::Service(string n, Parametri i, Parametri o) {
	nome = n;
	in = Parametri(i);
	out = Parametri(o);
}

string Service::get_nome(){
	return nome;
}

string Service::descrizione(){
	Parametro* q;
	string dsc = string(nome);
	dsc = dsc+';';
	q = in.get_testa();
	for(int j = 0; j < in.get_quanti(); j++){		
		switch(q->get_tipo()){
			case i:	dsc = dsc+"IN;int;"; 
				break;
			case d:	dsc = dsc+"IN;double;";
				break;
			case s:	dsc = dsc+"IN;string;"; 
				break;
			case b:	dsc = dsc+"IN;buffer;";
				break;
		}
		q = q->next;	
	}
	q = out.get_testa();
	for(int j = 0; j < out.get_quanti(); j++){		
		switch(q->get_tipo()){
			case i:	dsc = dsc+"OUT;int;"; 
				break;
			case d:	dsc = dsc+"OUT;double;";
				break;
			case s:	dsc = dsc+"OUT;string;"; 
				break;
			case b:	dsc = dsc+"OUT;buffer;";
				break;
		}
		q = q->next;	
	}
	return dsc;
}

bool Service::verifica(Parametri p){
	if((in.get_quanti())!=(p.get_quanti()))
		return false;
	Parametro* q;
	Parametro* h;
	q = in.get_testa();
	h = p.get_testa();
	for(int j = 0; j < in.get_quanti(); j++){
		if((q->get_tipo())!=(h->get_tipo()))
			return false;
		q = q->next;
		h = h->next;	
	}
	return true;
}

bool Service::send_request(int sk, Parametri input){
	Parametro* p;
	tipo t;
	int n;
	char c;
	n = input.get_quanti();
	if(n == 0){
		c = 'N';
		write(sk, &c, sizeof(c));
		return true;	
	}
	c = 'S';
	write(sk, &c, sizeof(c));
	write(sk, &n, sizeof(int));
	p = input.get_testa();
	for(int j = 0; j < n; j++){
		t = p->get_tipo();
		write(sk, &t, sizeof(t));
		switch(t){
			case i:	{
				int vi;
				if(!(p->get_int(vi)))
					return false;			
				write(sk, &vi, sizeof(vi));
				break;
			}
			case d:	{
				double vd;
				if(!(p->get_double(vd)))
					return false;			
				write(sk, &vd, sizeof(vd));
				break;	
			}
			case s:	{
				string vs;
				if(!(p->get_string(vs)))
					return false;
				int dim = vs.size();
				write(sk, &dim, sizeof(dim));				
				write(sk, vs.data(), vs.size());
				break;
			}
			case b:	{
				BYTE vb;
				if(!(p->get_byte(vb)))
					return false;
				int dim = vb.lun;
				write(sk, &dim, sizeof(dim));
				
				int primi,secondi;
				secondi = dim%512;
				primi = dim-secondi;
				primi = primi/512;
				
				for(int j = 0; j < primi; j++)
					write(sk, &vb.b[j*512], 512);
				write(sk, &vb.b[primi*512], secondi);
				break;
			}
		}
		p = p->next;	
	}
	return true;
}

int Service::receive_request(int sk, Parametri &input){
	int r;
	char c;
	r = read(sk, &c, 1);
	if(r == -1)
		return -1;
	if((c != 'N')&&(c != 'S'))
		return -3;
	if(c == 'N'){
		input = Parametri();
	}else{
		tipo t;
		int n;
		Parametro* p;
		r = read(sk, &n, sizeof(n));
		if(r == -1)
			return -1;	
		for(int j = 0; j < n; j++){
			r = read(sk, &t, sizeof(t));
			if(r == -1)
				return -1;		
			switch(t){
				case i:	{
					int vi;			
					r = read(sk, &vi, sizeof(vi));
					if(r == -1)
						return -1;					
					p = new Param_Int(vi,t);
					break;
				}
				case d:	{
					double vd;			
					r = read(sk, &vd, sizeof(vd));
					if(r == -1)
						return -1;					
					p = new Param_Double(vd,t);
					break;
				}
				case s:	{
					int dim;			
					r = read(sk, &dim, sizeof(dim));
					if(r == -1)
						return -1;
					char* v = new char[dim];
					r = read(sk, v, dim);
					if(r == -1)
						return -1;
					string vs(v,dim);
					p = new Param_String (vs,t);
					break;
				}
				case b:	{
					int dim;			
					r = read(sk, &dim, sizeof(dim));
					if(r == -1)
						return -1;
					byte* v = new byte[dim];
					
					int primi,secondi;
					secondi = dim%512;
					primi = dim-secondi;
					primi = primi/512;
				
					for(int j = 0; j < primi; j++){
						r = secure_read(sk, &v[j*512], 512);
						if(r == -1)
							return -1;
					}
					r = secure_read(sk, &v[primi*512], secondi);			
					if(r == -1)
						return -1;
					BYTE vb(v,dim);					
					p = new Param_Byte (vb,t);
					break;
				}
			}
			input.insert(p);		
		}
	}
	return 1;
}

int Service::do_service(int sk,int id){
	return -10;
}

Response::Response(){}

bool Response::send_response(int sk, Parametri output){
	Parametro* p;
	tipo t;
	int n;
	n = output.get_quanti();
	if(n == 0)
		return false;	
	write(sk, &n, sizeof(int));
	p = output.get_testa();
	for(int j = 0; j < n; j++){
		t = p->get_tipo();
		write(sk, &t, sizeof(t));
		switch(t){
			case i:	{
				int vi;
				if(!(p->get_int(vi)))
					return false;			
				write(sk, &vi, sizeof(vi));
				break;
			}
			case d:	{
				double vd;
				if(!(p->get_double(vd)))
					return false;			
				write(sk, &vd, sizeof(vd));
				break;	
			}
			case s:	{
				string vs;
				if(!(p->get_string(vs)))
					return false;
				int dim = vs.size();
				write(sk, &dim, sizeof(dim));				
				write(sk, vs.data(), vs.size());
				break;
			}
			case b:	{
				BYTE vb;
				if(!(p->get_byte(vb)))
					return false;
				int dim = vb.lun;
				write(sk, &dim, sizeof(dim));

				int primi,secondi;
				secondi = dim%512;
				primi = dim-secondi;
				primi = primi/512;
				
				for(int j = 0; j < primi; j++)
					write(sk, &vb.b[j*512], 512);
				write(sk, &vb.b[primi*512], secondi);	
				break;
			}
		}
		p = p->next;	
	}
	return true;
}

int Response::receive_response(int sk, Parametri &output){
	int r;
	tipo t;
	int n;
	Parametro* p;
	r = read(sk, &n, sizeof(n));
	if(r == -1)
		return -1;		
	for(int j = 0; j < n; j++){
		r = read(sk, &t, sizeof(t));
		if(r == -1)
			return -1;			
		switch(t){
			case i:	{
				int vi;			
				r = read(sk, &vi, sizeof(vi));
				if(r == -1)
					return -1;					
				p = new Param_Int(vi,t);
				break;
			}
			case d:	{
				double vd;			
				r = read(sk, &vd, sizeof(vd));
				if(r == -1)
					return -1;					
				p = new Param_Double(vd,t);
				break;
			}
			case s:	{
				int dim;			
				r = read(sk, &dim, sizeof(dim));
				if(r == -1)
					return -1;
				char* v = new char[dim];
				r = read(sk, v, dim);
				if(r == -1)
					return -1;
				string vs(v,dim);
				p = new Param_String (vs,t);
				break;
			}
			case b:	{
				int dim;			
				r = read(sk, &dim, sizeof(dim));
				if(r == -1)
					return -1;
				byte* v = new byte[dim];
			
				int primi,secondi;
				secondi = dim%512;
				primi = dim-secondi;
				primi = primi/512;
			
				for(int j = 0; j < primi; j++){
					r = secure_read(sk, &v[j*512], 512);
					if(r == -1)
						return -1;
				}
				r = secure_read(sk, &v[primi*512], secondi);			
				if(r == -1)
					return -1;
				BYTE vb(v,dim);					
				p = new Param_Byte (vb,t);
				break;
			}
		}
		output.insert(p);		
	}
	return 1;
}

Register::Register(in_port_t p, in_addr_t a){
	port = p;
	addr = a;
}

void Register::set_port(in_port_t p){
	port = p;
}
	
void Register::set_addr(in_addr_t a){
	addr = a;
}
	
in_port_t Register::get_port(){
	return port;
}
	
in_addr_t Register::get_addr(){
	return addr;
}

int Register::rec_service(string dsc, in_port_t p, in_addr_t ad){
	int r;
	command c;
	int dim;
	result a;
	struct sockaddr_in server_addr;
	int sk;

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk < 0)
		return -4;	

	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = addr;

	r = connect(sk, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if(r == -1){
		close(sk);
		return -3;	
	}	

	c = rec;
	write(sk, &c, sizeof(c));
	dim = dsc.size();
	write(sk, &dim, sizeof(dim));				
	write(sk, dsc.data(), dim);
	write(sk, &p, sizeof(p));
	write(sk, &ad, sizeof(ad));

	r = read(sk, &a, sizeof(a));
	if(r == -1){
		close(sk);
		return -1;
	}
	
	close(sk);
	if(a == ok)
		return 0;
	else
		return -1;	
}

int Register::derec_service(string nome, in_port_t p, in_addr_t ad){
	int r;
	command c;
	int dim;
	result a;
	struct sockaddr_in server_addr;
	int sk;
	
	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk < 0)
		return -4;

	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = addr;

	r = connect(sk, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if(r == -1){
		close(sk);
		return -3;	
	}	
	
	c = derec;
	write(sk, &c, sizeof(c));
	dim = nome.size();
	write(sk, &dim, sizeof(dim));				
	write(sk, nome.data(), dim);
	write(sk, &p, sizeof(p));
	write(sk, &ad, sizeof(ad));

	r = read(sk, &a, sizeof(a));
	if(r == -1){
		close(sk);
		return -1;
	}
	
	close(sk);
	if(a == ok)
		return 0;
	else
		return -1;	
}

int Register::del_server(in_port_t p, in_addr_t ad){
	int r;
	command c;
	result a;
	struct sockaddr_in server_addr;
	int sk;

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk < 0)
		return -4;

	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = addr;

	r = connect(sk, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if(r == -1){
		close(sk);
		return -3;	
	}		

	c = del;
	write(sk, &c, sizeof(c));
	write(sk, &p, sizeof(p));
	write(sk, &ad, sizeof(ad));	

	r = read(sk, &a, sizeof(a));
	if(r == -1){
		close(sk);
		return -1;
	}
	
	close(sk);
	if(a == ok)
		return 0;
	else
		return -1;		
}

int Register::request_service(string nome, string &dsc, in_port_t &p, in_addr_t &a){
	int r;
	int dim;
	command c;
	result ris;
	struct sockaddr_in server_addr;
	int sk;

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk < 0)
		return -4;

	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = addr;

	r = connect(sk, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if(r == -1){
		close(sk);
		return -3;	
	}		

	c = req;
	write(sk, &c, sizeof(c));
	dim = nome.size();
	write(sk, &dim, sizeof(dim));				
	write(sk, nome.data(), dim);
	
	r = read(sk, &ris, sizeof(ris));
	if(r == -1){
		close(sk);
		return -1;
	}
	
	if(ris == error){
		close(sk);
		return -1;
	}
	else{
		if(ris == noservice){
			close(sk);
			return -5;
		}
		else{
			r = read(sk, &p, sizeof(in_port_t));
			if(r == -1){
				close(sk);
				return -1;
			}	
	
			r = read(sk,&a,sizeof(in_addr_t));
			if(r == -1){
				close(sk);
				return -1;
			}

			r = read(sk,&dim,sizeof(dim));
			if(r == -1){
				close(sk);
				return -1;
			}

			char* v = new char[dim];
			r = read(sk,v,dim);
			if(r == -1){
				close(sk);
				return -1;
			}
			dsc.assign(v,dim);
			close(sk);
			return 0;
		}
	}	
}

Reader_Writer::Reader_Writer(){
	scrittori_bloccati = 0;
	lettori_bloccati = 0;
	lettori_attivi = 0;
	scrittore_attivo = false;
	pthread_mutex_init(&mutex,0);
	pthread_cond_init(&scrittori,0);
	pthread_cond_init(&lettori,0);
}

void Reader_Writer::richiesta_lettura(){
	pthread_mutex_lock(&mutex);
	if((scrittori_bloccati > 0)||(scrittore_attivo)){
		lettori_bloccati++;
		pthread_cond_wait(&lettori,&mutex);
		lettori_bloccati--;
	}
	lettori_attivi++;
	pthread_mutex_unlock(&mutex);
}

void Reader_Writer::richiesta_scrittura(){
	pthread_mutex_lock(&mutex);
	if((lettori_attivi > 0)||(scrittore_attivo)){
		scrittori_bloccati++;
		pthread_cond_wait(&scrittori,&mutex);
		scrittori_bloccati--;
	}
	scrittore_attivo = true;
	pthread_mutex_unlock(&mutex);
}

void Reader_Writer::fine_lettura(){
	pthread_mutex_lock(&mutex);
	lettori_attivi--;
	if((lettori_attivi == 0)&&(scrittori_bloccati > 0))
		pthread_cond_signal(&scrittori);
	pthread_mutex_unlock(&mutex);
}

void Reader_Writer::fine_scrittura(){
	pthread_mutex_lock(&mutex);
	scrittore_attivo = false;
	if(lettori_bloccati > 0){
		pthread_cond_broadcast(&lettori);
	}else{
		if(scrittori_bloccati > 0){
			pthread_cond_signal(&scrittori);
		}
	}
	pthread_mutex_unlock(&mutex);
}
