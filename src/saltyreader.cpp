#include <fstream>
#include <sstream>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "saltyreader.h"

void ComputeMMR (bool winner, signed short one_mmr, signed short two_mmr, unsigned short one_games, unsigned short two_games, signed short *new_one_mmr, signed short *new_two_mmr) {
	signed short max = one_mmr > two_mmr ? one_mmr : two_mmr;
	signed short min = one_mmr <= two_mmr ? one_mmr : two_mmr;
	signed short new_one = 0;
	signed short new_two = 0;
    float max_diff;
	float diff = max_diff = (max - min) * 6;
	if (diff > 400) diff = 400;
	if ((winner && max == one_mmr) || (!winner && max == two_mmr)) {
		new_one = (signed short)((1 - diff / 400) * (1 + 7 / ((float) one_games)) * 3);
		new_two = (signed short)((1 - diff / 400) * (1 + 7 / ((float) two_games)) * 3);
	} else if ((winner && max == two_mmr) || (!winner && max == one_mmr)) {
		new_one = (signed short)((1 + max_diff / 400) * (1 + 7 / ((float) one_games)) * 3); 
		new_two = (signed short)((1 + max_diff / 400) * (1 + 7 / ((float) two_games)) * 3);
	}
	new_one *= winner ? 1 : -1;
	new_two *= winner ? -1 : 1;
	new_one_mmr [0] = one_mmr + new_one;
	new_two_mmr [0] = two_mmr + new_two;
}

SaltyReader::SaltyReader () {}

SaltyReader::~SaltyReader () {}

std::string SaltyReader::GetMMR (std::string player, std::string *games) {
	std::fstream reader ("mmr", std::ios::in | std::ios::ate | std::ios::binary);
	std::streampos size;
	std::string name;
	char *buffer;
	if (reader.fail ()) {
		games [0] = "0";
		return "0";
	} else {
		size = reader.tellg ();
		buffer = new char [size];
		reader.seekg (0, std::ios::beg);
		reader.read (buffer, size);
		reader.close ();
		
		for (int i = 0; i < (int) size;) {
			for (; buffer [i] != 0; i ++) name += buffer [i];
			if (name == player) {
				ShortUnion short_union_mmr, short_union_games;
				short_union_games.byte.b1 = buffer [i + 2];
				short_union_games.byte.b2 = buffer [i + 1];
				games [0] = ToString (short_union_games.s);
				short_union_mmr.byte.b1 = buffer [i + 4];
				short_union_mmr.byte.b2 = buffer [i + 3];
				return ToString (short_union_mmr.s);
            }
            i += 6;
			name.clear ();
		}
		delete [] buffer;
		games [0] = "0";
		return "0";
	}
}

std::vector <CharacterRecord> SaltyReader::GetAllStats () {
	std::fstream reader ("mmr", std::ios::in | std::ios::ate | std::ios::binary);
	std::streampos size;
	std::string name;
	std::vector <CharacterRecord> stats;
	CharacterRecord record;
	char *buffer;
	if (reader.fail ()) {
		return stats;
	} else {
		size = reader.tellg ();
		buffer = new char [size];
		reader.seekg (0, std::ios::beg);
		reader.read (buffer, size);
		reader.close ();
		
		ShortUnion short_union_games;
		ShortUnion short_union_mmr;
		for (int i = 0; i < (int) size; i ++) {
			record = CharacterRecord ();
			for (; buffer [i] != 0; i ++) name += buffer [i];
			record.name = name;
			i ++;
			short_union_games.byte.b2 = buffer [i ++];
			short_union_games.byte.b1 = buffer [i ++];
			record.games = (unsigned short) short_union_games.s;
			short_union_mmr.byte.b2 = buffer [i ++];
			short_union_mmr.byte.b1 = buffer [i ++];
			record.mmr = (signed short) short_union_mmr.s;
			name.clear ();
			stats.push_back (record);
		}
		
		delete [] buffer;
		return stats;
	}
}

