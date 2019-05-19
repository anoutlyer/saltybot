#include <algorithm>

#include "saltyreference.h"

void Fight::Reverse () {
	winner = 1 - winner;
	std::swap (one_games, two_games);
	std::swap (one_mmr, one_mmr);
	std::swap (one_mmr_change, one_mmr_change);
}

std::string Fight::DateString () {
	std::string string_month = (int) month < 10 ? "0" + ToString ((int) month) : ToString ((int) month);
	std::string string_day = (int) day < 10 ? "0" + ToString ((int) day) : ToString ((int) day);
	return string_month + "/" + string_day + "/" + ToString (2016 + year);
}

std::string Fight::OneGamesString () {
	return ToString ((int) one_games);
}

std::string Fight::TwoGamesString () {
	return ToString ((int) two_games);
}

std::string Fight::OneMMRString () {
	return ToString ((int) one_mmr);
}

std::string Fight::TwoMMRString () {
	return ToString ((int) two_mmr);
}

std::string Fight::OneChangeString () {
	return ToString ((int) one_mmr_change);
}

std::string Fight::TwoChangeString () {
	return ToString ((int) two_mmr_change);
}

std::string Fight::ResultString () {
	return winner == 0 ? "0 : 1" : "1 : 0";
}

std::string Matchup::OneWins () {
	int wins = 0;
	for (int i = 0; i < fight_log.size (); i ++) {
		if (fight_log.at (i).winner == 1) wins ++;
	}
	return ToString (wins);
}

std::string Matchup::TwoWins () {
	int wins = 0;
	for (int i = 0; i < fight_log.size (); i ++) {
		if (fight_log.at (i).winner == 0) wins ++;
	}
	return ToString (wins);
}