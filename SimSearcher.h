#pragma once
#ifndef SIM
#define SIM
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
using namespace std;
const int SUCCESS = 0;
const int FAILURE = 1;

class SimSearcher
{
public:
	SimSearcher();
	~SimSearcher();

	vector<string> wordList;	//all words input  
	vector<unsigned> tooshortList; //words shorter than qGram, id in wordlist
	vector<vector<unsigned>> sortGramList;	//sorted vector List 
	unordered_map<string, unsigned> gramId;	//sorted ID List
	unordered_set<unsigned> longList; 
	unordered_set<unsigned> shortList; 
	vector<unsigned> possibleList;  
	vector<unsigned> countID; 
	
	int minLen;	//the min length of input word
	int maxLen;
	int qGram;
	
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
	
	void MakeGrams(const char* query);
	void MergeSkip(const char* query, unsigned th, int shortNum);
	void MergeOpt(unsigned start, unsigned end, unsigned th);
};
#endif 
