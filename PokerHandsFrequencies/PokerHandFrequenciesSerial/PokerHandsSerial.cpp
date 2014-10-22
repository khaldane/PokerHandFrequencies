/*
* Program: PokerHandFrequenciesSerial
* Module: PokerHandsSerial.cpp
* Author: Katherine Haldane & Jared Lerner
* Date: October 10, 2014
* Description: Poker Simluation Program Serial that simulates drawing 5-card
*      poker hands from a standard deck of 52 cards. The program will count
*      the number of frequencies for each of the ten hand-types.
*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <time.h>

#include "PokerHandsSerial.h"

using namespace std;

//Globals
bool PokerHandsSerial::_seeded = false;

//Constructor 
PokerHandsSerial::PokerHandsSerial(){
	PokerHandsSerial::setUpDeck();
	for (int i = 0; i < 5; ++i) {
		hand[i] = deck[i];
	}
}

//Deconstructor
PokerHandsSerial::~PokerHandsSerial(){
}

/*
* Purpose: Add 52 cards as integers to the deck
* Parameters: None
* Returns: None
*/
void PokerHandsSerial::setUpDeck(){
	for (int i = 1; i <= 52; ++i)
	{
		deck[i - 1] = i;
	}
}

/* 
* Purpose: Swap random elements from randomizeDeck()
* Parameters: Int pointer to ls, rs
* Returns: None
*/
void swap(int *ls, int *rs)
{
	int temp = *ls;
	*ls = *rs;
	*rs = temp;
}

/*
* Purpose: Randomizes the deck of integers
* Parameters: Int array and Int
* Returns: None
*/
void PokerHandsSerial::randomizeDeck(int arr[], int n){
	//Start from the last element ad swap one by one.
	for (int i = n - 1; i >= 0; i--)
	{
		//pick a random index from 0 - i
		int j = getRandom();

		//Swap deck[i] with the element at random index
		swap(&deck[i], &arr[j]);
	}
}

/*
* Purpose: Sort the hand of 5 cards to determine its suit and rank
* Parameters: const int pointer to ls, rs
* Returns:
*/
bool sort_cards(const int& ls, const int& rs) {
	return ls % 13 < rs % 13;
}

/*
* Purpose: Creates a seed if it hasn't already been seeded and gets a random number
* Parameters: void
* Returns: A random int between 0 and 51
*/
int PokerHandsSerial::getRandom(void)
{
	if (!_seeded)
		_seedRNG();

	return rand() % 52;
}

/*
* Purpose: Checks if seeded flag needs to be seeded
* Parameters: void
* Returns: None
*/
void PokerHandsSerial::_seedRNG(void)
{
	// Seed the random number generator
	srand((int)(time(0)));

	_seeded = true;
}

/*
* Purpose: Helper functions to see if it is one or two digit number.
*		   This is used to format the report.
* Parameters: Int
* Returns: Int
*/
int countDigits(int num) {
	if (num < 10) {
		return 1;
	}
	else {
		return countDigits(num / 10) + 1;
	}
}

