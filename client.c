/*********************************************************************
Copyright:
File Name: client.c
Author:    Qiang Fan
Date:2021-04-29
Version: V1.0
Description: This file to realize the functions 
of a server encrpted by ssl.				
**********************************************************************/

#include "client.h"

// @func description: crate the socket to connect to the destination server
// @param: hostname: IP of server; port: port# of server
// @return: socket
// @birth:
int OpenConnection(const char *hostname, int port)
{
    int sock;
    struct hostent *host;
    struct sockaddr_in addr; /* addr struct with ip addr and port#*/
    if ((host = gethostbyname(hostname)) == NULL)
    {
        perror(hostname);
        abort();
    }
    sock = socket(PF_INET, SOCK_STREAM, 0); /* create a socket */
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)(host->h_addr);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) /*open connection, bind socket and addr */
    {
        close(sock);
        perror(hostname);
        abort();
    }
    return sock;
}

// @Description: creating and setting up ssl context structure
// @param: void
// @return: ctx:ssl context;
// @birth:
SSL_CTX *InitSslCtx(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms(); /* Load crypt */
    SSL_load_error_strings(); /* Bring in and register error messages */
    method = (SSL_METHOD *) TLSv1_2_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method); /* Create ssl context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

// @Description: Show the ceritficates to server and match them
// but here we are not using any client certificate
// @param:  ssl: ssl connection;
// @return: void
// @birth:
void PrintCert(SSL *ssl) 
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line); /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

// @Description: the main function to crate client and realize the secure communications
// @param: count: # of params  ; *strings[]: IP addr (127.0.0.1) and port number of server;
// @return: int;
// @birth:
int main(int count, char *strings[]) 
{
    SSL_CTX *ctx;
    int sock;
    SSL *ssl;
    char buf[1024];
    char input[BUFFER];
    int bytes;
    char *server_ip, *portnum;
    pid_t cpid; /* fork variable*/
    if (count != 3)
    {
        printf("usage: %s  \n", strings[0]);
        exit(0);
    }
    SSL_library_init(); /*load encryption in ssl*/
    server_ip = strings[1];
    portnum = strings[2];
    ctx = InitSslCtx();
    sock = OpenConnection(server_ip, atoi(portnum)); /* open the socket and connect to the detination server */
    ssl = SSL_new(ctx); /* create new SSL connection state */
    SSL_set_fd(ssl, sock); /* attach the socket descriptor */
    if (SSL_connect(ssl) == FAIL) /* perform connection, and the client gets the certificate from the server */
        ERR_print_errors_fp(stderr);
    else
    {
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        PrintCert(ssl);
        /* client starts to write(send) message, or read(receive) message */
        cpid = fork();
        /*Fork system call is used to create a new process*/
        if (cpid == 0)
        {
            while (1)
            {
                printf("\nMESSAGE TO SERVER:");
                fgets(input, BUFFER, stdin);
                SSL_write(ssl, input, strlen(input)); /* encrypt & send message */
            }
        }
        else
        {
            while (1)
            {
                bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
                if (bytes > 0)
                {
                    buf[bytes] = 0;
                    printf("\nMESSAGE FROM SERVER: %s\n", buf);
                }
            }
        }
        SSL_free(ssl); /* release ssl connection */
    }
    close(sock); /* close socket */
    SSL_CTX_free(ctx); /* release context */
    return 0;
}
