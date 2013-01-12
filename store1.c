/*
 * store1.c
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "store1.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once
/*
 * Gets sockaddr, IPv4 or IPv6:
 * Reused from beej's guide
 * Parameters: sockaddr
 * Return: void
 */
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

/*
 * Show appropriate errors
 * Parameters: errorcode
 * Return: void
 */
void Usage(int errorcode) {
	if (errorcode == 1)
		perror("File could not be opened.\n");
	if (errorcode == 2)
		perror("The input file content is incorrect\n");
	exit(1);
}

/*
 * Parse the input file and store the data in store objects
 * Parameters: fp i/p file pointer
 * 			   newStore o/p store object
 * Return: success or failure
 */
int parseFile(FILE* fp, store* newStore) {
	char buf[1025];
	while (fgets(buf, sizeof(buf), fp) != NULL ) {
		char *start_ptr = buf;
		if (strchr(start_ptr, '\n') != NULL ) {
			char *space_ptr = strchr(start_ptr, ' ');
			char *eol_ptr = strchr(start_ptr, '\n');
			*eol_ptr = '\0';
			if (space_ptr != NULL ) {
				*space_ptr++ = '\0';
				if (!strcmp(start_ptr, "Cameras")) {
					newStore->Cameras = strtol(space_ptr, (char **) NULL, 10);
				} else if (!strcmp(start_ptr, "Laptops")) {
					newStore->Laptops = strtol(space_ptr, (char **) NULL, 10);
				} else if (!strcmp(start_ptr, "Printers")) {
					newStore->Printers = strtol(space_ptr, (char **) NULL, 10);
				}
			} else {
				Usage(2);
			}
		} else {
			Usage(2);
		}
	}
	return 0;
}

/*
 * Gets the store data from the i/p file
 * Parameters: fileName, o/p store object pointer
 * Return: void
 */
void getStoreData(const char* fileName, store* newStore) {
	FILE* fp1 = fopen(fileName, "r");
	if (fp1 == NULL ) {
		Usage(1);
	}
	if (parseFile(fp1, newStore)) {
		Usage(2);
	}
	fclose(fp1);
}

/*
 * Loads/unloads the objects from the truck according to the requirements
 * Parameters: store_1, Truck objects
 * Return: void
 */
void handleRecievedTruck(store *myStore, store *truck) {
	if ((myStore->Cameras > 0) || ((-1 * myStore->Cameras) < truck->Cameras)) {
		truck->Cameras += myStore->Cameras;
		myStore->Cameras = 0;
	} else {
		myStore->Cameras += truck->Cameras;
		truck->Cameras = 0;
	}
	if ((myStore->Laptops > 0) || ((-1 * myStore->Laptops) < truck->Laptops)) {
		truck->Laptops += myStore->Laptops;
		myStore->Laptops = 0;
	} else {
		myStore->Laptops += truck->Laptops;
		truck->Laptops = 0;
	}
	if ((myStore->Printers > 0)
			|| ((-1 * myStore->Printers) < truck->Printers)) {
		truck->Printers += myStore->Printers;
		myStore->Printers = 0;
	} else {
		myStore->Printers += truck->Printers;
		truck->Printers = 0;
	}
}

/*
 * Create the TCP/UDP connections, get the data and process it.
 */
