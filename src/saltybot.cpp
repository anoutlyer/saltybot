#include <stdio.h>
#include "saltyonline.h"
#include "saltyreader.h"

SaltyOnline *salty_online;
SaltyReader *salty_reader;

int main () {
	Matchup matchup;
	std::string one_games, two_games;
	std::string one_wins, two_wins;
	std::string percent = "";
    salty_online = new SaltyOnline ();
	salty_reader = new SaltyReader ();
	char flag_mode = 3;
	bool flag_mode_change = false;
	bool bet_choice;
	for (DWORD counter = 0; true; counter ++) {
		if (counter % 100000 == 0) {
			if (salty_online->Update ()) {
                if (salty_online->OnNewMatch ()) {
                    matchup = Matchup ();
                    printf ("New Match!\n");
                    printf ("Player 1 - %s\n", salty_online->GetPlayerOne ().c_str ());
                    printf ("Player 2 - %s\n", salty_online->GetPlayerTwo ().c_str ());
                    salty_reader->FindMatchup (&matchup, salty_online->GetPlayerOne (), salty_online->GetPlayerTwo ());
                    printf ("ELO         %s : %s\n", salty_reader->GetMMR (salty_online->GetPlayerOne (), &one_games).c_str (), salty_reader->GetMMR (salty_online->GetPlayerTwo (), &two_games).c_str ());
                    printf ("Games       %s : %s\n", one_games.c_str (), two_games.c_str ());
                    printf ("Record      %s : %s\n", matchup.OneWins ().c_str (), matchup.TwoWins ().c_str ());
                    bet_choice = salty_reader->GetBetPercent (salty_online->GetPlayerOne (), salty_online->GetPlayerTwo (), &percent);
                    printf ("Outcome Percentage: %s\n", percent.c_str ());
                    if (flag_mode == 3) printf ("\n");
                } else if (salty_online->OnWinner () && flag_mode != 3) {
                    std::string winner = salty_online->GetWinner () ?  salty_online->GetPlayerOne () : salty_online->GetPlayerTwo ();
                    std::string winner_mmr;
                    printf (std::string ("Match over, " + winner + " wins!\n").c_str ());
                    salty_reader->AddFight (salty_online->GetPlayerOne (), salty_online->GetPlayerTwo (), salty_online->GetWinner (), &winner_mmr);
                    printf ("New ELO      %s : %s\n", salty_reader->GetMMR (salty_online->GetPlayerOne (), &one_games).c_str (), salty_reader->GetMMR (salty_online->GetPlayerTwo (), &two_games).c_str ());
                    printf ("New Games    %s : %s\n", one_games.c_str (), two_games.c_str ());
                    salty_reader->FindMatchup (&matchup, salty_online->GetPlayerOne (), salty_online->GetPlayerTwo ());
                    printf ("New Record   %s : %s\n", matchup.OneWins ().c_str (), matchup.TwoWins ().c_str ()); 
                    printf (std::string ("(" + winner + " gained " + winner_mmr + " MMR)\n\n").c_str ()); 
                }
                flag_mode = salty_online->GetMode ();
                flag_mode_change = salty_online->OnNewMode ();
                if (flag_mode_change) {
                    switch (flag_mode) {
                        case 1: printf ("MATCHMAKING\n\n");
                        break;
                        case 2: printf ("TOURNAMENT\n\n");
                        break;
                        case 3: printf ("EXHIBITIONS\n\n");
                        break;
                    }
                }
                one_games.clear ();
                two_games.clear ();	
                counter = 0;
            } else {
                printf ("Saltybet servers are down. Attempting to reconnect...\n");
            }
		}
	}
	return 0;
}