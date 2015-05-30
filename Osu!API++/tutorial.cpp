/*
	abraker, May 2015
*/

//#pragma comment(lib, "Osu!API++-debug")

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include "header/Osu-Info.h"

using namespace std;

int main(int argc, char *argv[])
{
	// user_best info. The template parameter is the type of data we will be getting
	Osu_Info::OsuData<MODE::get_user_best> user_best;

	// declare an array of parameters. THIS MUST BE SIZE OF 8!
	array<string, 8> params;  

	
	ifstream APIKey = ifstream("API.key");
	if (!APIKey.good()) // detection could be better like chacking if the length is correct, but this will suffice for now
	{
		APIKey.close();
		string apiKey;
		cout << "API key not found! Please enter your API key: ";
		cin >> apiKey;  // replace this with your method of getting the API key
		Osu_Info::getInstance().setAPIKey(apiKey);
	}
	
	// load parameters
	params[U32 PARAM::user_ID]	 = "abraker";
	params[U32 PARAM::game_mode] = GAMEMODE::Mania;
	params[U32 PARAM::limit]	 = "50";
	user_best = Osu_Info::getInstance().getInfo<MODE::get_user_best>(params);

	// refresh the parameters
	params.fill(""); 

	// we will need these to store the info
	vector<string> diff;
	Osu_Info::OsuData<MODE::get_beatmaps> beatmap_info;

	// go through the beatmaps, getting their info
	for (unsigned j = 0; j < user_best.size(); j++) 
	{
		params[U32 PARAM::beatmap_ID] = user_best.getValue(j, "beatmap_id");  // access the jth value of the beatmap_id token
		params[U32 PARAM::game_mode]  = GAMEMODE::Mania;
		params[U32 PARAM::limit]	  = "50";
		beatmap_info = Osu_Info::getInstance().getInfo<MODE::get_beatmaps>(params);

		// since there is only 1 beatmap in question, we will only need to access the 0th index
		diff.push_back(beatmap_info.getValue(0, "difficultyrating"));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // limit the number of requests per time period so peppy doesn't get upset
	}
	
	// print out the data
	cout << "Difficulty          PP" << endl;
	for (unsigned j = 0; j < user_best.size(); j++)
	{
		cout << diff[j] << "   "
			 << user_best.getValue(j, "pp") << endl;
	}
	std::system("PAUSE");
	return EXIT_SUCCESS;
}
