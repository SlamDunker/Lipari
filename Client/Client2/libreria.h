/**
* \file libreria.h 
*
* \brief libreria che mette a disposizione funzionalita' utilizzabili da ogni entita' del nostro scenario
*/
#include <iostream>
#include <cstdlib>
#include <string>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>

using namespace std;
/**
* \enum tipo
*
* \brief tipi di parametri consentiti
*/
typedef enum {i, d, s, b} tipo;
/**
* \enum command
*
* \brief tipi di comandi per il service_register consentiti
*/
typedef enum {rec, derec, del, req} command;
/**
* \enum result
*
* \brief tipi di risultati consentiti
*/
typedef enum {ok, error, noservice} result;
/**
* \typedef byte
*/
typedef unsigned char byte;
/**
* \brief funzione di supporto per la lettura di byte da un socket
*
* \param sk descrittore del socket
* \param buf puntatore al buffer che conterra' i byte letti
* \param quanti numero di byte da leggere
*
* \return 1 in caso di successo, -1 altrimenti
*/
int secure_read(int sk, byte* buf, int quanti);
/**
* \class BYTE
*
* \brief implementa il tipo buffer di byte
*/
class BYTE {
	public:
	/// buffer di byte 
	byte* b;
	/// lunghezza del buffer
	int lun;
	
	/// costruttore default
	BYTE();
	/**
	* \brief costruttore
	*
	* \param p buffer di byte
	* \param l lunghezza del buffer
	*/
	BYTE(byte* p, int l);
	/// costruttore di copia
	BYTE(const BYTE &b1);
};
/**
* \class Parametro
*
* \brief implementa un singolo parametro generico
*/
class Parametro {
	protected:
	/// tipo del parametro
	tipo t;

	public:
	/// puntatore al parametro successivo
	Parametro* next;

	/**
	* \brief costruttore
	*
	* \param t1 tipo del parametro
	*/
	Parametro(tipo t1);
	///costruttore di copia
	Parametro(const Parametro &p1);
	/**
	* \brief recupera il tipo del parametro
	*
	* \return tipo del parametro
	*/
	tipo get_tipo();
	/**
	* \brief recupera il parametro successivo
	*
	* \return puntatore al parametro successivo
	*/
	Parametro* get_next();
	/**
	* \brief imposta il parametro successivo
	*/
	void set_next(Parametro* n);
	/**
	* \brief funzione virtuale che recupera il valore del tipo intero. Dovra' essere ridefinita dalle classi
	* 	 che la estendono
	*
	* \param val variabile che conterra' il valore intero
	*
	* \return false
	*/
	bool virtual get_int(int &val);
	/**
	* \brief funzione virtuale che recupera il valore del tipo double. Dovra' essere ridefinita dalle classi
	* 	 che la estendono
	*
	* \param val variabile che conterra' il valore double
	*
	* \return false
	*/
	bool virtual get_double(double &val);
	/**
	* \brief funzione virtuale che recupera il valore del tipo string. Dovra' essere ridefinita dalle classi
	* 	 che la estendono
	*
	* \param val variabile che conterra' il valore string
	*
	* \return false
	*/
	bool virtual get_string(string &val);
	/**
	* \brief funzione virtuale che recupera il valore del tipo BYTE. Dovra' essere ridefinita dalle classi
	* 	 che la estendono
	*
	* \param val variabile che conterra' il valore BYTE
	*
	* \return false
	*/
	bool virtual get_byte(BYTE &val);
};

/**
* \class Param_Int
*
* \brief implementa un singolo parametro intero
*/
class Param_Int : public Parametro {
	private:
	/// valore del parametro
	int valore;
	
	public:
	/**
	* \brief costruttore
	*
	* \param v valore del parametro
	* \param t1 tipo del parametro
	*/
	Param_Int(int v, tipo t1);
	/// costruttore di copia
	Param_Int(const Param_Int &p1);
	/**
	* \brief recupera il valore del tipo intero
	*
	* \param val variabile che conterra' il valore intero
	*
	* \return true in caso di successo altrimenti false
	*/
	bool get_int(int &val);
};

/**
* \class Param_Double
*
* \brief implementa un singolo parametro double
*/
class Param_Double : public Parametro {
	private:
	/// valore del parametro
	double valore;

	public:
	/**
	* \brief costruttore
	*
	* \param v valore del parametro
	* \param t1 tipo del parametro
	*/
	Param_Double(double v, tipo t1);
	/// costruttore di copia
	Param_Double(const Param_Double &p1);
	/**
	* \brief recupera il valore del tipo double
	*
	* \param val variabile che conterra' il valore double
	*
	* \return true in caso di successo altrimenti false
	*/
	bool get_double(double &val);
};

