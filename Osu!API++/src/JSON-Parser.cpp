/*
	By: abraker 4/25/15
*/

#include "../header/JSON-Parser.h"
#define MAX(a, b) (a>b? a : b)

JSON_Parser* JSON_Parser::Instance = NULL;

JSON_Parser& JSON_Parser::getInstance()
{
	static JSON_Parser Instance;
	return Instance;
}

int find(string* _str, char _chr, unsigned _beg = 0, unsigned _end = -1)
{
	if (_end > _str->size()) _end = _str->size();
	for (unsigned i = _beg; i < MAX(_str->size(), _end); i++)
	{
		if ((*_str)[i] == string(&_chr)[0])
			return i;
	}
	return -1;
}

string JSON_Parser::OpenJSON(ifstream* _fin)
{
	string buff = "", data = "";

	while (_fin->good())
	{
		*_fin >> buff;
		data += buff;

		if (buff == "[]")
		{
			#ifdef _DEBUG
				cout << endl << "Warning: Blank data!" << endl;
			#endif
		}
		else if (buff.find("error") != -1)
		{
			#ifdef _DEBUG
				cout << endl  << "An error has occured! Check data.txt for more info." << endl;
				system("PAUSE");
			#endif
			return "";
		}
	}
	_fin->close();

	if (data.size() == 0)
	{
		#ifdef _DEBUG
			cout << endl << "Error: Invalid or blank File";
			system("PAUSE");
		#endif
		return "";
	}
	else if (!((data.at(0) == '[') && (data.at(data.size() - 1) == ']')))
	{
		#ifdef _DEBUG
			cout << data.at(0) << data.at(data.size() - 1) << endl;
			cout << endl << "FILE IS NOT A JSON FILE";
			system("PAUSE");
		#endif
		return "";
	}

	return data;
}

int JSON_Parser::getNumSets(string* _data)
{
	int pos = 0, count = -1;
	while (pos != -1)
	{
		pos = find(_data, '}', pos+1);
		count++;
	}
	return count;
}

string JSON_Parser::getSet(string* _data, int _setNum)
{
	if (_setNum < 0) _setNum = 0;
	if (_setNum < JSON_Parser::getNumSets(_data))
	{
		int posBeg = 0, posEnd = 0, count = 0;
		while (posEnd != -1 && count != _setNum + 1)
		{
			posBeg = find(_data, '{', posBeg + 1);
			posEnd = find(_data, '}', posBeg);
			count++;
		}
		if (posBeg == -1)
		{
			#ifdef _DEBUG
				cout << "ERROR Getting set number " << _setNum << " out of " << count << endl;
				system("PAUSE");
			#endif
			return "";
		}
		else
			return _data->substr(posBeg, posEnd - posBeg + 1);
	}
	else return "";
}

pair<vector<string>, vector<string>> JSON_Parser::getTokenInfo(string* _set)
{
	pair<vector<string>, vector<string>> TokenInfo;
	unsigned pos = 0;
	bool inQuotes = false;
	enum { TOKEN, VALUE } type = TOKEN;

	if (_set->size() == 0)
	{
		#ifdef _DEBUG
				cout << "Invalid set... Getting token list failed!" << endl;
				system("PAUSE");
		#endif
		return TokenInfo;
	}
	else
	{
		if (((_set->at(0) != '{') && (_set->at(_set->size() - 1) != '}')) || _set->size() == 0)
		{
			#ifdef _DEBUG
				cout << "Invalid set... Getting token list failed!" << endl;
				system("PAUSE");
			#endif
			return TokenInfo;
		};

		while (pos < _set->size())
		{
			if (inQuotes)
			{
				int posTmp = pos, size = 0;
				if (_set->at(pos) == '\"')  /// \NOTE: Prevents a bug where there is an empty value
					size = 0;
				else
				{
					while ((_set->at(++pos) != '\"') || !(_set->at(pos - 1) != '\\'));   /// \NOTE: check for '\\' to make sure the value is not implying a string 
					size = pos - posTmp;
				}

				if (type == TOKEN) TokenInfo.first.push_back(_set->substr(posTmp, size));   /// \NOTE: No +1 to not include quotes 
				if (type == VALUE) TokenInfo.second.push_back(_set->substr(posTmp, size));
				inQuotes = false;
			}
			else
			{
				if (_set->at(pos) == '\"')	inQuotes = true;
				else if (_set->at(pos) == ':')
				{
					if (_set->substr(pos + 1, 4) != "null")
					{
						type = VALUE;
					}
					else
					{
						pos += 5;								// a "null" bug where JSON apperently allows null values 
						TokenInfo.second.push_back("null");     // to be without quotes, allowing an uneven vector pair
					}
				}
				else if (_set->at(pos) == ',')	type = TOKEN;
			}

			pos++;
		}
		return TokenInfo;
	}
}

vector<string> JSON_Parser::getTokenList(string* _set)
{
	return JSON_Parser::getTokenInfo(_set).first;
}

vector<string> JSON_Parser::getValueList(string* _set)
{
	return JSON_Parser::getTokenInfo(_set).second;
}

JSON_Parser::JSON_Parser(){}
JSON_Parser::JSON_Parser(JSON_Parser const&){}
JSON_Parser& JSON_Parser::operator=(JSON_Parser const&){ return *this; }