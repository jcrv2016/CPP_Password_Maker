#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <math.h>
#include <Windows.h>
#include <vector>
#include <sstream>
#include <stdarg.h>
#include <cctype>
#include <unordered_map> 
#include <algorithm>

using namespace std;

random_device device;
mt19937 generator(device());

int getRandom(int limit=0)
{
	int maxNo;
	if (limit)
	{
		maxNo = (limit -1);
	}
	else
	{
		maxNo = 10;
	}
	uniform_int_distribution<int> distribution(1, maxNo);
	return distribution(generator);
}

class Word
{
private:
	string word;				//main repository for word
	int length;
	unordered_map<char, char> charmap;
	void initializeChars();
public:

	//constructors and destructors
	Word();
	Word(string text);

	void setWord(string i);
	string getWord();
	string replaceChars(unordered_map<char, char> charmap, string inString);
	string passwordize(int r);
	int getLength();
	char random_chars();
	void l33tize();
};
Word::Word(string text)
{
	setWord(text); 
	initializeChars();
}
Word::Word()
{
	initializeChars();
}
void Word::initializeChars()
{
	charmap['A'] = '@';
	charmap['E'] = '3';
	charmap['I'] = '1';
	charmap['O'] = '0';
	charmap['S'] = '$';
	charmap['T'] = '7';
}
void Word::setWord(string input)
{
	word = input;
}
string Word::getWord()
{
	return word;
}
char Word::random_chars()
{
	char randomChars[22] = "~!@#$%^&*()0123456789";
	int randIndex = getRandom((sizeof(randomChars) - 1));
	char c = randomChars[randIndex];
	return c;
}
int Word::getLength()
{
	return word.length();
}
string Word::replaceChars(unordered_map<char, char> charmap, string inString)
{
	string outString = inString;

	/*traverse inString. in between, traverse charmap. test whether each char traversal in inString
	matches a char in the charmap. if it does, call getRandom(), and if the random number is even, replace the char in
	inString with the item in charmap*/

	for (auto i = 0; i <= (outString.length() - 1); i++)
	{
		
		if (getRandom() % 2)
		{
			outString[i] = toupper(outString[i]);
		}
		for (auto x : charmap)
		{
			if (toupper(outString[i]) == x.first)
			{
				if (getRandom() % 2)
				{
					outString[i] = x.second;
				}
			}
		}
	}
	return outString;
}
void Word::l33tize() //replaces certain letters for numbers and symbols
{
	word = replaceChars(charmap, word);
}
string Word::passwordize(int r)
//randomly changes case of word, then calls l33tize(), which replaces certain letters for numbers and symbols
//then adds 0 or more random characters to end of string
{
	int randCharAmt = r;
	l33tize();
	for (int i = 0; i < randCharAmt; i++)
	{
		if (getRandom() % 2)
		{
			word = this->random_chars() + word;
		}
		else
		{
			word += this->random_chars();
		}
	}
	return word;
}

class WordList
{
private:
	typedef vector<Word> UnorderedWordSet;
	typedef unordered_map<int, UnorderedWordSet> WordsMapped2Lengths;
	WordsMapped2Lengths wordMap;
	vector<int> sorted;
public:
	//constructors and destructors

	//access functions
	
	//modifier functions
	void insert(Word e);

	//input/output functions
	int getCount();
	void makePasswords(WordList source);

	//data members
	int wordListLength = 0;

	Word chooseWord(int c);
	void CSV(string filename);
};

int WordList::getCount()
{
	return wordListLength;
}
//modifier functions
void WordList::insert(Word e)
{
	wordMap[e.getLength()].push_back(e);
	wordListLength += 1;
}
void WordList::makePasswords(WordList source)

//meta-function for password production
//asks for word length and quantity, then calls chooseWord, which calls other functions
//when password produced, inputs words into WordList
{
	int wordLength;
	bool noMatches;
	do {
		noMatches = false;
		do {
			//can define min/max in class
			cout << "Specify length of passwords (min 8, max 20): \n";
			cin >> wordLength;
		} while (wordLength < 8 || wordLength > 20);
		int quantity;
		cout << "How many passwords to produce?\n";
		cin >> quantity;
		quantity -= 1;
		Word temporary;
		for (int i = 0; i <= quantity; i++)
		{
			temporary = source.chooseWord(wordLength);
			if (temporary.getWord() == "0")
			{
				noMatches = true;
				break;
			}
			else
			{
				insert(temporary);
				cout << temporary.getWord() << "\n";
			}
		}
		if (noMatches)
		{
			cout << "Please pick a new number. Not enough words of this length found in list.\n";
		}
	} while (noMatches);
}

void WordList::CSV(string filename) //outputs wordlist to csv
{
	ofstream outfile;
	outfile.open(filename);
	for (int i = 0; i < wordMap.size(); i++)
	{
		for (int j = 0; j < wordMap[i].size(); j++)
		{
			outfile<< wordMap[i].at(j).getWord() << "," << endl;
		}
	}
	outfile.close();
	cout << "File Saved.\n";
}

Word WordList::chooseWord(int c)
{
	int wordLength = c;
	int maxRandomChars = 3;
	Word temp;
	int randCharAmt = getRandom(maxRandomChars);
	int targetLength = wordLength - randCharAmt;
	if (!wordMap[targetLength].empty())
	{
		//if the vector hasn't been sorted yet, do so, then note it.
		if (find(sorted.begin(), sorted.end(), targetLength) == sorted.end())
		{
			random_shuffle(wordMap[targetLength].begin(), wordMap[targetLength].end());
			sorted.push_back(targetLength);
		}
		temp = wordMap[targetLength].back();
		wordMap[targetLength].pop_back();
		temp.setWord(temp.passwordize(randCharAmt));
	}
	else
	{
		temp.setWord("0");
	}
	return temp;
};

int main() {
	string line, filename;
	cout << "Please specify the filename of dictionary to read in:\n";
	cin >> filename;
	ifstream infile(filename);
	if (infile.is_open())
	{
		clock_t t;
		t = clock();
		cout << "Reading in dictionary\n";
		WordList inputList;
		while (getline(infile, line))
		{
			inputList.insert(Word(line));
		}
		t = clock() - t;
		cout << '\a';
		cout << "It took me " << (t / CLOCKS_PER_SEC) << " seconds to read in " << inputList.getCount() << " words.\n";
		cout << "Dictionary read in successfully." << "\n";
		WordList outputList;
		string yOrNo = "";
		char input = '0';

		do {
			outputList.makePasswords(inputList);
			cin.clear();                              //clear errors/bad flags on cin
			cout << "\nWant to produce more words? (y/n)";
			getline(cin, yOrNo);
			//why stringstream here, and cin below?
			stringstream myStream(yOrNo);
			myStream >> input;
			if ((input == 'n' ) | (input == 'N'))
				break;
		} while (true);
		cout << "\nPlease specify the filename of output CSV file for generated passwords (ending in CSV):\n";
		string path;
		cin >> path;
		outputList.CSV(path);
		system("pause");
	}
	else cout << "\nUnable to open file";
	return 0;
}
