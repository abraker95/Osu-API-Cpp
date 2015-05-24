/*
	By: abraker 4/25/15
*/

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#ifdef _DEBUG
	#include <iostream> 
#endif
#include <fstream>
#include <vector>
#include <string>
//#include <utility>      // std::pair

using namespace std;

/// \TODO: Make the string parameters pointers so that huge string of data doesn't get copied all over the place
/// \INFO: Parses JSON files and returns requested info
class JSON_Parser
{
	public:
		static JSON_Parser& getInstance();

		/**
		   \TODO: Big files will make the string too big. Have the file still open for reading?
		   \INFO: Opens the JSON file for reading
		   \RET:  The text inside the file. 
		**/
		string OpenJSON(ifstream* _fin);  

		// JSON files are divided into sets of data. This funtion gets the number of those sets
		int getNumSets(string* _data);

		// Gets the number set, starting from 0
		string getSet(string* _data, int _set);

		// Gets the token name and value that token is equavilent to
		pair<vector<string>, vector<string>> getTokenInfo(string* _set);

		// Returns a list of tokens available
		vector<string> getTokenList(string* _set);

		// Returns a list of values associated with the list of tokens
		vector<string> getValueList(string* _set);

	private:
		static JSON_Parser* Instance;

		JSON_Parser();
		JSON_Parser(JSON_Parser const&);
		JSON_Parser& operator=(JSON_Parser const&);
		
};

#endif