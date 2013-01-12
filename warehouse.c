/*
 * warehouse.c
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

#include "warehouse.h"

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
 * Creates a truck object using the inputs from the stores
 * Parameter: truck, storeList[], remainderTruck
 * Return: void
 */
void getTruckVector(store* truck, store* storeList[STORECOUNT],
		store* remainderTruck) {
	int storeCount;
	truck->Cameras = 0;
	truck->Laptops = 0;
	truck->Printers = 0;
	for (storeCount = 0; storeCount < STORECOUNT; ++storeCount) {
		truck->Cameras = truck->Cameras + storeList[storeCount]->Cameras;
		truck->Laptops = truck->Laptops + storeList[storeCount]->Laptops;
		truck->Printers = truck->Printers + storeList[storeCount]->Printers;
	}
	if (truck->Cameras > 0) {
		remainderTruck->Cameras = -1 * truck->Cameras;
		truck->Cameras = 0;
	} else {
		remainderTruck->Cameras = 0;
		truck->Cameras = -1 * truck->Cameras;
	}
	if (truck->Laptops > 0) {
		remainderTruck->Laptops = -1 * truck->Laptops;
		truck->Laptops = 0;
	} else {
		remainderTruck->Laptops = 0;
		truck->Laptops = -1 * truck->Laptops;
	}
	if (truck->Printers > 0) {
		remainderTruck->Printers = -1 * truck->Printers;
		truck->Printers = 0;
	} else {
		remainderTruck->Printers = 0;
		truck->Printers = -1 * truck->Printers;
	}
}

int main(int argc, char* argv[]) {

	int storeCount;
	int sockfd; // listen on sock_fd
	struct addrinfo hints, *servinfo, *p, *q, *myaddrinfo;
	struct sockaddr_storage their_addr; // connector's address information
	int yes = 1;
	int rv;
	char s[INET6_ADDRSTRLEN];
	int new_fd;
	socklen_t sin_size;

	store truck;
	store remainderTruck;
	store *storeList[STORECOUNT]; // Store list to store the replies from the stores
	for (storeCount = 0; storeCount < STORECOUNT; ++storeCount) {
		storeList[storeCount] = (store *) malloc(sizeof(store));
	}

	/*TCP Connection to collect the data from the stores*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo("nunki.usc.edu", TCPPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL ) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s,
			sizeof s);
	printf(
			"Phase 1: The central warehouse has TCP port number %d and IP address %s \n",
			atoi(TCPPORT), s);
	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	/*Iterate through the store list to collect the data*/
	for (storeCount = 0; storeCount < STORECOUNT; storeCount++) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);

		if (recv(new_fd, storeList[storeCount], sizeof(store), 0) == -1)
			perror("send");
		printf(
				"Phase 1: The central warehouse received information from store_%d\n",
				storeCount + 1);

		close(new_fd);
	}
	printf("End of Phase 1 for the central warehouse\n");

	/*Calculate the truck object using the values sent by stores*/
	getTruckVector(&truck, storeList, &remainderTruck);

	/***** UDP_1: Send the Truck to first store ************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", THEIR_UDP1PORT, &hints, &servinfo))
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
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP1PORT, &hints, &myaddrinfo))
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
	printf(
			"\nPhase 2: The central warehouse has UDP port number %d and IP address %s \n",
			atoi(MY_UDP1PORT), s);

	printf(
			"Phase 2: Sending the truck-vector to outlet store store_1. The truck vector value is <%d,%d,%d>.\n",
			truck.Cameras, truck.Laptops, truck.Printers);

	int numbytes;
	if ((numbytes = sendto(sockfd, (void *) &truck, sizeof(store), 0,
			p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	freeaddrinfo(servinfo);

	/***** UDP_2: Get the final Truck object from the last store *************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", MY_UDP4PORT, &hints, &servinfo)) != 0) {
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
	freeaddrinfo(servinfo);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", THEIR_UDP4PORT, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL ; p = p->ai_next) {
		break;
	}
	if (p == NULL ) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s,
			sizeof s);
	printf(
			"\nPhase 2: The central warehouse has UDP port number %d and IP address %s\n",
			atoi(MY_UDP4PORT), s);

	socklen_t addr_len = sizeof p->ai_addr;
	if ((numbytes = recvfrom(sockfd, &truck, sizeof(store), 0,
			(struct sockaddr *) p->ai_addr, &addr_len)) == -1) {
		perror("recvfrom");
	}
	printf(
			"Phase 2: The final truck-vector is received from the outlet store store_4, the truck-vector value is: <%d,%d,%d>\n",
			truck.Cameras, truck.Laptops, truck.Printers);

	close(sockfd);
	printf("End of Phase 2 for the central warehouse\n");

	/*Verification of o/p using the final status of the Truck*/
	if (!((truck.Cameras + remainderTruck.Cameras)
			&& (truck.Laptops + remainderTruck.Laptops)
			&& (truck.Printers + remainderTruck.Printers))) {
		/*printf("Operation Successful!!\n");*/
	} else {
		perror("Operation Failed");
	}

	for (storeCount = 0; storeCount < STORECOUNT; ++storeCount) {
		free(storeList[storeCount]);
	}

	return 0;
}
