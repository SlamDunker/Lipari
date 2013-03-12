/**
* \file registro.cpp 
*
* \brief implementazione del file registro.h
*/
#include "registro.h"

AddrService::AddrService(in_port_t p, in_addr_t a){
	porta = p;
	ind = a;
	next = NULL;
}

DescService::DescService(string n, string d){
	nome = n;
	desc = d;
	testa = NULL;
	next = NULL;
}

bool DescService::insert(in_port_t p, in_addr_t a){
	if(testa == NULL){
		testa = new AddrService(p,a);
		return true;
	}
	AddrService* q;
	AddrService* h;
	q = testa;
	h = testa;
	while(q != NULL){
		if((q->porta == p)&&(q->ind == a))
			return false;
		h = q;
		q = q->next;
	}
	h->next = new AddrService(p,a);
	return true;
}

bool DescService::erase(in_port_t p, in_addr_t a){
	AddrService* q;
	AddrService* h;
	q = testa;
	h = testa;
	while(q != NULL){
		if((q->porta == p)&&(q->ind == a)){
			if(h == q)
				testa = q->next;
			else
				h->next = q->next;
			delete q;
			return true;
		}	
		h = q;
		q = q->next;
	}
	return false;
}

bool DescService::scegli(in_port_t &p, in_addr_t &a){
	if(testa == NULL)
		return false;
	AddrService* q;
	AddrService* j;
	AddrService* h;
	q = testa;
	p = q->porta;
	a = q->ind;
	if(testa->next == NULL)
		return true;
	testa = testa->next;
	q->next = NULL;
	
	j = testa;
	h = testa;
	while(j != NULL){
		h = j;
		j = j->next;
	}	
	h->next = q;
	return true;
}

Servizi::Servizi(){
	testa = NULL;
}

bool Servizi::find(string n){
	DescService* q = testa;
	while(q != NULL){
		if(q->nome.compare(n) == 0)
			return true;
		q = q->next;
	}
	return false;
}

bool Servizi::insert(string n, string d){
	if(find(n))
		return false;
	DescService* q = new DescService(n,d);
	q->next = testa;
	testa = q;
	return true;
}

bool Servizi::retrive(string n, DescService* &s){
	DescService* q = testa;
	while(q != NULL){
		if(q->nome.compare(n) == 0){
			s = q;
			return true;
		}
		q = q->next;
	}
	return false;
}

bool Servizi::erase(string n){
	DescService* q;
	DescService* h;
	q = testa;
	h = testa;
	while(q != NULL){
		if(q->nome.compare(n) == 0){
			if(q->testa != NULL)
				return false;
			if(h == q)
				testa = q->next;
			else
				h->next = q->next;
			delete q;
			return true;
		}	
		h = q;
		q = q->next;
	}
	return false;
}
