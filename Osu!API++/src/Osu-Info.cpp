#include "../header/Osu-Info.h"
#include "../header/JsonCpp/json/reader.h"

// Need c++11
#if defined(_MSC_VER)
#   if _MSC_VER < 1800 
#       error This project needs atleast Visual Studio 2013
#   endif
#elif __cplusplus <= 199711L
#   error This project can only be compiled with a compiler that supports C++11
#endif

string Osu_Info::key = "";
Osu_Info* Osu_Info::Instance = new Osu_Info;

Osu_Info& Osu_Info::getInstance()
{
	return *Osu_Info::Instance;
}

void Osu_Info::setAPIKey(string _key)
{
	Osu_Info::key = _key;
}

string Osu_Info::getData(string _url)
{
	ifstream JSON = Osu_Info::getJSON(_url);
	return JSON_Parser::getInstance().OpenJSON(&JSON);
}

/*vector<vector<string>> Osu_Info::getInfo(MODE _mode, array<string, 8> _params)
{
	string url		 = Osu_Info::URL(_mode, _params);
	string data		 = Osu_Info::getData(url);
	unsigned numSets = JSON_Parser::getInstance().getNumSets(&data);
	vector<vector<string>> values;

	if (data != "[]" && data != "")
	{
		values.resize(numSets);
		for (unsigned i = 0; i < numSets; i++)
		{
			string set = JSON_Parser::getInstance().getSet(&data, i);
			Osu_Info::Table<string, string> info = JSON_Parser::getInstance().getTokenInfo(&set);
			for (unsigned j = 0; j < info.first.size(); j++)
				values[i].push_back(info.second[j]);
		}
	}
	else
	{
		#ifdef _DEBUG
			cout << "Unable to get info from data "<<endl;
		#endif
		values.resize(1);
		for(unsigned i = 0; i < getNumValue(_mode); i++)
			values[0].push_back("");
	}
	
	return values;
}*/

//**********  PRIVATE PARTS... NO TOUCHY  *****************

