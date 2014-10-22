/*
* Program: PokerHandFrequenciesParallel
* Module: PokerHandsMPI.cpp
* Author: Katherine Haldane & Jared Lerner
* Date: October 10, 2014
* Description: Poker Simluation Parallel Program  that simulates drawing 5-card
*      poker hands from a standard deck of 52 cards. Using MPI and 1 (serial) or more (parallel) processors,
*      the program will count the number of frequencies for each of the ten hand-types until all have been drawn.
*      Simulation results are then output to the console.
*/

#include <cstdlib> // srand(), rand()
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <ctime>
#include "PokerHandsMPI.h"
#include <mpi.h>

using namespace std;

//Globals
bool PokerHandsMPI::_seeded = false;
//Const
const int TAG_DATA = 1, TAG_QUIT = 2, TAG_RESULTS = 3;
const int MAX_MSG_SIZE = 16;

//Constructor
PokerHandsMPI::PokerHandsMPI(){
	setUpDeck();
	frequencyMap["     Full House"] = 0;
	frequencyMap["       Two Pair"] = 0;
	frequencyMap[" Four of a Kind"] = 0;
	frequencyMap["Three of a Kind"] = 0;
	frequencyMap["       One Pair"] = 0;
	frequencyMap["    Royal Flush"] = 0;
	frequencyMap[" Straight Flush"] = 0;
	frequencyMap["       Straight"] = 0;
	frequencyMap["          Flush"] = 0;
	frequencyMap["        No Pair"] = 0;
}

//Destructor
PokerHandsMPI::~PokerHandsMPI(){
}

