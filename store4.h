/*
 * store4.h
 *
 *  Created on: Nov 10, 2012
 *      Author: vspathak
 */

#ifndef STORE4_H_
#define STORE4_H_

typedef struct storeDef{
	int Cameras;
	int Laptops;
	int Printers;
}store;

#define TCPPORT "21533" // the port client will be connecting to
#define MY_UDP1PORT "17533"
#define MY_UDP2PORT "18533"
#define MY_UDP3PORT "19533"
#define MY_UDP4PORT "20533"


#define THEIR_UDP1PORT "14533"
#define THEIR_UDP2PORT "7533"
#define THEIR_UDP3PORT "16533"
#define THEIR_UDP4PORT "32533"

#endif /* STORE4_H_ */
