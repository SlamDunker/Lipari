/**
* \file registro.h 
*
* \brief libreria che mette a disposizione funzionalita' utilizzabili dal service register
*/
#include "libreria.h"
#include <semaphore.h>
#include <netdb.h>
#include <signal.h>

/**
* \class AddrService
*
* \brief identifica un singolo service provider
*/
class AddrService {
	public:
	/// porta del service provider (in formato network)
	in_port_t porta;
	/// indirizzo ip del service provider
	in_addr_t ind;
	/// puntatore al successivo service provider
	AddrService* next;

	/**
	* \brief costruttore
	*
	* \param p porta del service provider (in formato network)
	* \param a indirizzo ip del service provider
	*/
	AddrService(in_port_t p, in_addr_t a);
};

/**
* \class DescService
*
* \brief identifica un singolo servizio (che puo' essere messo a disposizione da piu' server)
*/
class DescService {
	public:
	/// nome del servizio
	string nome;
	/// descrizione del servizio
	string desc;
	/// puntatore alla testa della lista di service provider che mettono a disposizione tale servizio
	AddrService* testa;
	/// puntatore al successivo servizio
	DescService* next;

	/**
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param d descrizione del servizio
	*/
	DescService(string n, string d);
	/**
	* \brief funzione che inserisce un service provider (che fornisce tale servizio) in coda alla lista (se non 
	*	 e' gia' presente)
	*
	* \param p porta del service provider (in formato network)
	* \param a indirizzo ip del service provider
	*
	* \return true in caso di successo, false se il service provider era gia' presente
	*/
	bool insert(in_port_t p, in_addr_t a);
	/**
	* \brief funzione che elimina il service provider specificato (che fornisce tale servizio) 
	*
	* \param p porta del service provider (in formato network)
	* \param a indirizzo ip del service provider
	*
	* \return true in caso di successo, false se il service provider non era presente
	*/
	bool erase(in_port_t p, in_addr_t a);
	/**
	* \brief funzione che sceglie un service provider (che fornisce tale servizio) secondo la politica 
	*	 round robin.
	*
	* \param p porta del service provider (in formato network)
	* \param a indirizzo ip del service provider
	*
	* \return true in caso di successo, false se il service provider era gia' presente
	*/
	bool scegli(in_port_t &p, in_addr_t &a);
};

/**
* \class Servizi
*
* \brief identifica una lista di oggetti di tipo DescService
*/
class Servizi {
	public:
	/// puntatore alla testa della lista
	DescService* testa;

	/// costruttore di default
	Servizi();
	/**
	* \brief funzione che cerca nella lista uno specifico servizio
	*
	* \param n nome del servizio da trovare
	*
	* \return true in caso di successo, false se il servizio non e' registrato
	*/
	bool find(string n);
	/**
	* \brief funzione che inserisce un servizio nella lista
	*
	* \param n nome del servizio da inserire
	* \param d descrizione del servizio da inserire
	*
	* \return true in caso di successo, false se il servizio era gia' registrato
	*/
	bool insert(string n, string d);
	/**
	* \brief funzione che un determinato servizio
	*
	* \param n nome del servizio da eliminare
	*
	* \return true in caso di successo, false se il servizio non e' presente o se ci sono ancora provider
	* 	  registrati che offrono tale servizio
	*/
	bool erase(string n);
	/**
	* \brief funzione che recupera il DescService di un determinato servizio
	*
	* \param n nome del servizio da recuperare
	* \param s puntatore che puntera' al DescService richiesto
	*
	* \return true in caso di successo, false se il servizio non e' registrato
	*/
	bool retrive(string n, DescService* &s);
};
