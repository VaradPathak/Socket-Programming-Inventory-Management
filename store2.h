/*
 * store2.h
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */

#ifndef STORE2_H_
#define STORE2_H_

typedef struct storeDef{
	int Cameras;
	int Laptops;
	int Printers;
}store;

#define TCPPORT "21533" // the port client will be connecting to
#define MY_UDP1PORT "9533"
#define MY_UDP2PORT "10533"
#define MY_UDP3PORT "11533"
#define MY_UDP4PORT "12533"


#define THEIR_UDP1PORT "6533"
#define THEIR_UDP2PORT "13533"
#define THEIR_UDP3PORT "8533"
#define THEIR_UDP4PORT "15533"

#endif /* STORE2_H_ */
