/**
* \file server1.h 
*
* \brief libreria che mette a disposizione funzionalita' utilizzabili dal service provider per lo storage delle immagini
*/
#include "libreria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
* \class Immagine 
*
* \brief implementa il tipo immagine
*/
class Immagine{
	public:
	/// nome immagine
	string nome;
	/// puntatore all'immagine successiva
	Immagine* next;

	/** 
	* \brief costruttore
	*
	* \param n nome del file
	*/
	Immagine(string n);
};

/**
* \class Immagini 
*
* \brief implementa una lista di oggetti di tipo immagine
*/
class Immagini{
	public:
	/// percorso in cui vengono si trovano le immagini
	string path;
	/// puntatore alla testa della lista
	Immagine* testa;

	/** 
	* \brief costruttore
	*
	* \param p percorso in cui si vogliono salvare le immagini
	*/
	Immagini(string p);
	/** 
	* \brief inserisce un oggetto Immagine in testa alla lista
	*
	* \param n nome del file
	*/
	void insert(string n);
	/** 
	* \brief verifica la presenza di un oggetto Immagine
	*
	* \param n nome del file
	*
	* \return true in caso di successo, false altrimenti
	*/
	bool find(string n);
	/** 
	* \brief crea una stringa con la lista dei nomi delle immagini presenti
	*
	* \param l stringa che conterra' i nomi delle immagini presenti
	*
	* \return true nel caso sia presente almeno un'immagine, false altrimenti
	*/
	bool list(string &l);
};

/**
* \class GetL 
*
* \brief eredita Service ed implementa il servizio Get List
*/
class GetL : public Service{
	public:
	/** 
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	GetL(string n,Parametri i,Parametri o);
	/** 
	* \brief implementa il servizio vero e proprio
	*
	* \param sk descrittore del socket
	* \param id identificativo del thread che esegue il servizio
	* 
	* \return 1 in caso di successo, -1 se errore generico, -3 se parametri errati
	*/
	int do_service(int sk,int id);
};

/**
* \class GetI 
*
* \brief eredita Service ed implementa il servizio Get Image
*/
class GetI : public Service{
	public:
	/** 
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	GetI(string n,Parametri i,Parametri o);
	/** 
	* \brief implementa il servizio vero e proprio
	*
	* \param sk descrittore del socket
	* \param id identificativo del thread che esegue il servizio
	* 
	* \return 1 in caso di successo, -1 se errore generico, -3 se parametri errati,
	*	  -4 se file non presente, -5 se errore nell'apertura del file	
	*/	
	int do_service(int sk,int id);
};

/**
* \class Store 
*
* \brief eredita Service ed implementa il servizio Store Image
*/
class Store : public Service{
	public:
	/** 
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	Store(string n,Parametri i,Parametri o);
	/** 
	* \brief implementa il servizio vero e proprio
	*
	* \param sk descrittore del socket
	* \param id identificativo del thread che esegue il servizio
	* 
	* \return 1 in caso di successo, -1 se errore generico, -3 se parametri errati,
	*	  -4 se file non presente, -5 se errore nell'apertura del file	
	*/		
	int do_service(int sk,int id);
};