/*
* Purpose: Simulates drawing a poker hand by referencing the first 5 cards in the deck data structure.
*		   Analyzes the hand to identify which hand-type the had represents and increments the corresponding
*		   element in the frequencies data structure. End the loop when one of every hand type has been found.
* Parameters: None
* Returns: None
*/
void PokerHandsSerial::drawHands() {
	int handTypeCount = 0;
	clock_t start;
	do {
		//Start the clock
		start = clock();
		bool isPair = false, isTriple = false, isTwoPair = false, isFullHouse = false, isQuadruple = false, isRoyal = false;
		bool isStraight = true, isFlush = true;
		//Randomize the deck of cards
		randomizeDeck(deck, 52);
		for (int i = 0; i < 5; ++i) {
			hand[i] = deck[i];
			// Check the suits for flushness while the hand is populated
			if (hand[0] / 13 != hand[i] / 13) {
				isFlush = false;
			}
		}
		// Sort the hand array with STL sort.
		sort(hand, hand + 5, sort_cards);
		// If not flush, then we can have pairs, triples, quadruples, fullhouse so check for these hand-types
		if (!isFlush) {
			for (int i = 0; i < 4;) {
				int comp = hand[i] % 13;
				// Loop until there is no longer a matching value, then calculate how many values were matched
				for (int j = i + 1; j <= 5; ++j) {
					if (j == 5 || comp != hand[j] % 13) {
						if (j - i == 2) {
							//Check if pair
							if (isPair) {
								isTwoPair = true;
							}
							//Check if triple
							else if (isTriple) {
								isFullHouse = true;
							}
							else {
								//Otherwise pair
								isPair = true;
							}
						}
						//Check if pair
						else if (j - i == 3) {
							if (isPair) {
								isFullHouse = true;
							}
							else {
								//Otherwise tripple
								isTriple = true;
							}
						}
						else if (j - i == 4) {
							//Is four of a kind
							isQuadruple = true;
						}
						i += j - i;
						break;
					}
				}
			}
		}
		// If there weren't two or more of a kind, it could be a straight
		if (!(isPair || isTwoPair || isTriple || isFullHouse || isQuadruple)) {
			// Check for a straight
			for (int i = 1; i < 5; ++i) {
				if (hand[0] % 13 != (hand[i] - i) % 13) {
					isStraight = false;
					break;
				}
			}
			// Check for a straight with aces high (royal)
			// Since it's sorted and there are no pairs or any kind, if the first is ace and the second is 10, then the rest must be J,Q,K
			if (hand[0] % 13 == 0 && hand[1] % 13 == 9) {
				isRoyal = true;
				isStraight = true;
			}
		}
		//Go through each hand and add the count to the frequency map. If it is the first time this hand has been found increase the hand-type counter
		//Set up the frequency map
		if (isFullHouse) {
			if (++frequencyMap["     Full House"] == 1) {
				++handTypeCount;
			}
		}
		else if (isTwoPair) {
			if (++frequencyMap["       Two Pair"] == 1) {
				++handTypeCount;
			}
		}
		else if (isTriple) {
			if (++frequencyMap["Three of a Kind"] == 1) {
				++handTypeCount;
			}
		}
		else if (isPair) {
			if (++frequencyMap["       One Pair"] == 1) {
				++handTypeCount;
			}
		}
		else if (isQuadruple) {
			if (++frequencyMap[" Four of a Kind"] == 1) {
				++handTypeCount;
			}
		}
		else if (isRoyal && isFlush) {
			if (++frequencyMap["    Royal Flush"] == 1) {
				++handTypeCount;
			}
		}
		else if (isStraight && isFlush) {
			if (++frequencyMap[" Straight Flush"] == 1) {
				++handTypeCount;
			}
		}
		else if (isStraight) {
			if (++frequencyMap["       Straight"] == 1) {
				++handTypeCount;
			}
		}
		else if (isFlush) {
			if (++frequencyMap["          Flush"] == 1) {
				++handTypeCount;
			}
		}
		else {
			if (++frequencyMap["        No Pair"] == 1) {
				++handTypeCount;
			}
		}
	} while (handTypeCount < 10);
	//Stop Timer
	duration = ((float)start) / CLOCKS_PER_SEC;
}

/*
* Purpose:Report total number of poker hands generated and absolute and relative frequencies for each of the ten hand-types
* Parameters: None
* Returns: None
*/
void PokerHandsSerial::report(){
	//Get the total hands generated
	unsigned int handsGenerated = 0;
	for (std::map<string, int>::iterator it = frequencyMap.begin(); it != frequencyMap.end(); ++it)
	{
		handsGenerated += it->second;
	}
	//Print out Hand Type and Frequency and relative frequency
	for (std::map<string, int>::iterator it = frequencyMap.begin(); it != frequencyMap.end(); ++it)
	{
		cout << fixed << setprecision(6);
		cout << "  " << it->first;
		printNumber(it->second);
		//Calculate relative frequency
		double value = 100.0 * (it->second) / handsGenerated;
		if (value < 10.0)
			cout << " ";
		cout << "                    " << value << '\n';
	}

	cout << fixed << setprecision(3);
	cout << "--------------------------------------------------------------------------------";
	cout << "  Hands Generated: " << handsGenerated << endl;
	cout << " Elapsed Time (s): " << duration << endl;
}


/*
* Purpose: Count the number of digits and add spaces in front of hands generated and frequency. 
*		   This function is used for formatting the report.
* Parameters: Int
* Returns: None
*/
void PokerHandsSerial::printNumber(int num) {
	for (int i = countDigits(num); i < 25; ++i)
		cout << " ";
	cout << num;
}

int main(int argc, char* argv[])
{
	PokerHandsSerial s;
	cout << "            Poker Hand Frequency Simulation [Serial Version]\n";
	cout << "================================================================================\n";
	cout << "        Hand Type                Frequency       Relative Frequency (%)         \n";
	cout << "--------------------------------------------------------------------------------\n";
	s.drawHands();
	s.report();
	cout << "--------------------------------------------------------------------------------";
}

