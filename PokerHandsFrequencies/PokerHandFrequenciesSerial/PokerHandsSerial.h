#if !defined(__POKERHANDSSERIAL_H__)
#define __POKERHANDSSERIAL_H__
/*
* Program: PokerHandFrequenciesSerial
* Module: PokerHandsSerial.h
* Author: Katherine Haldane & Jared Lerner
* Date: October 10, 2014
* Description Poker Simluation Program Serial that simulates drawing 5-card
*      poker hands from a standard deck of 52 cards. The program will count
*      the number of frequencies for each of the ten hand-types.
*/

#include <map>

class PokerHandsSerial {
public:

	//Constructor/Deconstructor
	PokerHandsSerial();
	~PokerHandsSerial();

	//Public Method	
	void drawHands();	
	void report();

	// Public static method
	static int getRandom(void);
	static void printHand(int hand[5]);
	static void printNumber(int num);

private:
	//Private Functions
	void setUpDeck();
	void randomizeDeck(int arr[], int n);
	void countFrequencies();

	//Member Variables
	int cardIdx;
	int deck[52];
	int hand[5];
	double duration;
	std::map<std::string, int> frequencyMap;
	std::map<std::string, int>::iterator it;

	// Private static method
	static bool _seeded;
	static void _seedRNG(void);
};
#endif;

