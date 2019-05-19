#ifndef _SALTYONLINE_H
#define _SALTYONLINE_H

#include <string>
#include <wininet.h>

struct StateData;

class SaltyOnline {
public:
	SaltyOnline ();
	~SaltyOnline ();
	
	bool Update ();
	
	bool OnWinner ();
	bool OnNewMatch ();
	bool OnNewMode ();
	char GetMode ();
	bool GetWinner ();
	
	std::string GetPlayerOne ();
	std::string GetPlayerTwo ();
private:
	StateData *old_state_data;
	StateData *state_data;
	bool flag_players;
	bool flag_player_won;
	bool flag_match;
	bool flag_mode_change;
	char flag_mode;
	HINTERNET h_internet;
	HINTERNET h_connect;
};

#endif /* _SALTYONLINE_H */