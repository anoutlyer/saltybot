#ifndef _SALTYREFERENCE_H
#define _SALTYREFERENCE_H

#include <sstream>
#include <vector>

template <typename T> std::string ToString (const T& n) {
	std::ostringstream oss;
	oss << n;
	return oss.str ();
}

union ShortUnion {
  short s;
  struct {
    char b1, b2;
  } byte;
};

struct Fight {
	unsigned char month;
	unsigned char day;
	unsigned char year;
	unsigned char winner;
	unsigned short one_games;
	unsigned short two_games;
	signed short one_mmr;
	signed short two_mmr;
	signed short one_mmr_change;
	signed short two_mmr_change;
	
	void Reverse ();
	std::string DateString ();
	std::string OneGamesString ();
	std::string TwoGamesString ();
	std::string OneMMRString ();
	std::string TwoMMRString ();
	std::string OneChangeString ();
	std::string TwoChangeString ();
	std::string ResultString ();
};

struct Matchup {
	std::string player_one;
	std::string player_two;
	std::vector <Fight> fight_log;
	
	std::string OneWins ();
	std::string TwoWins ();
};

struct CharacterRecord {
	std::string name;
	unsigned short games;
	unsigned short wins;
	unsigned short losses;
	signed short mmr;
};

#endif /* _SALTYREFERENCE_H */