/**
* \class Param_String
*
* \brief implementa un singolo parametro string
*/
class Param_String : public Parametro {
	private:
	/// valore del parametro
	string valore;

	public:
	/**
	* \brief costruttore
	*
	* \param v valore del parametro
	* \param t1 tipo del parametro
	*/
	Param_String(string v, tipo t1);
	/// costruttore di copia
	Param_String(const Param_String &p1);
	/**
	* \brief recupera il valore del tipo string
	*
	* \param val variabile che conterra' il valore string
	*
	* \return true in caso di successo altrimenti false
	*/
	bool get_string(string &val);
};

/**
* \class Param_Byte
*
* \brief implementa un singolo parametro BYTE
*/
class Param_Byte : public Parametro {
	private:
	/// valore del parametro
	BYTE valore;

	public:
	/**
	* \brief costruttore
	*
	* \param v valore del parametro
	* \param t1 tipo del parametro
	*/
	Param_Byte(BYTE v, tipo t1);
	/// costruttore di copia
	Param_Byte(const Param_Byte &p1);
	/**
	* \brief recupera il valore del tipo BYTE
	*
	* \param val variabile che conterra' il valore BYTE
	*
	* \return true in caso di successo altrimenti false
	*/
	bool get_byte(BYTE &val);
};

/**
* \class Parametri
*
* \brief implementa una lista di parametri
*/
class Parametri {
	private:
	/// puntatore alla testa della lista
	Parametro* testa;
	/// puntatore alla coda della lista
	Parametro* coda;
	/// numero di elementi presenti nella lista
	int quanti;

	public:
	/// costruttore di default
	Parametri();
	/// costruttore di copia
	Parametri(const Parametri &p1);
	/**
	* \brief recupera la testa della lista
	*
	* \return puntatore alla testa della lista
	*/
	Parametro* get_testa();
	/**
	* \brief inserisce un elemento in coda
	*
	* \param p puntatore all'oggetto da inserire
	*/
	void insert(Parametro* p);
	/**
	* \brief estrazione in testa e spostamento in coda dell'elemento estratto
	*
	* \return puntatore ad un oggetto creato uguale a quello estratto
	*/
	Parametro* extract();
	/**
	* \brief copia di un parametro
	*
	* \param p puntatore al parametro da copiare
	*
	* \return puntatore al nuovo oggetto
	*/
	Parametro* copia(Parametro* p);
	/**
	* \brief recupera il numero di elementi della lista
	*
	* \return numero di elementi della lista
	*/
	int get_quanti();
};

/**
* \class Response
*
* \brief implementa l'invio e la ricezione della risposta 
*/
class Response {
	public:
	/// costruttore di default
	Response();
	/**
	* \brief invia sul socket la lista dei parametri di risposta
	*
	* \param sk descrittore del socket
	* \param output lista dei parametri di risposta
	*
	* \return true in caso di successo altrimenti false
	*/
	bool send_response(int sk, Parametri output);
	/**
	* \brief riceve sul socket la lista dei parametri di risposta
	*
	* \param sk descrittore del socket
	* \param output lista che conterra' i parametri di risposta
	*
	* \return 1 in caso di successo, -1 se errore generico
	*/
	int receive_response(int sk, Parametri &output);
};

/**
* \class Service
*
* \brief implementa un servizio generico 
*/
class Service {
	private:
	/// nome del servizio
	string nome;
	/// lista parametri input
	Parametri in;
	/// lista parametri output
	Parametri out;
	

	public:
	/// oggetto Response per la risposta del servizio
	Response resp;	

	/**
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	Service(string n, Parametri i, Parametri o);
	/**
	* \brief crea una stringa che contiene la descrizione del servizio
	*
	* \return stringa descrizione
	*/
	string descrizione();
	/**
	* \brief recupera il nome del servizio
	*
	* \return nome del servizio
	*/
	string get_nome();
	/**
	* \brief verifica la correttezza della richiesta di servizio
	*
	* \param p lista di parametri ricevuti dal server
	*
	* \return true in caso di successo, false altrimenti
	*/
	bool verifica(Parametri p);
	/**
	* \brief invia sul socket la richiesta di servizio
	*
	* \param sk descrittore del socket
	* \param input lista parametri inviati al server
	*
	* \return true in caso di successo, false altrimenti
	*/
	bool send_request(int sk, Parametri input);
	/**
	* \brief riceve sul socket la richiesta di servizio
	*
	* \param sk descrittore del socket
	* \param input lista che conterra' i parametri di richiesta
	*
	* \return 1 in caso di successo, -1 se errore generico, -3 se richiesta errata
	*/
	int receive_request(int sk, Parametri &input);
	/**
	* \brief funzione virtuale che esegue effettivamente il servizio. Dovra' essere ridefinita dalle 
	*	 classi che estendono Service
	*
	* \param sk descrittore del socket
	* \param id identificativo del thread che esegue il servizio
	*
	* \return -10
	*/
	int virtual do_service(int sk,int id);
};

