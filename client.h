#ifndef __CLIENT_H_H1__
# define __CLIENT_H_H1__


#include <errno.h> /*using the error library for finding errors */ 
#include <stdio.h> /*standard i/o*/

#include <unistd.h> /*for using FORK for sending and receiving messages for one time*/ 

#include <malloc.h> /*for memory allocation */

#include <string.h> /*using fgets funtions for geting input from user*/

#include <sys/socket.h> /*for creating sockets*/

#include <resolv.h> /*server to find out the runner's IP address*/ 

#include <netdb.h> /*definitions for network database operations */

#include <openssl/ssl.h> /*using openssl function's and certificates and configuring them*/

#include <openssl/err.h> /* helps in finding out openssl errors*/

#include <unistd.h>  /*for using FORK for sending and receiving messages for one time*/ 

#define FAIL    -1 /*for error output == -1 */

#define BUFFER  1024  /*buffer for reading messages*/

int OpenConnection(const char *hostname, int port);   
SSL_CTX* InitSslCtx(void);   	/*creating and setting up ssl context structure*/
void PrintCert(SSL* ssl);	/*show the ceritficates to server and match them but here we are not using any client certificate*/
int main(int count, char *strings[]);  /* getting port and ip as an argument*/


# endif
