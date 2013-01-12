/*
 * store3.h
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */

#ifndef STORE3_H_
#define STORE3_H_

typedef struct storeDef{
	int Cameras;
	int Laptops;
	int Printers;
}store;

#define TCPPORT "21533" // the port client will be connecting to
#define MY_UDP1PORT "13533"
#define MY_UDP2PORT "14533"
#define MY_UDP3PORT "15533"
#define MY_UDP4PORT "16533"


#define THEIR_UDP1PORT "10533"
#define THEIR_UDP2PORT "17533"
#define THEIR_UDP3PORT "12533"
#define THEIR_UDP4PORT "19533"

#endif /* STORE3_H_ */