int main(int argc, char *argv[]) {
	store truck;
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p, *q, *myaddrinfo;
	struct sockaddr_storage their_addr;
	int rv;
	char s[INET6_ADDRSTRLEN];

	/********* TCP: Send the store data to the warehouse **************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo("nunki.usc.edu", TCPPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}
	if (p == NULL ) {
		perror("client: failed to connect\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure
	if (p->ai_family == PF_INET) {
		struct sockaddr_in my_addr;
		socklen_t addrlen = sizeof(my_addr);
		int getsock_check = getsockname(sockfd, (struct sockaddr *) &my_addr,
				&addrlen);
		//Error checking
		if (getsock_check == -1) {
			perror("getsockname");
			exit(1);
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) &my_addr), s,
				sizeof s);
		printf("Phase 1: store_1 has TCP port number %d and IP address %s \n",
				my_addr.sin_port, s);
	} else if (p->ai_family == PF_INET6) {
		struct sockaddr_in6 my_addr;
		socklen_t addrlen = sizeof(my_addr);
		int getsock_check = getsockname(sockfd, (struct sockaddr *) &my_addr,
				&addrlen);
		//Error checking
		if (getsock_check == -1) {
			perror("getsockname");
			exit(1);
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) &my_addr), s,
				sizeof s);
		printf("Phase 1: store_1 has TCP port number %d and IP address %s \n",
				my_addr.sin6_port, s);
	}

	store newStore;
	getStoreData("Store-1.txt", &newStore);
	if ((numbytes = send(sockfd, (void *) &newStore, sizeof(store), 0)) == -1) {
		perror("recv");
		exit(1);
	}

	printf(
			"Phase 1: The outlet vector <%d,%d,%d> for Store_1 has been sent to the central warehouse\n",
			newStore.Cameras, newStore.Laptops, newStore.Printers);
	printf("End of Phase 1 for Store_1\n");

	/*********** UDP-1: Get the Truck from the warehouse *************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP1PORT, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	if (p == NULL ) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s,
			sizeof s);
	printf("\nPhase 2: Store_1 has UDP port %d and IP address %s \n",
			atoi(MY_UDP1PORT), s);
	freeaddrinfo(servinfo);

	socklen_t addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, &truck, sizeof(store), 0,
			(struct sockaddr *) &their_addr, &addr_len)) == -1) {
		perror("recvfrom");
	}
	printf(
			"Phase 2: Store_1 received the truck-vector<%d,%d,%d> from the central warehouse.\n",
			truck.Cameras, truck.Laptops, truck.Printers);

	close(sockfd);

	/******************* Process the received truck object **********/
	handleRecievedTruck(&newStore, &truck);

	/********* UDP-2: Forward the Truck to store_3 *****************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", THEIR_UDP2PORT, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL ) {
		perror("talker: failed to bind socket\n");
		return 2;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP2PORT, &hints, &myaddrinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	for (q = myaddrinfo; q != NULL ; q = q->ai_next) {
		if (bind(sockfd, q->ai_addr, q->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	if (q == NULL ) {
		perror("listener: failed to bind socket\n");
		return 2;
	}
	inet_ntop(q->ai_family, get_in_addr((struct sockaddr *) q->ai_addr), s,
			sizeof s);
	printf("\nPhase 2: Store_1 has UDP port %d and IP address %s \n",
			atoi(MY_UDP2PORT), s);

	usleep(1000);
	if ((numbytes = sendto(sockfd, (void *) &truck, sizeof(store), 0,
			p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf(
			"Phase 2: The updated truck-vector<%d,%d,%d> has been sent to store_2\n",
			truck.Cameras, truck.Laptops, truck.Printers);
	printf("Phase 2: Store_1 updated outlet-vector is <%d,%d,%d>\n\n",
			newStore.Cameras, newStore.Laptops, newStore.Printers);
	freeaddrinfo(servinfo);
	close(sockfd);

	/********** UDP-3: Get the Truck from Store_1 ***********/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP3PORT, &hints, &myaddrinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for (q = myaddrinfo; q != NULL ; q = q->ai_next) {
		if ((sockfd = socket(q->ai_family, q->ai_socktype, q->ai_protocol))
				== -1) {
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, q->ai_addr, q->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	if (p == NULL ) {
		perror("listener: failed to bind socket\n");
		return 2;
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s,
			sizeof s);
	printf("\nPhase 2: Store_1 has UDP port %d and IP address %s \n",
			atoi(MY_UDP3PORT), s);

	freeaddrinfo(servinfo);

	addr_len = sizeof p->ai_addr;
	if ((numbytes = recvfrom(sockfd, &truck, sizeof(store), 0,
			(struct sockaddr *) p->ai_addr, &addr_len)) == -1) {
		perror("recvfrom");
	}
	close(sockfd);
	printf("Phase 2: truck-vector <%d,%d,%d> has been received from Store_4\n",
			truck.Cameras, truck.Laptops, truck.Printers);

	/******************* Process the received truck object **********/
	handleRecievedTruck(&newStore, &truck);

	/*********** UDP-4: Forward the Truck to store_3 *****************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", THEIR_UDP4PORT, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and make a socket
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL ) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP4PORT, &hints, &myaddrinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	for (q = myaddrinfo; q != NULL ; q = q->ai_next) {
		if (bind(sockfd, q->ai_addr, q->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	if (q == NULL ) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}
	inet_ntop(q->ai_family, get_in_addr((struct sockaddr *) q->ai_addr), s,
			sizeof s);
	printf("\nPhase 2: Store_1 has UDP port %d and IP address %s \n",
			atoi(MY_UDP4PORT), s);

	usleep(1000);
	if ((numbytes = sendto(sockfd, (void *) &truck, sizeof(store), 0,
			p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	freeaddrinfo(servinfo);
	printf(
			"Phase 2: The updated truck-vector<%d,%d,%d> has been sent to store_2\n",
			truck.Cameras, truck.Laptops, truck.Printers);
	printf("Phase 2: Store_1 updated outlet vector is <%d,%d,%d>\n\n",
			newStore.Cameras, newStore.Laptops, newStore.Printers);
	printf("End of Phase 2 for Store_1\n");

	close(sockfd);
	return 0;
}
