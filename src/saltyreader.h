#ifndef _SALTYREADER_H
#define _SALTYREADER_H

#include "saltyreference.h"

class SaltyReader {
public:
	SaltyReader ();
	~SaltyReader ();
	
	std::string GetMMR (std::string, std::string *);
	std::vector <CharacterRecord> GetAllStats ();
	bool FindMatchup (Matchup *, std::string, std::string);
	bool GetBetPercent (std::string, std::string, std::string *);
	void AddFight (std::string, std::string, bool, std::string *);
};

#endif /* _SALTYREADER_H */