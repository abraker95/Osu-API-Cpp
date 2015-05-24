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
#include "JSON-Parser.h"
#include "Downloader.h"

using namespace std;
#define U32 (unsigned)

namespace GAMEMODE{
	 const string Osu = "0", Taiko = "1", CtB = "2", Mania = "3";
};

class Osu_Info
{
	public:
		template <typename X, typename Y> using Table = pair<vector<X>, vector<Y>>;

		enum class MODE{ get_beatmaps = 0, get_user, get_scores, get_user_best, get_user_recent, get_match };
		enum class PARAM{ user_ID = 0, beatmap_ID, limit, beatmapset_ID, match_ID, autoconverts, game_mode, since };  // event_days and type are not supported for now

		enum class GET_BEATMAPS { beatmapset_id = 0, beatmap_ID, approved, approved_date, last_update, total_length, hit_length, version,
			artist, title, creator, bpm, source, difficultyrating, diff_size, diff_overall, diff_approach, diff_drain, mode, SIZE
		};
		/*enum class GET_USER { user_ID = 0, username, count300, count100, count50, playcount, ranked_score, total_score, pp_rank, level, pp_raw, accuracy,
			count_rank_ss, count_rank_s, count_rank_a, country, events, beatmap_ID, beatmapset_ID, data, epicfactor, SIZE
		};*/ // Unsuported for now :(
		enum class GET_SCORES { score = 0, username, count300, count100, count50, countmiss, maxcombo, countkatu, countgeki, perfect, enabled_mods,
			user_ID, date, rank, pp, SIZE
		};
		enum class GET_USER_BEST {	beatmap_ID = 0, score, maxcombo, count50, count100, count300, countmiss, countkatu, countgeki, perfect,
			enabled_mods, user_ID, date, rank, pp, SIZE
		};
		enum class  GET_USER_RECENT { beatmap_ID = 0, score, maxcombo, count50, count100, count300, countmiss, countkatu, countgeki, perfect,
			enabled_mods, user_ID, date, rank, SIZE
		};
		enum class GET_MATCH { match = 0, games, SIZE }; // not used for now
		enum class MODS { /* TODO */ SIZE }; // not used for now

		static Osu_Info& getInstance();

		// Sets the API key
		void setAPIKey(string _key);

		// Gets the info from the URL
		// \RET: 
		//		@Fail: Empty string if invalid URL
		//      @Good: Contents of data.txt (the downloaded JSON file)
		string getData(string _url);

		// Gets the values from the sets beased on the given mode and paramenters.
		// Organizes the data by: [# set][Type of data]
		// \RET: 
		//		@Fail: Empty vector if data.txt is empty or invalid
		//      @Good: Vector of values
		vector<vector<string>> getInfo(MODE, array<string, 8> _params);

		// Deprecated
		// Goes through all the sets within the data, to find and extract data relating to the given token(s).
		//static vector<Table<string, string>> getTables(string* _data, vector<string> _tokens);

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
