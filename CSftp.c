/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "dir.h"
#include "usage.h"
#include <pthread.h>
#include <ctype.h>

#define MAXDATASIZE 256

#define BACKLOG 10 // how many pending connections queue will hold

int sockfd, new_fd; // listen on sock_fd, new connection on new_fd

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

typedef enum
{
	INVALID = -1,
	USER,
	RETR,
	QUIT,
	CWD,
	CDUP,
	TYPE,
	MODE,
	STRU,
	PASV,
	NLST
} FTP_CMD;

typedef struct
{
	FTP_CMD cmd;
	char *args;
} ftp_cmd;

ftp_cmd parse_cmd(char *buf);
int pasv();

int login(char *args)
{
	printf("In login\n");
}

int changeDirectory(char *args) {
	printf("Inside change directory\n");
	// todo: check if first character is . (./)
	// todo: check if second character is .. (../)
	// todo: check if the path includes ../
	// 

	if (strstr(args, "../") != NULL) {
		// todo bad path
	} else if (strstr(args, "./") != NULL) {
		// todo: bad path
	} else {
		chdir(args);
		printf("In else statement\n");
	}


}

int main(int argc, char **argv)
{
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char buf[MAXDATASIZE];
	int isLoggedIn = 0;

	char *PORT = argv[1];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	// sa.sa_handler = sigchld_handler; // reap all dead processes
	// sigemptyset(&sa.sa_mask);
	// sa.sa_flags = SA_RESTART;
	// if (sigaction(SIGCHLD, &sa, NULL) == -1)
	// {
	// 	perror("sigaction");
	// 	exit(1);
	// }

	printf("server: waiting for connections...\n");



	while (1)
	{ // main accept() loop
		printf("just entered while loop\n");
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr),
				  s, sizeof s);
		printf("server: got connection from %s\n", s);

		// * Responds with 220 to initiate login sequence
		bzero(buf, MAXDATASIZE);
		strcpy(buf, "220\n");
		send(new_fd, buf, sizeof(buf), 0);

		//
		//
		//
		while (1)
		{
			bzero(buf, MAXDATASIZE);
			// printf("in second while loop\n");
			char buf[MAXDATASIZE];
			if (recv(new_fd, buf, MAXDATASIZE - 1, 0) == -1)
			{
				perror("error reading from socket");
				close(new_fd);
				break;
			}
			printf("Buffer is: %s\n", buf);
			ftp_cmd cmd = parse_cmd(buf);

			printf("COMMAND: %d\n", cmd.cmd);
			printf("ARGS: %s\n", cmd.args);
			printf("ARGS: %d\n", isLoggedIn);
			

			// * if trying to login with username other than cs317
			if (cmd.cmd == USER && strcmp(cmd.args, "cs317") != 0)
			{
				printf("Inside bad username\n");
				bzero(buf, MAXDATASIZE);
				strcpy(buf, "530: Not Logged In\n");
				send(new_fd, buf, sizeof(buf), 0);
				// printf("Inside first if statement\n");

				// * if trying to login with cs317
			}
			else if (cmd.cmd == USER && strcmp(cmd.args, "cs317") == 0)
			{
				
				isLoggedIn = 1;
				printf("Logged In\n");

				// if user is already logged in
				// ! ASSUMING USER HAS TO BE LOGGED IN TO QUIT
			}
			else if (isLoggedIn == 1)
			{
				printf("[+] Switch Statement\n");
				printf("Command is: %d\n", cmd.cmd);
				switch (cmd.cmd)
				{
				case USER:
					// login(cmd.args);
					// TODO do nothing ?? since we already check login earlier
					break;
				case RETR:
					break;
				case QUIT: //
					break;
				case CWD: //
					printf("In CWD\n");
					changeDirectory(cmd.args);
					break;
				case CDUP: //
					break;
				case TYPE: //
					break;
				case MODE: //
					break;
				case STRU: //
					break;
				case PASV:
					printf("In PASV\n");
					pasv();
					break;
				case NLST:
					break;
				case INVALID:
					break;

					// default:
					/**
					 * Return 500 response
					 */
				}
			} else {
				bzero(buf, MAXDATASIZE);
				strcpy(buf, "530: Not Logged In\n");
				send(new_fd, buf, sizeof(buf), 0);
				printf("Not Logged In");
			  }
		}
		//
		//
		//

		close(sockfd); // child doesn't need the listener
		if (send(new_fd, "Hello, world!", 13, 0) == -1)
			perror("send");
		close(new_fd);
		exit(0);
	}

	return 0;
}

