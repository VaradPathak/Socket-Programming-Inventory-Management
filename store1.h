/*
 * store1.h
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */

#ifndef STORE1_H_
#define STORE1_H_

typedef struct storeDef{
	int Cameras;
	int Laptops;
	int Printers;
}store;

#define TCPPORT "21533" // the port client will be connecting to
#define MY_UDP1PORT "5533"
#define MY_UDP2PORT "6533"
#define MY_UDP3PORT "7533"
#define MY_UDP4PORT "8533"

#define THEIR_UDP1PORT "31533"
#define THEIR_UDP2PORT "9533"
#define THEIR_UDP3PORT "18533"
#define THEIR_UDP4PORT "11533"

#endif /* STORE1_H_ */
