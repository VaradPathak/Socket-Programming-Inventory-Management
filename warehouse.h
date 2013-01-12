/*
 * warehouse.h
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */

#ifndef WAREHOUSE_H_
#define WAREHOUSE_H_

typedef struct storeDef {
	int Cameras;
	int Laptops;
	int Printers;
} store;

#define STORECOUNT 4
#define TCPPORT "21533" // the port users will be connecting to

#define MY_UDP1PORT "31533"
#define MY_UDP4PORT "32533"

#define THEIR_UDP1PORT "5533"
#define THEIR_UDP4PORT "20533"

#define BACKLOG 10 // how many pending connections queue will hold
#endif /* WAREHOUSE_H_ */