ftp_cmd parse_cmd(char *buf)
{
	char *cmd;
	char *arg;
	ftp_cmd cmd1;
	int cmdInt;

	cmd = strtok(buf, " \r\n");

// 	char * cmdUp = cmd;
// 	while (*cmdUp) {
//     *cmdUp = toupper((unsigned char) *cmdUp);
//     cmdUp++;
//   }

	// printf("CMDUP is: %s\n", cmdUp);
	printf("Command is: %s\n", cmd);
	arg = strtok(NULL, " \r\n");
	printf("Arg is: %s\n", arg);
	
	

	if (strcasecmp(cmd, "USER") == 0) 
		cmdInt = 0;
	else if (strcasecmp(cmd, "RETR") == 0)
		cmdInt = 1;
	else if (strcasecmp(cmd, "QUIT") == 0)
		cmdInt = 2;
	else if (strcasecmp(cmd, "CWD") == 0)
		cmdInt = 3;
	else if (strcasecmp(cmd, "CDUP") == 0)
		cmdInt = 4;
	else if (strcasecmp(cmd, "TYPE") == 0)
		cmdInt = 5;
	else if (strcasecmp(cmd, "MODE") == 0)
		cmdInt = 6;
	else if (strcasecmp(cmd, "STRU") == 0)
		cmdInt = 7;
	else if (strcasecmp(cmd, "PASV") == 0)
		cmdInt = 8;
	else if (strcasecmp(cmd, "NLST") == 0)
		cmdInt = 9;
	
	cmd1.cmd = cmdInt;
	cmd1.args = arg;

	return cmd1;
}

int pasv() {
	int pasvfd, new_pasvfd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage pasv_their_addr;
	socklen_t pasv_sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char buf[MAXDATASIZE];
	int isLoggedIn = 0;
	char pasv_msg[MAXDATASIZE];

	char hostbuffer[MAXDATASIZE];
    char *pasv_addr;
    struct hostent *host_entry;
    int hostname;

	struct sockaddr_in sin;

	char *PORT = "0";

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP


	printf("initial port: %s\n", PORT);

	// using a random PORT number (0)
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((pasvfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(pasvfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(pasvfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(pasvfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	socklen_t sin_len = sizeof(sin); 
	if (getsockname(pasvfd, (struct sockaddr *)&sin, &sin_len) == -1)
	{
    	perror("getsockname");
		exit(1);
	}

	int pasv_port = (int) ntohs(sin.sin_port);
	printf("PASV on port port: %d\n", pasv_port);
  
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    host_entry = gethostbyname(hostbuffer);
    pasv_addr = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));
  
	printf("PASV on IP address: %s\n", pasv_addr);

	// getting the IP address as an array
  	unsigned char init_adr_array[4] = {0};
    size_t index = 0;

    char *temp = pasv_addr; /* save the pointer */
    while (*temp) {
        if (isdigit((unsigned char)*temp)) {
            init_adr_array[index] *= 10;
            init_adr_array[index] += *temp - '0';
        } else {
            index++;
        }
        temp++;
    }

    int ip1 = init_adr_array[0];
    int ip2 = init_adr_array[1];
    int ip3 = init_adr_array[2];
    int ip4 = init_adr_array[3];

	int port_p1 = pasv_port / 256;
	int port_p2 = pasv_port % 256;


	// send client entering passive mode message
	bzero(pasv_msg, MAXDATASIZE);
	snprintf(pasv_msg, MAXDATASIZE, "227 Entering passive mode (%d,%d,%d,%d,%d,%d).\n", 
		ip1, ip2, ip3, ip4, port_p1, port_p2);
	send(new_fd, pasv_msg, sizeof(pasv_msg), 0);

	char port_msg[MAXDATASIZE];
	bzero(port_msg, MAXDATASIZE);
	snprintf(port_msg, MAXDATASIZE, "Connect to Data client on %s:%d", pasv_addr, pasv_port);
	send(new_fd, port_msg, sizeof(port_msg), 0);

	// timeout stuff
	fd_set fdset;
	fd_set *rfds, *wfds;
	struct timeval timeout;
	int timeout_state;

	FD_ZERO (&fdset);
    FD_SET  (pasvfd, &fdset);
    timeout.tv_sec = 20;
    timeout.tv_usec = 0;

	// listen on the passive socket
	if (listen(pasvfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	while(1)
	{
		pasv_sin_size = sizeof pasv_their_addr;
		printf("before accept\n");
		new_pasvfd = accept(pasvfd, (struct sockaddr *)&pasv_their_addr, &pasv_sin_size);
		printf("after accept\n");
		inet_ntop(pasv_their_addr.ss_family,
				get_in_addr((struct sockaddr *)&pasv_their_addr),
				s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (new_pasvfd == -1)
		{
			perror("accept");
		}
		printf("Passive mode entered on IP address: %s, port: %d\n", pasv_addr, pasv_port);
		if (!timeout_state) {
			break;
		}
	}
	
	return 0;
}