# Osu-API-Cpp
A C++ API for interfacing with the Osu API: https://github.com/peppy/osu-api/wiki

Refer to [tutorial.cpp](https://github.com/abraker95/Osu-API-Cpp/blob/master/Osu!API%2B%2B/tutorial.cpp) when reading this.

Before you begin, you might want to create a API.key file at the root of the project with your osu!API key in it for early testing of your application. Alternatively, you can implement a way to request the user for it, but the library wont work until there is a valid key in the directory. This key is passed as a parameter behind the scenes when generating the url, so you will only need to worry about it just being there and valid.

The format of this tutorial is code first then explanation. So then...

~~~
// declare an array of param. THIS MUST BE SIZE OF 8!
array<string, 8> params; 

// user_best info. The template parameter is the type of data we will be getting
Osu_Info::OsuData<MODE::get_user_best> user_best;
~~~

Now to start, we need to declare to variable: One to hold the parameters and one to hold the info we are going to get. The variable to hold the parameters is a std::array type and MUST be of size 8. The variable to hold the info is called OsuData. So OsuData has a template arguement called MODE::get_user_best. This the template arguement indicates a mode which allows the variable to store user best data. All types of data are named accordingly to match what is listed under [osu!API's documentation](https://github.com/peppy/osu-api/wiki).

~~~
// load parameters
params[U32 PARAM::user_ID]	 = "abraker";
params[U32 PARAM::game_mode] = GAMEMODE::Mania;
params[U32 PARAM::limit]	 = "50";
user_best = Osu_Info::getInstance().getInfo<MODE::get_user_best>(params);
~~~

The osu!API takes up to 8 types of parameters. Here we are passing the user ID, game mode, and limit as listed under the get_user_best section of the osu!API documentation. Some modes require parameters such as user ID to be specified and the library WILL complain if it is not. Refer to [osu!API documentation](https://github.com/peppy/osu-api/wiki) on the required parameters under the specified mode(s). After loading the parameters, the pass the to the getInfo function which returns the data we initially wanted. Notice that the getInfo function also has a template parameter. That HAS to match the type of data you are assigning it to or your compiler will error.

~~~
// refresh the parameters
params.fill(""); 

// we will need these to store the info
vector<string> diff;
Osu_Info::OsuData<MODE::get_beatmaps> beatmap_info;
~~~

So now that we got the user_best data, we might want to get something like the difficulty rating of the map the user to good on. Unfortunatly, that info is not part of user_best. Howver, what we can use in the user_best data to get the diffuclty rating is the beatmap ID. So let's just clear the parameters so we can reuse the variable and declare new variables where we are going to hold the data. Yea, and the osuData type is not reusable in this case since it contains a different type of data. For this we need a new osuData variable using the get_beatmaps mode.

~~~
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
~~~

Now lets iterate over the user's top maps to get the difficulty of each one. To access the data within osuData we use its getValue member function. Since we specified to return the data pertaining to a certain map by passing the beatmap ID parameter, there will be only one beatmap returned. So in this example, we are getting the 0th difficulty rating value (1st map). Refer to the [osu!API documentation](https://github.com/peppy/osu-api/wiki) for other supported data. Since beatmap_info will be overwritten with each iteration, we store the the difficulty rating into a vector of strings. Alternatively it can be an array of strings, but concidering that some beatmaps are takened down ([this](https://osu.ppy.sh/s/145573) one for example), the API will have trouble retriving info. In this event, the data returned should be empty strings.

~~~
/ print out the data
	cout << "Difficulty          PP" << endl;
	for (unsigned j = 0; j < user_best.size(); j++)
	{
		cout << diff[j] << "   "
			 << user_best.getValue(j, "pp") << endl;
	}
~~~
	
If the previous example is too hard to figure out on how to get the data, here is a simpler example. Here we are just printing the difficulty rating and PP of the beatmaps one by one. 