/*
* Purpose: Add 52 cards as integers to the deck
* Parameters: None
* Returns: None
*/
void PokerHandsMPI::setUpDeck() {
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
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

/*
* Purpose: Sort the hand of 5 cards to determine its suit and rank
* Parameters: const int pointer to ls, rs
* Returns:
*/
bool sort_cards(const int& a, const int& b) {
	return a % 13 < b % 13;
}

/*
* Purpose: Randomizes the deck of integers
* Parameters: Int array, Int, Int
* Returns: None
*/
void PokerHandsMPI::randomizeDeck(int arr[], int n, int rank) {

	//Start from the last element and swap one by one.
	for (int i = n - 1; i >= 0; i--)
	{
		//pick a random index from 0 - i
		int j = getRandom(rank);

		//Swap deck[i] with the element at random index
		swap(&deck[i], &arr[j]);
	}
}

/*
* Purpose:Report total number of poker hands generated and absolute and relative frequencies for each of the ten hand-types
* Parameters: None
* Returns: None
*/
void PokerHandsMPI::report(int numProcs){
	//Get the total hands generated
	unsigned int handsGenerated = 0;
	for (std::map<string, int>::iterator it = frequencyMap.begin(); it != frequencyMap.end(); ++it)
	{
		handsGenerated += it->second;
	}
	//Print out Hand Type and Frequency and relative frequency
	if (numProcs == 1)
		cout << "            Poker Hand Frequency Simulation [Serial Version]\n";
	else
		cout << "            Poker Hand Frequency Simulation [Parallel Version]\n";
	cout << "================================================================================\n";
	cout << "        Hand Type                Frequency       Relative Frequency (%)         \n";
	cout << "--------------------------------------------------------------------------------\n";

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
	cout << "--------------------------------------------------------------------------------\n";
	cout << "  Hands Generated: " << handsGenerated << endl;  //Hands Generated is the total frequency
	cout << " Elapsed Time (s): " << duration << endl;
	cout << "   # of Processes: " << numProcs << endl;
}

/*
* Purpose: Creates a seed if it hasn't already been seeded and gets a random number
* Parameters: void
* Returns: A random int between 0 and 51
*/
int PokerHandsMPI::getRandom(int rank)
{
	if (!_seeded)
		_seedRNG(rank);

	return rand() % 52;
}

/*
* Purpose: Checks if seeded flag needs to be seeded
* Parameters: void
* Returns: None
*/
void PokerHandsMPI::_seedRNG(int rank)
{
	// Seed the random number generator
	srand((int)(time(0) / rank));

	_seeded = true;
}

/*
* Purpose: Simulates drawing a poker hand by referencing the first 5 cards in the deck data structure.
*		   Analyzes the hand to identify which hand-type the had represents and increments the corresponding
*		   element in the frequencies data structure. End the loop when one of every hand type has been found.
* Parameters: None
* Returns: None
*/
void PokerHandsMPI::processSerial() {
	int handTypeCount = 0;
	bool isFlush, isRoyal;
	do {
		//Start the clock
		start = clock();
		bool isPair = false, isTriple = false, isTwoPair = false, isFullHouse = false, isQuadruple = false;
		isRoyal = false;
		bool isStraight = true;
		isFlush = true;
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
		// iIf there weren't two or more of a kind, it could be a straight
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
* Purpose: print out the hand to the console, useful for debugging
* Parameters: Int array
* Returns: None
*/
void PokerHandsMPI::printHand(int hand[5]) {
	for (int i = 0; i < 5; ++i) {
		cout << hand[i] % 13 + 1;
		if (hand[i] / 13 == 0) {
			cout << "S";
		}
		else if (hand[i] / 13 == 1) {
			cout << "H";
		}
		else if (hand[i] / 13 == 2) {
			cout << "D";
		}
		else if (hand[i] / 13 == 3) {
			cout << "C";
		}
		if (i != 4) {
			cout << ",";
		}
	}
	cout << endl;
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
* Purpose: Count the number of digits and add spaces in front of hands generated and frequency.
*		   This function is used for formatting the report.
* Parameters: Int
* Returns: None
*/
void PokerHandsMPI::printNumber(int num) {
	for (int i = countDigits(num); i < 25; ++i)
		cout << " ";
	cout << num;
}

/*
* Purpose: check if the slave sent in a new result
* Parameters: msg - the potential new match the slave found
* Returns: bool - true if a new match
*/
bool PokerHandsMPI::checkForMatch(string msg) {
	static map<string, int>  matches;
	if (++matches[msg] == 1) {
		return true;
	}
	else {
		return false;
	}
}

/*
* Purpose: send a terminate message to the slaves
* Parameters: numProcs - number of total process, to determine hom many messages to send
* Returns: bool - all messages sent
*/
bool PokerHandsMPI::terminateSlaves(int numProcs) {
	int msgBuff = 0;
	for (int p = 1; p < numProcs; ++p)
		MPI_Send(&msgBuff, 1, MPI_INT, p, TAG_QUIT, MPI_COMM_WORLD);

	return true;
}

/*
* Purpose: add to final count the results from a slave
* Parameters: result - number of results for the hand type
*			   procID - the id of the process that returned this result
*			   activeCount - decrement this when we get the last message from a slave process
*/
void PokerHandsMPI::tallyResult(int result, int procID, int& activeCount) {

	// procRecvd - Used to track how many results have been received from each process,
	// they come in in order and are done when all 10 results are received
	static map<int, int> procRecvd;

	int tallyThis = ++procRecvd[procID];

	if (tallyThis == 1) {
		frequencyMap["     Full House"] += result;
	}
	else if (tallyThis == 2) {
		frequencyMap["       Two Pair"] += result;
	}
	else if (tallyThis == 3) {
		frequencyMap[" Four of a Kind"] += result;
	}
	else if (tallyThis == 4) {
		frequencyMap["Three of a Kind"] += result;
	}
	else if (tallyThis == 5) {
		frequencyMap["       One Pair"] += result;
	}
	else if (tallyThis == 6) {
		frequencyMap["    Royal Flush"] += result;
	}
	else if (tallyThis == 7) {
		frequencyMap[" Straight Flush"] += result;
	}
	else if (tallyThis == 8) {
		frequencyMap["       Straight"] += result;
	}
	else if (tallyThis == 9) {
		frequencyMap["          Flush"] += result;
	}
	else if (tallyThis == 10) {
		frequencyMap["        No Pair"] += result;
		--activeCount;
	}
}

/*
* Purpose: check with a non blocking receive, if any messages have been sent from any slave process
* Parameters: matchesLeft - decremented if we find a new match
*/
void PokerHandsMPI::checkMessagesFromSlaves(int& matchesLeft, int& activeCount) {

	static string msgBuff;
	static int resultBuff;

	static int recvFlagMatch;
	static int recvFlagResult;

	MPI_Status statusMatch;
	MPI_Status statusResult;

	static MPI_Request requestMatch = 0;
	static MPI_Request requestResult = 0;

	static bool listenMatch = false;
	static bool listenResult = false;

	if (requestMatch)
	{
		// Test to see if Match a message has "come in"
		MPI_Test(&requestMatch, &recvFlagMatch, &statusMatch);
		if (recvFlagMatch)
		{
			// Message has "come in"
			if (statusMatch.MPI_TAG == TAG_DATA) {
				// check if this is a first time match
				if (checkForMatch(&msgBuff[0])) {
					--matchesLeft;
				}
			}

			// Reset the Request handle
			requestMatch = 0;
		}
	}

	if (!requestMatch)
	{
		// Start listening again for a new match message
		listenMatch = true;
		MPI_Irecv(&msgBuff[0], MAX_MSG_SIZE, MPI_CHAR, MPI_ANY_SOURCE, TAG_DATA, MPI_COMM_WORLD, &requestMatch);
	}

	if (requestResult)
	{
		// Test to see if a Result message has "come in"
		MPI_Test(&requestResult, &recvFlagResult, &statusResult);

		if (recvFlagResult)
		{
			// Message has "come in"
			if (statusResult.MPI_TAG == TAG_RESULTS) {
				tallyResult(resultBuff, statusResult.MPI_SOURCE, activeCount);
			}
			// Reset the Request handle
			requestResult = 0;
		}
	}

	if (!requestResult && activeCount > 0)
	{
		// Start listening again, for an int message/result
		listenResult = true;
		MPI_Irecv(&resultBuff, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULTS, MPI_COMM_WORLD, &requestResult);
	}
}

/*
* Purpose: perform the function of the master process, simulating card hands if serial or processing results from the slaves
* Parameters: numProcs- the of processes, if greater then one then there are slaves to manage
*/
void PokerHandsMPI::processMaster(int numProcs) {
	if (numProcs == 1)
	{
		setUpDeck();
		randomizeDeck(deck, 52);
		processSerial();
	}
	else {
		// Start the timer
		double startTime = MPI_Wtime();

		bool isAllFound = false;
		int activeCount = numProcs - 1;
		int matchesLeft = 10;

		// Get new hand types from slaves, exit when we get them all
		bool slavesTerminated = false;
		while (matchesLeft > 0 || activeCount > 0) {
			checkMessagesFromSlaves(matchesLeft, activeCount);
			if (matchesLeft == 0 && !slavesTerminated)
				// Sends a TERMINATE message to each slave
				slavesTerminated = terminateSlaves(numProcs);
		}

		duration = MPI_Wtime() - startTime;
		// print out the results
	}
}

/*
* Purpose: check with a non blocking receive, if any messages have been sent from the master process
* Parameters: activeCount - decremented if the process is told to quit
*/
void PokerHandsMPI::checkMessageFromMaster(int& activeCount)
{
	static int msgBuff, recvFlag;
	MPI_Status status;
	static MPI_Request request;

	if (request)
	{
		// Already listening for a message

		// Test to see if a message has "come in"
		MPI_Test(&request, &recvFlag, &status);

		if (recvFlag)
		{
			// Message has "come in"
			if (status.MPI_TAG == TAG_QUIT)
				// Slave is quitting
				activeCount--;

			// Reset the Request handle
			request = 0;
		}
	}

	if (!request && activeCount > 0)
	{
		// Start listening again
		MPI_Irecv(&msgBuff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
	}
}

/*
* Purpose: perform the function of a slave process, simulating card hands and processing results
* Parameters: rank - the rank of the slave process
*/
void PokerHandsMPI::processSlave(int rank) {

	MPI_Request request = 0;
	string msg = "";
	int isActive = 1;
	int exitMessage = 0;
	bool isFlush, isRoyal;

	while (isActive) {


		// Draw hands, tell the master if you get a new hand type
		bool isPair = false, isTriple = false, isTwoPair = false, isFullHouse = false, isQuadruple = false;
		isRoyal = false;
		bool isStraight = true;
		isFlush = true;
		//b)  Simulate drawing a poker hand by referencing the first 5 cards in the deck data structure
		//c) Analyze poker hand to identify which hand-type the poker hand represents and increment the corresponding element in the frequencies data structure
		//Randomize then get the first five cards of the array 
		randomizeDeck(deck, 52, rank);
		for (int i = 0; i < 5; ++i) {
			hand[i] = deck[i];
			// check the suits for flushness while the hand is populated
			if (hand[0] / 13 != hand[i] / 13) {
				isFlush = false;
			}
		}
		// Sort the hand array with STL sort.
		sort(hand, hand + 5, sort_cards);
		// if not flush, then we can have pairs, triples, quadruples, fullhouse etc so check for these
		if (!isFlush) {
			for (int i = 0; i < 4;) {
				int comp = hand[i] % 13;
				// loop until no longer a matching value, then calculate how many matched
				for (int j = i + 1; j <= 5; ++j) {
					if (j == 5 || comp != hand[j] % 13) {
						if (j - i == 2) {
							if (isPair) {
								isTwoPair = true;
							}
							else if (isTriple) {
								isFullHouse = true;
							}
							else {
								isPair = true;
							}
						}
						else if (j - i == 3) {
							if (isPair) {
								isFullHouse = true;
							}
							else {
								isTriple = true;
							}
						}
						else if (j - i == 4) {
							isQuadruple = true;
						}
						i += j - i;
						break;
					}
				}
			}
		}
		// if there weren't two or more of a kind, it could be a straight
		if (!(isPair || isTwoPair || isTriple || isFullHouse || isQuadruple)) {
			// check for a straight
			for (int i = 1; i < 5; ++i) {
				if (hand[0] % 13 != (hand[i] - i) % 13) {
					isStraight = false;
					break;
				}
			}
			// for a straight with aces high (royal)
			// since it's sorted and there are no pairs or any kind, if the first is ace and the second is 10, then the rest must be J,Q,K
			if (hand[0] % 13 == 0 && hand[1] % 13 == 9) {
				isRoyal = true;
				isStraight = true;
			}
		}
		// check for a quit message
		checkMessageFromMaster(isActive);

		// if no exit, tally up the hand, and let the master know if it's a new one
		if (isActive) {
			if (isFullHouse) {
				if (++frequencyMap["     Full House"] == 1) {
					msg = "     Full House";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isTwoPair) {
				if (++frequencyMap["       Two Pair"] == 1) {
					msg = "       Two Pair";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isQuadruple) {
				if (++frequencyMap[" Four of a Kind"] == 1) {
					msg = " Four of a Kind";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isTriple) {
				if (++frequencyMap["Three of a Kind"] == 1) {
					msg = "Three of a Kind";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isPair) {
				if (++frequencyMap["       One Pair"] == 1) {
					msg = "       One Pair";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isRoyal && isFlush) {
				if (++frequencyMap["    Royal Flush"] == 1) {
					msg = "    Royal Flush";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isStraight && isFlush) {
				if (++frequencyMap[" Straight Flush"] == 1) {
					msg = " Straight Flush";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isStraight) {
				if (++frequencyMap["       Straight"] == 1) {
					msg = "       Straight";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else if (isFlush) {
				if (++frequencyMap["          Flush"] == 1) {
					msg = "          Flush";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
			else {
				if (++frequencyMap["        No Pair"] == 1) {
					msg = "        No Pair";
					MPI_Isend(&msg[0], MAX_MSG_SIZE, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, &request);
				}
			}
		}
	}

	// send the results then quit/finalize
	MPI_Send(&frequencyMap["     Full House"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["       Two Pair"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap[" Four of a Kind"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["Three of a Kind"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["       One Pair"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["    Royal Flush"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap[" Straight Flush"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["       Straight"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["          Flush"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);
	MPI_Send(&frequencyMap["        No Pair"], 1, MPI_INT, 0, TAG_RESULTS, MPI_COMM_WORLD);

}

int main(int argc, char* argv[])
{
	if (MPI_Init(&argc, &argv) == MPI_SUCCESS)
	{
		int rank, numProcs;
		// Obtain the # of processes
		MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

		// Continue only if there are a number of processes specified
		if (numProcs > 0)
		{
			// Obtain the rank of this process
			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			if (rank == 0) {
				PokerHandsMPI ph;
				ph.processMaster(numProcs);
				ph.report(numProcs);
			}
			else {
				PokerHandsMPI ph;
				ph.processSlave(rank);
			}
		}
		else
		{
			cerr << "This program requires processes!" << endl;
		}
		MPI_Finalize();
	}
}

