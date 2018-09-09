#include "SimSearcher.h"
#include<iostream>
using namespace std;



int main(int argc, char **argv)
{
	
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 3, edThreshold = 7;
	double jaccardThreshold = 0.2;
	
	searcher.createIndex("sample.txt", q);

//	searcher.searchJaccard("query", jaccardThreshold, resultJaccard);
	searcher.searchED("query", edThreshold, resultED);
	for(vector<pair<unsigned, unsigned>>::iterator it = resultED.begin(); it != resultED.end(); it++)
	{
		cout << it->first << " " << it->second << endl;	
	}
	return 0;

	for(vector<pair<unsigned, double>>::iterator it = resultJaccard.begin(); it != resultJaccard.end(); it++)
	{
		cout << it->first << " " << it->second << endl;	
	}
	return 0;
}