/**
* \class Register
*
* \brief implementa il service_register 
*/
class Register {
	private:
	/// porta del service_register
	in_port_t port;
	/// indirizzo ip del service_register
	in_addr_t addr;
	
	public:
	/**
	* \brief costruttore
	*
	* \param p porta del service_register (in formato network)
	* \param a indirizzo ip del service_register
	*
	* \return true in caso di successo, false altrimenti
	*/
	Register(in_port_t p, in_addr_t a);
	/**
	* \brief imposta manualmente la porta del service_register
	*
	* \param p numero di porta del service_register
	*/
	void set_port(in_port_t p);
	/**
	* \brief imposta manualmente l'indirizzo ip del service_register
	*
	* \param a indirizzo ip del service_register
	*/
	void set_addr(in_addr_t a);
	/**
	* \brief recupera la porta del service_register
	*
	* \return porta del service_register
	*/
	in_port_t get_port();
	/**
	* \brief recupera l'indirizzo ip del service_register
	*
	* \return indirizzo ip del service_register
	*/
	in_addr_t get_addr();
	/**
	* \brief richiesta registrazione servizio al service_register
	*
	* \param dsc descrizione del servizio
	* \param p porta del server che implementa tale servizio (in formato network)
	* \param ad indirizzo ip del server che implementa tale servizio
	*
	* \return 1 in caso di successo, -1 se errore generico, -3 se errore connect
	*	  -4 se errore nell'apertura socket
	*/
	int rec_service(string dsc, in_port_t p, in_addr_t ad);
	/**
	* \brief richiesta deregistrazione servizio al service_register
	*
	* \param nome nome del servizio da deregistrare
	* \param p porta del server che implementa tale servizio (in formato network)
	* \param ad indirizzo ip del server che implementa tale servizio
	*
	* \return 1 in caso di successo, -1 se errore generico, -3 se errore connect
	*	  -4 se errore nell'apertura socket
	*/
	int derec_service(string nome, in_port_t p, in_addr_t ad);
	/**
	* \brief richiesta cancellazione server al service_register
	*
	* \param p porta del server che si vuole cancellare (in formato network)
	* \param ad indirizzo ip del server che si vuole cancellare
	*
	* \return 1 in caso di successo, -1 se errore generico, -3 se errore connect
	*	  -4 se errore nell'apertura socket
	*/
	int del_server(in_port_t p, in_addr_t ad);
	/**
	* \brief richiesta servizio al service_register
	*
	* \param nome nome del servizio richiesto
	* \param dsc conterra' la descrizione del servizio richiesto
	* \param p conterra' la porta del server che implementa tale servizio (in formato network)
	* \param a conterra' l'indirizzo ip del server che implementa tale servizio
	*
	* \return 1 in caso di successo, -1 se errore generico, -3 se errore connect
	*	  -4 se errore nell'apertura socket, -5 se servizio non esistente
	*/
	int request_service(string nome, string &dsc, in_port_t &p, in_addr_t &a);
};

/**
* \class Reader_Writer
*
* \brief implementa la politica di locking lettori/scrittori 
*/
class Reader_Writer {
	private:
	/// semaforo di mutua esclusione
	pthread_mutex_t mutex;
	/// condition degli scrittori
	pthread_cond_t scrittori;
	/// condition dei lettori
	pthread_cond_t lettori;
	/// numero scrittori sospesi
	int scrittori_bloccati;
	/// numero lettori sospesi
	int lettori_bloccati;
	/// numero lettori attivi
	int lettori_attivi;
	/// scrittore attivo?
	bool scrittore_attivo;

	public:
	/// costruttore di default
	Reader_Writer();
	/// verifica se si puo' effettuare una lettura, altrimenti si blocca sulla condition dei lettori
	void richiesta_lettura();
	/// verifica se si puo' effettuare una scrittura, altrimenti si blocca sulla condition degli scrittori
	void richiesta_scrittura();
	/// termina una lettura, se non ci sono altre letture attive si controlla se si puo' svegliare uno scrittore
	void fine_lettura();
	/// termina una scrittura, se ci letture in attesa si svegliano, altrimenti si controlla se si puo' svegliare
	/// uno scrittore
	void fine_scrittura();
};
