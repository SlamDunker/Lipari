/**
* \file server2.h 
*
* \brief libreria che mette a disposizione funzionalita' utilizzabili dal service provider per la modifica (flip o rotate) delle *        immagini
*/
#include "libreria.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <netdb.h>
#include <sstream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#define cimg_display 0

#include "CImg.h"
using namespace cimg_library;

/**
* \class Rotate 
*
* \brief eredita Service ed implementa il servizio Rotate Image
*/
class Rotate : public Service{
	public:
	/** 
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	Rotate(string n,Parametri i,Parametri o);
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
* \class Flip 
*
* \brief eredita Service ed implementa il servizio Flip Horizontal Image
*/
class Flip : public Service{
	public:
	/** 
	* \brief costruttore
	*
	* \param n nome del servizio
	* \param i lista parametri input
	* \param o lista parametri output
	*/
	Flip(string n,Parametri i,Parametri o);
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
