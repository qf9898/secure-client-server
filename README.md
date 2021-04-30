Project: ssl secure tunnel for client-server

About:
This is a ssl based client-server chat. The whole project is based on C language. 
The openssl library is employed to generate the certificate and realize the ssl secure tunnel.
The project is developed based on Linux OS (Ubuntu)
 
Build:
The code is compiled on Ubuntu OS.
As the project utilizes openssl lib, the lib needs to be installed in advance.

### install openssl library 
$ sudo apt-get install libssl-dev
(if doesn't work, plase also try $ sudo apt install libssl-dev)
### build the codes
1. save the package to your favorable directory (project directory) 
2. Open a terminal and reach project directory: 
   $ cd project-directory

   Create a certificate file with OpenSSL lib: (optional, the certificate.pem is already attached in the package) 
   $ openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout certificate.pem -out certificate.pem

3. Compile the ssl-server and ssl-client (Note that a Makefile is given):
   $ make all

4. Open two terminals A and B, and reach the project directory ($ cd project-directory)
   For termal A (ssl-server):
   $ ./ssl-server port#       (e.g. $ ./ssl-server 4080)
   For Termal B (ssl-client):
   $ ./ssl-client 127.0.0.1 port#   (e.g., $ ./ssl-client 127.0.0.1 4080)
   At this time, client and server can text with each other via ssl tunnel.

5. At last, to release the project, executable files (i.e., ssl-server and ssl-client) can be removed to save package size:
   $ make clean 
