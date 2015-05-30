/*
	By: abraker
	Date: May 2015

	4/25/15:
		Initial work begins
		[ADD] Download class
		[ADD] JSON_Parser class
	
	4/28/15:
		[ADD] Store to extracted JSON tokens and values into vector

	4/29/15:
		[ADD] Osu_Info class

	5/??/15:
		Undocumented :(  (About 4 redesigns of the API during this phase)

	5/11/15:
		API 90% complete
		Uploaded to Github
		
	5/17/15:
		[FIX] Fixed the Debug release crashing
		Initial Release

	ERROR CODES:  
		-1:  Invalid JSON File
		-2:	 Invalid API key
		-3:	 Invalid set
		-4:	 Blank data
		-5:	 Illegal action/mode specified
		-6:  Required peram is not specied for action/mode
		-7:  Token not found
*/
#ifdef _DEBUG
	#include <iostream>
#endif
#include <utility>		// std::pair
#include <vector>
#include <string>
#include <array>
#include "../header/JsonCpp/json/reader.h"
#include "Downloader.h"

using namespace std;
#define U32 (unsigned)

namespace GAMEMODE{
	 const string Osu = "0", Taiko = "1", CtB = "2", Mania = "3";
};

enum class MODE{ get_beatmaps = 0, get_user, get_scores, get_user_best, get_user_recent, get_match };
enum class PARAM{ user_ID = 0, beatmap_ID, limit, beatmapset_ID, match_ID, autoconverts, game_mode, since };  // event_days and type are not supported for now
enum class MODS { /* TODO */ SIZE }; // not used for now

class Osu_Info
{
	public:
		template <typename X, typename Y> using Table = pair<vector<X>, vector<Y>>;
		
		template<MODE _mode>
		struct OsuData
		{
			OsuData(Json::Value& _data)
			{
				data = _data;
			}

			OsuData(){}

			string getValue(int _index, string _param)
			{
				string value = "";
				if (data.size() == 0)
				{
					#ifdef _DEBUG
						cout << "ERROR: Data never loaded!" << endl;
					#endif
					return value;
				}

				if(_index>=0 && _index<data.size())
				{
					bool valid = false;
					if (_mode == MODE::get_beatmaps)
					{
						valid = (_param == "beatmapset_id") || (_param == "approved") || (_param == "approved_date") ||
						        (_param == "last_update") || (_param == "total_length") || (_param == "hit_length") ||
								(_param == "version") || (_param == "artist") || (_param == "title") ||
								(_param == "creator") || (_param == "bpm") || (_param == "source") ||
								(_param == "difficultyrating") || (_param == "diff_size") || (_param == "diff_overall") ||
								(_param == "diff_approach") || (_param == "diff_drain") || (_param == "mode");
					}
					else if (_mode == MODE::get_scores)
					{
						valid = (_param == "score") || (_param == "username") || (_param == "count300") ||
							    (_param == "count100") || (_param == "count50") || (_param == "countmiss") ||
								(_param == "maxcombo") || (_param == "countkatu") || (_param == "countgeki") ||
								(_param == "perfect") || (_param == "enabled_mods");	
					}
					else if (_mode == MODE::get_user_best)
					{
						valid = (_param == "beatmap_id") || (_param == "score") || (_param == "maxcombo") ||
							    (_param == "count50") || (_param == "count100") || (_param == "count300") ||
								(_param == "countmiss") || (_param == "countkatu") || (_param == "countgeki") ||
								(_param == "perfect") || (_param == "enabled_mods") || (_param == "user_id") ||
								(_param == "date") || (_param == "rank") || (_param == "pp");
					}
					else if (_mode == MODE::get_user_recent)
					{
						valid = (_param == "beatmap_id") || (_param == "score") || (_param == "maxcombo") ||
							    (_param == "count50") || (_param == "count100") || (_param == "count300") ||
								(_param == "countmiss") || (_param == "countkatu") || (_param == "countgeki") ||
								(_param == "perfect") || (_param == "enabled_mods") || (_param == "user_id") ||
								(_param == "date") || (_param == "rank");
					}
					else if (_mode == MODE::get_user)
					{
						valid = (_param == "user_ID") || (_param == "username") || (_param == "count300") ||
							    (_param == "count100") || (_param == "count50") || (_param == "playcount") ||
								(_param == "ranked_score") || (_param == "total_score") || (_param == "pp_rank") ||
								(_param == "level") || (_param == "pp_raw") || (_param == "accuracy") ||
								(_param == "count_rank_ss") || (_param == "count_rank_a") || (_param == "country") ||
								/*(_param == "events") ||*/ (_param == "beatmap_id") || (_param == "beatmapset_id") ||
								(_param == "data") || (_param == "epicfactor"); // events Unsuported for now :(
					}
					/*else if (_mode == MODE::get_match)  // Unsuported for now
					{
						valid = (_param == "match") || (_param == "games");
					}*/
					else
					{
						#ifdef _DEBUG
								cout << "ERROR: Invalid mode!" << endl;
						#endif
					}

					if (valid)
						value = data[_index][_param].asString();
					else
					{
						#ifdef _DEBUG
							cout << "ERROR: Invalid token \""<<_param<<"\" for specified mode!" << endl;
						#endif
					}
				}
				else
				{
					#ifdef _DEBUG
								cout << "ERROR: Specified range is out of bounds!" << endl;
					#endif
				}

				return value;
			}

			unsigned int size() const
			{
				return data.size();
			}

			void operator=(Json::Value& _data){ data = _data; }

			private:
				Json::Value data;
		};

		static Osu_Info& getInstance();

		// Sets the API key
		void setAPIKey(string _key);

		// Gets the info from the URL
		// \RET: 
		//		@Fail: Empty string if invalid URL
		//      @Good: Contents of data.txt (the downloaded JSON file)
		string getData(string _url);

		template<MODE T>
		OsuData<T> getInfo(array<string, 8> _params)
		{
			OsuData<T> data;
			Json::Value root;
			Json::Reader reader;
			string url = Osu_Info::URL(T, _params);

			bool parsedSuccess = reader.parse(Osu_Info::getJSON(url), root, false);
			if (!parsedSuccess)
			{
				#ifdef _DEBUG
					cout << "Failed to parse JSON" << endl
					  	 << reader.getFormattedErrorMessages() << endl;
				#endif
			}

			data = root;
			return data;
		}

	private:
		static string key;
		static Osu_Info* Instance;

		Osu_Info();
		~Osu_Info();
		Osu_Info(Osu_Info const&);
		Osu_Info& operator=(Osu_Info const&);
		
		// Returns the specified token's index within the set
		int findToken(vector<string> *_set, string _token);

		// Returns the entire contents of the JSON file as recieved from the server
		ifstream getJSON(string _url);

		// Returns the number of values expected from the indicated mode
		unsigned getNumValue(MODE _mode);

		// Returns the API key from the API.key file. Creates a new API.key file if it is not found with the input Osu_Info::key
		string getAPIKey();

		// Generates an URL based on the given paramenters. It also ensurse that the info has been corretly inputed
		string URL(MODE _mode, array<string, 8> _param);
};

