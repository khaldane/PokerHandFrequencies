#if !defined(__POKERHANDSMPI_H__)
#define __POKERHANDSMPI_H__
/*
* Program: PokerHandFrequenciesParallel
* Module: PokerHandsMPI.h
* Author: Katherine Haldane & Jared Lerner
* Date: October 10, 2014
* Description: Poker Simluation Parallel Program  that simulates drawing 5-card
*      poker hands from a standard deck of 52 cards. Using MPI and 1 or more processors, the program will count
*      the number of frequencies for each of the ten hand-types until all have been drawn,
*      Simulation results are then output to the console.
*/

class PokerHandsMPI {
public:

	//Constructor/Deconstructor
	PokerHandsMPI();
	~PokerHandsMPI();

	int deck[52];
	int hand[5];

	std::map<std::string, int> frequencyMap;
	std::map<std::string, int>::iterator it;


	//Public Methods
	void report(int numProcs);
	void processMaster(int numProcs);
	void processSlave(int rank);


private:
	// Private static methods
	static void _seedRNG(int rank);
	static int getRandom(int rank);
	static void printHand(int hand[5]);
	static void printNumber(int num);

	void setUpDeck();
	void randomizeDeck(int arr[], int n, int rank = 1);

	void processSerial();

	bool terminateSlaves(int numProcs);
	void checkMessagesFromSlaves(int& matchesLeft, int& activeCount);
	void checkMessageFromMaster(int& activeCount);

	bool checkForMatch(std::string msg);
	void tallyResult(int result, int procID, int& activeCount);

	//Member Variables
	double duration;
	std::clock_t start;

	static bool _seeded;

};
#endif;