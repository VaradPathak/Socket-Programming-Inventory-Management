Socket-Programming-Inventory-Management
=======================================

The application for inventory management. Uses TCP and UDP sockets.

- In this project I have written programs of a warehouse and 4 stores which exchange messages using TCP/UDP connection to simulate 
   the inventory management system. I have used C-programming language for the developement.
   
- Following is the list of code files and their corresponding functionality.
  - warehouse.c - The program to simulate the warehouse in the project. This acts as a server in TCP connection of phase 1 of project. In phase 2 this code calculates the truck object and sends the same to the store_1. This code also calculates a remainder truck object and verifies the end condition of the project and prints "Operation failed" if the o/p is not correct. 
  - warehouse.h - This file is used to map the PORT numbers used in different connections and constants such as num of stores. This file also has the definition of store type which is a struct.
  - store1.c, store2.c, store3.c, store4.c - These files have code to simulate the stores in the project and they one by one connect to the warehouse by using TCP connection to share the information anout their status. In phase 2 these stores exchange  the truck object using UDP connection. 
  - store1.h, store2.h, store3.h, store4.h - These files are used to map the TCP & UDP port numbers used in different connections. These files also have the definition of store type which is a struct.
  - Makefile -    This file has the code to make and clean all the required executables.
	   
- Steps to execute the program:
	- Go to the project directory and copy all the required text files in this directory (Store-1.txt, Store-2.txt, etc)
	- Run "make" command to compile the code and create the executables
	- Run the program for warehouse, store1, store2, store3, store4 in the given sequence in different terminals to nunki
	- Verify the o/p in each terminal window
	- Run "make clean" command to remove all the .o and executable files created by the "make" command.
	 
- I am using store type (a struct defined in header files) of object to exchange information. The printed messages are according to specifications.  

- Reused Code: I have reused code given in beej's guide to create TCP and UDP connections. Also, parseFile function uses some of the ideas taught in CSCI-402 (fall-2012) class by Prof. Bill Cheng.   
