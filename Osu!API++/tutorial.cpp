/*
	abraker, May 2015
*/

#pragma comment(lib, "Osu!API++-debug")

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
	// user_best info. It is loaded into the vector as so: user_best[row][token]
	vector<vector<string>> user_best;

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
	params[U32 Osu_Info::PARAM::user_ID]	= "abraker";
	params[U32 Osu_Info::PARAM::game_mode]	= GAMEMODE::Mania;
	params[U32 Osu_Info::PARAM::limit]		= "50";
	user_best = Osu_Info::getInstance().getInfo(Osu_Info::MODE::get_user_best, params);

	// refresh the parameters
	params.fill(""); 

	// we will need these to store the info
	vector<string> diff;
	vector<vector<string>> beatmap_info;

	// go through the beatmaps, getting their info
	for (unsigned j = 0; j < user_best.size(); j++) 
	{
		params[U32 Osu_Info::PARAM::beatmap_ID] = user_best[j][U32 Osu_Info::GET_USER_BEST::beatmap_ID];  // access the jth value of the beatmap_ID token
		params[U32 Osu_Info::PARAM::game_mode]  = GAMEMODE::Mania;
		params[U32 Osu_Info::PARAM::limit]		= "50";
		beatmap_info = Osu_Info::getInstance().getInfo(Osu_Info::MODE::get_beatmaps, params);

		// since there is only 1 beatmap in question, we will only need to access the 0th index
		diff.push_back(beatmap_info[0][U32 Osu_Info::GET_BEATMAPS::difficultyrating]);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // limit the number of requests per time period so peppy doesn't get upset
	}
	
	// print out the data
	cout << "Difficulty          PP" << endl;
	for (unsigned j = 0; j < user_best.size(); j++)
	{
		cout << diff[j] << "   "
			 << user_best[j][U32 Osu_Info::GET_USER_BEST::pp] << endl;
	}
	std::system("PAUSE");
	return EXIT_SUCCESS;
}