bool SaltyReader::FindMatchup (Matchup *out_matchup, std::string player_one, std::string player_two) {
	std::string file_name = "matchups/" + player_one + "$" + player_two + ".sbm";
	std::fstream reader (file_name.c_str (), std::ios::in | std::ios::ate | std::ios::binary);
	Matchup matchup;
	bool reverse = false;
	
	if (reader.fail ()) {
		file_name = "matchups/" + player_two + "$" + player_one + ".sbm";
		reader.open (file_name.c_str (), std::ios::in | std::ios::ate | std::ios::binary);
		if (reader.fail ()) {
			out_matchup [0] = Matchup ();
			return false;
		}
		else reverse = true;
	}
	
	Fight fight;
	std::streampos size;
	char *buffer;
	
	size = reader.tellg ();
	buffer = new char [(int) size];
	reader.seekg (0, std::ios::beg);
	reader.read (buffer, size);
	
	matchup.player_one = player_one;
	matchup.player_two = player_two;
	
	ShortUnion one_games, two_games;
	ShortUnion one_mmr, two_mmr;
	ShortUnion one_mmr_change, two_mmr_change;
	for (int i = 0; i < (int) size; i ++) {
		fight = Fight ();
		
		fight.month = (unsigned char) buffer [i];
		fight.day = (unsigned char) buffer [i + 1];
		fight.year = (unsigned char) buffer [i + 2];
		fight.winner = (unsigned char) buffer [i + 3];
		
		one_games.byte.b2 = buffer [i + 4];
		one_games.byte.b1 = buffer [i + 5];
		two_games.byte.b2 = buffer [i + 6];
		two_games.byte.b1 = buffer [i + 7];
		one_mmr.byte.b2 = buffer [i + 8];
		one_mmr.byte.b1 = buffer [i + 9];
		two_mmr.byte.b2 = buffer [i + 10];
		two_mmr.byte.b1 = buffer [i + 11];
		one_mmr_change.byte.b2 = buffer [i + 12];
		one_mmr_change.byte.b1 = buffer [i + 13];
		two_mmr_change.byte.b2 = buffer [i + 14];
		two_mmr_change.byte.b1 = buffer [i + 15];
		
		fight.one_games = (unsigned short) one_games.s;
		fight.two_games = (unsigned short) two_games.s;
		fight.one_mmr = (signed short) one_mmr.s;
		fight.two_mmr = (signed short) two_mmr.s;
		fight.one_mmr_change = (signed short) one_mmr_change.s;
		fight.two_mmr_change = (signed short) two_mmr_change.s;
		if (reverse) fight.Reverse ();
		matchup.fight_log.push_back (fight);
		i += 15;
	}
	
	out_matchup [0] = matchup;
	return true;
}

bool SaltyReader::GetBetPercent (std::string player_one, std::string player_two, std::string *percent) {
	std::fstream reader ("mmr", std::ios::in | std::ios::ate | std::ios::binary);
	std::streampos size;
	std::string name;
	signed short one_mmr = 0;
	signed short two_mmr = 0;
	signed short diff = 0;
	float percent_float;
	int random;
	bool player;
	char *buffer;
	if (reader.fail ()) {
		percent [0] = "0%";
		return true;
	} else {
		size = reader.tellg ();
		buffer = new char [size];
		reader.seekg (0, std::ios::beg);
		reader.read (buffer, size);
		reader.close ();
		
		for (int i = 0; i < (int) size; i ++) {
			ShortUnion short_union_mmr;
			for (; buffer [i] != 0; i ++) name += buffer [i];
			if (name == player_one) {
				short_union_mmr.byte.b1 = buffer [i + 4];
				short_union_mmr.byte.b2 = buffer [i + 3];
				one_mmr = short_union_mmr.s;
			} else if (name == player_two) {
				short_union_mmr.byte.b1 = buffer [i + 4];
				short_union_mmr.byte.b2 = buffer [i + 3];
				two_mmr = short_union_mmr.s;
			}
			name.clear ();
		}
		
		diff = one_mmr - two_mmr;
		if (diff > 80) diff = 80;
		else if (diff < -80) diff = -80;
		percent_float = 100 * ((80 + (float) diff) / 160);
		//srand (time (NULL));
		//random = rand () % 100;
		player = one_mmr > two_mmr;
		if (diff == 0) percent [0] = "50%";
		else percent [0] = player ? ToString ((int)(percent_float)) + "%" : ToString ((int)(100 - percent_float)) + "%";
		return player;
	}
}