string Osu_Info::URL(MODE _mode, array<string, 8> _param)
{
	string url = "https://osu.ppy.sh/api/";

	// params are arranged in order as written in the wiki: https://github.com/peppy/osu-api/wiki
	switch (_mode)
	{
		/// \TODO: Smart checking for invalid values  (undecided if to implement)
	case MODE::get_beatmaps:
		url += "get_beatmaps?";
		if (_param[U32 PARAM::since] != "")
			url += ("since=" + _param[U32 PARAM::since] + "&");
		if (_param[U32 PARAM::beatmapset_ID] != "")
			url += ("s=" + _param[U32 PARAM::beatmapset_ID] + "&");
		if (_param[U32 PARAM::beatmap_ID] != "")
			url += ("b=" + _param[U32 PARAM::beatmap_ID] + "&");
		if (_param[U32 PARAM::user_ID] != "")
			url += ("u=" + _param[U32 PARAM::user_ID] + "&");
		if (_param[U32 PARAM::game_mode] != "")
			url += ("m=" + _param[U32 PARAM::game_mode] + "&");
		if (_param[U32 PARAM::autoconverts] != "")
			url += ("a=" + _param[U32 PARAM::autoconverts] + "&");
		if (_param[U32 PARAM::limit] != "")
			url += ("limit=" + _param[U32 PARAM::limit] + "&");
		break;

	case MODE::get_user:
		url += "get_user?";
		if (_param[U32 PARAM::user_ID] != "")
			url += ("u=" + _param[U32 PARAM::user_ID] + "&");
		else
		{
			#ifdef _DEBUG
				cout << "ERROR: \"user_id\" required field is not specified for the \"get_user\" mode" << endl;
				system("PAUSE");
			#endif
			return "";
		}
		if (_param[U32 PARAM::game_mode] != "")
			url += ("m=" + _param[U32 PARAM::game_mode] + "&");
		break;

	case MODE::get_scores:
		url += "get_scores?";
		if (_param[U32 PARAM::beatmap_ID] != "")
			url += ("b=" + _param[U32 PARAM::beatmap_ID] + "&");
		else
		{
			#ifdef _DEBUG
				cout << "ERROR: \"beatmap_id\" required field is not specified for the \"get_scores\" mode" << endl;
				system("PAUSE");
			#endif		
			return "";
		}
		if (_param[U32 PARAM::user_ID] != "")
			url += ("u=" + _param[U32 PARAM::user_ID] + "&");
		if (_param[U32 PARAM::game_mode] != "")
			url += ("m=" + _param[U32 PARAM::game_mode] + "&");
		break;

	case MODE::get_user_best:
		url += "get_user_best?";
		if (_param[U32 PARAM::user_ID] != "")
			url += ("u=" + _param[U32 PARAM::user_ID] + "&");
		else
		{
			#ifdef _DEBUG
				cout << "ERROR: \"user_id\" required field is not specified for the \"get_user_best\" mode" << endl;
				system("PAUSE");
			#endif
			return "";
		}
		if (_param[U32 PARAM::game_mode] != "")
			url += ("m=" + _param[U32 PARAM::game_mode] + "&");
		if (_param[U32 PARAM::limit] != "")
			url += ("limit=" + _param[U32 PARAM::limit] + "&");
		break;

	case MODE::get_user_recent:
		url += "get_user_recent?";
		if (_param[U32 PARAM::user_ID] != "")
			url += ("u=" + _param[U32 PARAM::user_ID] + "&");
		else
		{
			#ifdef _DEBUG
				cout << "ERROR: \"user_id\" required field is not specified for the \"get_user_recent\" mode" << endl;
				system("PAUSE");
			#endif
			return "";
		}
		if (_param[U32 PARAM::game_mode] != "")
			url += ("m=" + _param[U32 PARAM::game_mode] + "&");
		if (_param[U32 PARAM::limit] != "")
			url += ("limit=" + _param[U32 PARAM::limit] + "&");
		break;

	case MODE::get_match:
		url += "get_match?";
		if (_param[U32 PARAM::match_ID] != "")
			url += ("mp=" + _param[U32 PARAM::match_ID] + "&");
		else
		{
			#ifdef _DEBUG
				cout << "ERROR: \"match_id\" required field is not specified for the \"get_match\" mode" << endl;
				system("PAUSE");
			#endif
			return "";
		}
		break;

	default:
		#ifdef _DEBUG
			cout << "ERROR: Illegal or unsupported mode specified!" << endl;
			system("PAUSE");
		#endif	
		return "";
	};

	url += ("k=" + getAPIKey());  // required; error otherwise
	return url;
}

ifstream Osu_Info::getJSON(string _url)
{
	bool reload = false;

	#ifdef _DEBUG
		cout<< "Beginning download" << endl;
	#endif
	try
	{
		bool good = Downloader::getInstance().download((char*)_url.c_str(), reload);
		#ifdef _DEBUG
			if(good)
				cout<< "Download Complete" << endl;
		#endif
	}
	catch (DLExc exc)
	{
		#ifdef _DEBUG
			cout << exc.geterr() <<endl 
				 << "Download interrupted" << endl;
		#endif
	}

	return ifstream("data.txt");
}

int Osu_Info::findToken(vector<string> *_set, string _token)
{
	unsigned i = 0;
	for (; ((i<_set->size()) && (_set->at(i)!=_token)); i++);

	if (i == _set->size())
	{
		#ifdef _DEBUG
			cout << "ERROR: Token " << _token << " not found!" << endl;
			system("PAUSE");
		#endif
		return -1;
	}
	else return i;
}

string Osu_Info::getAPIKey()
{
	string API_KEY;
	bool APIFound = false;

	/// \TODO: Add option to enter new API key
	while (!APIFound)
	{
		ifstream APIkey_in("API.key");
		if (!APIkey_in.good())
		{
			APIkey_in.close();
			#ifdef _DEBUG
				cout << "API key not found! Applying new key..."<<endl;
			#endif
			API_KEY = Osu_Info::key;

			ofstream APIKey_out = ofstream("API.key");
			APIKey_out << API_KEY;
			APIKey_out.close();
		}
		else
		{
			APIkey_in >> API_KEY;
			APIFound = true;
		}
	}

	return API_KEY;
}

Osu_Info::Osu_Info(){}
Osu_Info::~Osu_Info(){ delete Osu_Info::Instance; }
Osu_Info::Osu_Info(Osu_Info const&){}
Osu_Info& Osu_Info::operator=(Osu_Info const&){ return *this; }