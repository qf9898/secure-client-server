/**************************************************************************
Copyright:
File Name: client.c
Author:    Qiang Fan
Date:2021-04-29
Version: V1.0
Description: This file to realize the functions 
of a server encrpted by ssl.				
***************************************************************************/

#include "server.h"

// @Description: create the socket at the server and listen to the client requests
// @param: port#
// @return: socket
// @birth:
int OpenListener(int port)
{
    int sock;
    struct sockaddr_in addr; /*creating the sockets*/
    sock = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr)); /*free output the garbage space in memory*/
    addr.sin_family = AF_INET; /*getting ip address form machine */
    addr.sin_port = htons(port); /* converting host bit to n/w bit */
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) /* assiging the ip address and port*/
    {
        perror("can't bind port"); /* reporting error using errno.h library */
        abort();                   /*if error will be there then abort the process */
    }
    if (listen(sock, 10) != 0) /*for listening to max of 10 clients in the queue*/
    {
        perror("Can't configure listening port"); /* reporting error using errno.h library */
        abort(); /*if erroor will be there then abort the process */
    }
    return sock;
}

// @Description: creating and setting up ssl context structure
// @param: void
// @return: ssl context structure
// @birth:
SSL_CTX *InitServerCtx(void)
{ 
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms(); /* load & register all cryptos, etc. */
    SSL_load_error_strings(); /* load all error messages */
    method = (SSL_METHOD *) TLSv1_2_server_method(); /* create new server-method instance */
    ctx = SSL_CTX_new(method); /* create new context from method */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

// @Description: to load a certificate into an SSL_CTX structure
// @param: ctx: ssl context structure; cert_file: certificate file; key_file: key file
// @return: void
// @birth:
void LoadCert(SSL_CTX *ctx, char *cert_file, char *key_file)
{   
    /* set the local certificate from cert_file */
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from key_file (may be the same as cert_file) */
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

// @Description: Show the ceritficates to client and match them;
// @param: ssl: ssl connection
// @return: void
// @birth:
void PrintCert(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Server: %s\n", line); /*server certifcates*/
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("client: %s\n", line); /*client certificates*/
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

// @Description: The server sends message or receive message via the ssl connection
// @param: ssl: ssl connection
// @return: void
// @birth:
void SendReceiveData(SSL *ssl)
{
    char buf[1024];
    int connected_sock, bytes;
    char input[BUFFER];
    pid_t cpid;
    if (SSL_accept(ssl) == FAIL) /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        /*Fork system call is used to create a new process*/
        cpid = fork();
        if (cpid == 0)
        {
            while (1)
            {
                bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request and read message from server*/
                if (bytes > 0)
                {
                    buf[bytes] = 0;
                    printf("\nMESSAGE FROM CLIENT:%s\n", buf);
                }
                else
                    ERR_print_errors_fp(stderr);
            }
        }
        else
        {
            while (1)
            {
                printf("\nMESSAGE TO CLIENT:");
                fgets(input, BUFFER, stdin); /* get request and reply to client*/
                SSL_write(ssl, input, strlen(input));
            }
        }
    }
    connected_sock = SSL_get_fd(ssl); /* get socket connection */
    SSL_free(ssl); /* release SSL state */
    close(connected_sock); /* close connection */
}

// @Description: The main function of the server
// @param: count; *strings[]: port # of server
// @return: int
// @birth:
int main(int count, char *strings[]) 
{
    SSL_CTX *ctx;
    int listen_sock;
    char *portnum;
    if (count != 2)
    {
        printf("Usage: %s \n", strings[0]); /*send the usage guide if syntax of setting port is different*/
        exit(0);
    }
    SSL_library_init(); /*load encryption and hash algo's in ssl*/
    portnum = strings[1];
    ctx = InitServerCtx(); /* initialize ssl context */
    LoadCert(ctx, "certificate.pem", "certificate.pem"); /* load certicate of the server to the ssl context */
    listen_sock= OpenListener(atoi(portnum)); /* create listen socket for server */
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    SSL *ssl;
    listen(listen_sock, 5); /*setting 5 clients at a time to queue*/
    int connected_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &len); /* get the current connected socket towards a client */
    printf("Connection: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); /*printing connected client information*/
    ssl = SSL_new(ctx); /* create SSL connection based on certificate of the server */
    SSL_set_fd(ssl, connected_sock); /* attach the ssl connection to the client socket */
    SendReceiveData(ssl); /* Send or receive messges */
    close(listen_sock); /* close server's listen socket */
    SSL_CTX_free(ctx); /* release ssl context */
    return 0;
}
