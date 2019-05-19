#include "saltyonline.h"

#include <stdio.h>

struct StateData {
	std::string player_one;
	std::string player_two;
	std::string status;
	std::string remaining;
	
	void Set (StateData *);
};

void StateData::Set (StateData *data) {
	this->player_one = data->player_one;
	this->player_two = data->player_two;
	this->status = data->status;
	this->remaining = data->remaining;
}

SaltyOnline::SaltyOnline () : state_data (new StateData), old_state_data (new StateData) {
	this->h_internet = InternetOpenA ("InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	this->h_connect = InternetConnectA (this->h_internet, "saltybet.com", INTERNET_DEFAULT_HTTP_PORT, " ", " ", INTERNET_SERVICE_HTTP, 0, NULL);
}

SaltyOnline::~SaltyOnline () {}

bool SaltyOnline::Update () {
	HINTERNET hData = HttpOpenRequest (this->h_connect, "GET", "/state.json", NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);
	DWORD bytes = 0;
	int iterator = 11;
	char buffer [2048];
	
	old_state_data->Set (state_data);
	
	state_data->player_one.clear ();
	state_data->player_two.clear ();
	state_data->status.clear ();
	state_data->remaining.clear ();    
    
	HttpSendRequestA (hData, NULL, 0, NULL, 0);
	while (InternetReadFile (hData, buffer, 2048, &bytes) && bytes != 0) buffer [bytes] = 0;
    if (std::string (buffer).find ("</div>") != std::string::npos) return false;
	for (; buffer [iterator] != '\"'; iterator ++) state_data->player_one += buffer [iterator];
	iterator += 12;
	for (; buffer [iterator] != '\"'; iterator ++) state_data->player_two += buffer [iterator];
	iterator += 13;
	for (; buffer [iterator] != '\"'; iterator ++);
	iterator += 13;
	for (; buffer [iterator] != '\"'; iterator ++);
	iterator += 12;
	for (; buffer [iterator] != '\"'; iterator ++) state_data->status += buffer [iterator];
	iterator += 11;
	for (; buffer [iterator] != '\"'; iterator ++);
	iterator += 6;
	for (; buffer [iterator] != '\"'; iterator ++);
	iterator += 13;
	for (; buffer [iterator] != '\"'; iterator ++) state_data->remaining += buffer [iterator];
	
	if (old_state_data->player_one != state_data->player_one || old_state_data->player_two != state_data->player_two) this->flag_players = true;
	else this->flag_players = false;
	if (old_state_data->status == "locked" && state_data->status != old_state_data->status) {
		this->flag_match = true;
		if (state_data->status == "1") this->flag_player_won = true;
		else if (state_data->status == "2") this->flag_player_won = false;
	} else this->flag_match = false;
	if (state_data->remaining != old_state_data->remaining) {
		if (old_state_data->remaining == "Matchmaking mode will be activated after the next exhibition match!") {
			this->flag_mode_change = true;
			this->flag_mode = 1;
		} else if (old_state_data->remaining == "Tournament mode will be activated after the next match!") {
			this->flag_mode_change = true;
			this->flag_mode = 2;
		} else if (old_state_data->remaining == "FINAL ROUND! Stay tuned for exhibitions after the tournament!") {
			this->flag_mode_change = true;
			this->flag_mode = 3;
		} 
	} else this->flag_mode_change = false;
	if (state_data->remaining.find ("ournament") != std::string::npos && state_data->remaining.find ("match") != std::string::npos) this->flag_mode = 1;
	else if (state_data->remaining.find ("exhibition match") != std::string::npos) this->flag_mode = 3;
	
	delete [] buffer;
	InternetCloseHandle (hData);
    return true;
}

std::string SaltyOnline::GetPlayerOne () {
	return state_data->player_one;
}

std::string SaltyOnline::GetPlayerTwo () {
	return state_data->player_two;
}

bool SaltyOnline::OnWinner () {
	return this->flag_match;
}

bool SaltyOnline::OnNewMatch () {
	return this->flag_players;
}

bool SaltyOnline::OnNewMode () {
	return this->flag_mode_change;
}

char SaltyOnline::GetMode () {
	return this->flag_mode;
}

bool SaltyOnline::GetWinner () {
	return this->flag_player_won;
}