void SaltyReader::AddFight (std::string player_one, std::string player_two, bool winner, std::string *winner_mmr) {
	DIR *dir;
	dirent *ent;
	time_t current;
	tm *time_info;
	std::fstream reader ("mmr", std::ios::in | std::ios::ate | std::ios::binary);
	std::streampos size;
	std::string file_name;
	signed short one_mmr = 0;
	signed short two_mmr = 0;
	signed short mmr_gain_winner = 0;
	char *buffer, *old_buffer;
	bool new_matchup = true;
	Fight new_fight = Fight ();
	if (reader.fail ()) {
		buffer = new char [player_one.size () + player_two.size () + 12];
		ComputeMMR (winner, 0, 0, 1, 1, &new_fight.one_mmr, &new_fight.two_mmr);
		new_fight.one_mmr_change = new_fight.one_mmr;
		new_fight.two_mmr_change = new_fight.two_mmr;
		new_fight.one_games = new_fight.two_games = 1;
		mmr_gain_winner = winner ? new_fight.one_mmr : new_fight.two_mmr;
		winner_mmr [0] = winner ? ToString (new_fight.one_mmr) : ToString (new_fight.two_mmr);
		
		int i = 0;
		int j;
		for (; i < player_one.size (); i ++) buffer [i] = player_one [i];
		buffer [i ++] = 0;
		buffer [i ++] = 0;
		buffer [i ++] = 1;
		buffer [i ++] = (new_fight.one_mmr >> 8) & 0xFF;
		buffer [i ++] = new_fight.one_mmr & 0xFF;
		buffer [i ++] = 0;
		for (j = i; (j - i) < player_two.size (); j ++) buffer [j] = player_two [j - i];
		buffer [j ++] = 0;
		buffer [j ++] = 0;
		buffer [j ++] = 1;
		buffer [j ++] = (new_fight.two_mmr >> 8) & 0xFF;
		buffer [j ++] = new_fight.two_mmr & 0xFF;
		buffer [j ++] = 0;
		
		std::ofstream writer ("mmr", std::ofstream::out | std::ofstream::binary);
		writer.write (buffer, player_one.size () + player_two.size () + 12);
	} else {
		unsigned short one_games = 0;
		unsigned short two_games = 0;
		bool one_reg = false;
		bool two_reg = false;
		int mmr_pos_one = 0;
		int mmr_pos_two = 0;
		int buffer_len = 0;
		std::string name;
		
		size = reader.tellg ();
		old_buffer = new char [size];
		reader.seekg (0, std::ios::beg);
		reader.read (old_buffer, size);
		reader.close ();
		remove ("/mmr");
		
		for (int i = 0; i < (int) size;) {
			for (; old_buffer [i] != 0; i ++) name += old_buffer [i];
			if (name == player_one) {
				one_games = (signed short)(old_buffer [i + 2] | (old_buffer [i + 1] << 8));
				one_mmr = (signed short)(old_buffer [i + 4] | (old_buffer [i + 3] << 8));
				mmr_pos_one = i;
				one_reg = true;
			} else if (name == player_two) {
				two_games = (signed short)(old_buffer [i + 2] | (old_buffer [i + 1] << 8));
				two_mmr = (signed short)(old_buffer [i + 4] | (old_buffer [i + 3] << 8));
				mmr_pos_two = i;
				two_reg = true;
			}
			i += 6;
			name.clear ();
		}
		one_games ++;
		two_games ++;
		
		if (one_reg && two_reg) buffer_len = (int) size;
		else if (one_reg) buffer_len = (int) size + player_two.size () + 6;
		else if (two_reg) buffer_len = (int) size + player_one.size () + 6;
		else buffer_len = (int) size + player_one.size () + player_two.size () + 12;
		
		buffer = new char [buffer_len];
		for (int i = 0; i < (int) size; i ++) buffer [i] = old_buffer [i];
		delete [] old_buffer;
		
		mmr_gain_winner = winner ? one_mmr : two_mmr;
		new_fight.one_mmr_change = one_mmr;
		new_fight.two_mmr_change = two_mmr;
		ComputeMMR (winner, one_mmr, two_mmr, one_games, two_games, &one_mmr, &two_mmr);
		mmr_gain_winner = winner ? (one_mmr - mmr_gain_winner) : (two_mmr - mmr_gain_winner);
		new_fight.one_mmr_change = one_mmr - new_fight.one_mmr_change;
		new_fight.two_mmr_change = two_mmr - new_fight.two_mmr_change;
		new_fight.one_games = one_games;
		new_fight.two_games = two_games;
		new_fight.one_mmr = one_mmr;
		new_fight.two_mmr = two_mmr;
		
		if (one_reg && two_reg) {
			buffer [mmr_pos_one + 1] = (one_games >> 8) & 0xFF;
			buffer [mmr_pos_one + 2] = one_games & 0xFF;
			buffer [mmr_pos_one + 3] = (one_mmr >> 8) & 0xFF;
			buffer [mmr_pos_one + 4] = one_mmr & 0xFF;
			
			buffer [mmr_pos_two + 1] = (two_games >> 8) & 0xFF;
			buffer [mmr_pos_two + 2] = two_games & 0xFF;
			buffer [mmr_pos_two + 3] = (two_mmr >> 8) & 0xFF;
			buffer [mmr_pos_two + 4] = two_mmr & 0xFF;
		} else if (one_reg) {
			buffer [mmr_pos_one + 1] = (one_games >> 8) & 0xFF;
			buffer [mmr_pos_one + 2] = one_games & 0xFF;
			buffer [mmr_pos_one + 3] = (one_mmr >> 8) & 0xFF;
			buffer [mmr_pos_one + 4] = one_mmr & 0xFF;
			
			for (int i = 0; i < player_two.size (); i ++) buffer [(int) size + i] = player_two [i];
			buffer [(int) size + player_two.size ()] = 0;
			buffer [(int) size + player_two.size () + 1] = (two_games >> 8) & 0xFF;
			buffer [(int) size + player_two.size () + 2] = two_games & 0xFF;
			buffer [(int) size + player_two.size () + 3] = (two_mmr >> 8) & 0xFF;
			buffer [(int) size + player_two.size () + 4] = two_mmr & 0xFF;
			buffer [(int) size + player_two.size () + 5] = 0;
		} else if (two_reg) {
			buffer [mmr_pos_two + 1] = (two_games >> 8) & 0xFF;
			buffer [mmr_pos_two + 2] = two_games & 0xFF;
			buffer [mmr_pos_two + 3] = (two_mmr >> 8) & 0xFF;
			buffer [mmr_pos_two + 4] = two_mmr & 0xFF;
			
			for (int i = 0; i < player_one.size (); i ++) buffer [(int) size + i] = player_one [i];
			buffer [(int) size + player_one.size ()] = 0;
			buffer [(int) size + player_one.size () + 1] = (one_games >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + 2] = one_games & 0xFF;
			buffer [(int) size + player_one.size () + 3] = (one_mmr >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + 4] = one_mmr & 0xFF;
			buffer [(int) size + player_one.size () + 5] = 0;
		} else {
			for (int i = 0; i < player_one.size (); i ++) buffer [(int) size + i] = player_one [i];
			buffer [(int) size + player_one.size ()] = 0;
			buffer [(int) size + player_one.size () + 1] = (one_games >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + 2] = one_games & 0xFF;
			buffer [(int) size + player_one.size () + 3] = (one_mmr >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + 4] = one_mmr & 0xFF;
			buffer [(int) size + player_one.size () + 5] = 0;
			
			for (int i = 0; i < player_two.size (); i ++) buffer [(int) size + player_one.size () + 6 + i] = player_two [i];
			buffer [(int) size + player_one.size () + player_two.size () + 6] = 0;
			buffer [(int) size + player_one.size () + player_two.size () + 7] = (two_games >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + player_two.size () + 8] = two_games & 0xFF;
			buffer [(int) size + player_one.size () + player_two.size () + 9] = (two_mmr >> 8) & 0xFF;
			buffer [(int) size + player_one.size () + player_two.size () + 10] = two_mmr & 0xFF;
			buffer [(int) size + player_one.size () + player_two.size () + 11] = 0;
		}
		
		std::ofstream writer ("mmr", std::ofstream::out | std::ofstream::binary);
		writer.write (buffer, buffer_len);
		
		delete [] buffer;
		
		winner_mmr [0] = ToString (mmr_gain_winner);
	}
	dir = opendir ("matchups");
	file_name = player_one + "$" + player_two + ".sbm";
	time (&current);
	time_info = localtime (&current);
	if (dir == NULL) {
		CreateDirectory ("matchups", NULL);
		dir = opendir ("matchups");
	}
	for (int i = 0; (ent = readdir (dir)) != NULL; i ++) {
		if (i > 1 && file_name == ent->d_name) new_matchup = false;
	}
	file_name.insert (0, "matchups/");
	if (new_matchup) {
		size = 0;
		buffer = new char [16];
	} else {
		std::fstream matchup (file_name.c_str (), std::ios::in | std::ios::ate | std::ios::binary);
		size = matchup.tellg ();
		buffer = new char [(int) size + 16];
		matchup.seekg (0, std::ios::beg);
		matchup.read (buffer, size);
		remove (file_name.c_str ());
	}
	buffer [(int) size] = (char)(time_info->tm_mon + 1);
	buffer [(int) size + 1] = (char) time_info->tm_mday;
	buffer [(int) size + 2] = (char)(time_info->tm_year - 116);
	buffer [(int) size + 3] = winner ? 1 : 0;
	buffer [(int) size + 4] = (new_fight.one_games >> 8) & 0xFF;
	buffer [(int) size + 5] = new_fight.one_games & 0xFF;
	buffer [(int) size + 6] = (new_fight.two_games >> 8) & 0xFF;
	buffer [(int) size + 7] = new_fight.two_games & 0xFF;
	buffer [(int) size + 8] = (new_fight.one_mmr >> 8) & 0xFF;
	buffer [(int) size + 9] = new_fight.one_mmr & 0xFF;
	buffer [(int) size + 10] = (new_fight.two_mmr >> 8) & 0xFF;
	buffer [(int) size + 11] = new_fight.two_mmr & 0xFF;
	buffer [(int) size + 12] = (new_fight.one_mmr_change >> 8) & 0xFF;
	buffer [(int) size + 13] = new_fight.one_mmr_change & 0xFF;
	buffer [(int) size + 14] = (new_fight.two_mmr_change >> 8) & 0xFF;
	buffer [(int) size + 15] = new_fight.two_mmr_change & 0xFF;
	
	std::ofstream writer (file_name.c_str (), std::ofstream::out | std::ofstream::binary);
	writer.write (buffer, (int) size + 16);
}