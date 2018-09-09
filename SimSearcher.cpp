#include "SimSearcher.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <queue>
using namespace std;

const int MAX_LEN = 257;

SimSearcher::SimSearcher()
{
	wordList.clear();
	sortGramList.clear();
	gramId.clear();
	minLen = MAX_LEN;
}

SimSearcher::~SimSearcher()
{
}

int Min(int a, int b, int c)
{
	if(a <= b && a <= c)
		return a;
	if(b <= a && b <= c)
		return b;
	return c;
}

bool gramCompare(const pair<string, vector<unsigned>>& a, const pair<string, vector<unsigned>>& b)
{
	return a.second.size() < b.second.size();
}

struct heapCompare
{
	bool operator() (const pair<unsigned, unsigned>& a, const pair<unsigned, unsigned>& b)
	{
		return a.first > b.first;
	}
};

bool resCompare(const pair<unsigned, unsigned>& a, const pair<unsigned, unsigned>& b)
{
	return a.first < b.first;
}

//return ED of query and word
int getED(const char *query, const char *word)
{
	int lenQ = strlen(query);
	int lenW = strlen(word);

	static int dist[MAX_LEN][MAX_LEN];

	for(int i = 0; i <= lenQ; i++)
		dist[i][0] = i;
	for(int i = 0; i <= lenW; i++)
		dist[0][i] = i;
		
	for(int i = 1; i <= lenQ; i++)
	{
		for(int j = 1; j <= lenW; j++)
		{
			if(query[i-1] == word[j-1])
			{
				dist[i][j] = Min(dist[i-1][j-1], dist[i][j-1]+1, dist[i-1][j]+1);
			}
				
			else
				dist[i][j] = Min(dist[i-1][j-1]+1, dist[i][j-1]+1, dist[i-1][j]+1);	
			
		} 
	} 

	return dist[lenQ][lenW];
	
}

double getJaccard(const char *query, const char *word, int q)
{
	int intersect = 0;
	unordered_set<string> Qs, Ws;
	char s[260], t[260];
	strcpy(s, query);
	char *p1 = strtok(s, " ");
	while (p1 != NULL) {
		Qs.insert(p1);
		p1 = strtok(NULL, " ");
	}
	
	strcpy(t, word);
	char*p2 = strtok(t, " ");
	while(p2!=NULL)
	{
		Ws.insert(p2);
		p2 = strtok(NULL, " ");
	}
	
	for(unordered_set<string>::iterator it = Qs.begin(); it != Qs.end(); it++)
	{
		intersect += Ws.count(*it);
	}
//	cout << "QS: " << Qs.size() << " Ws: " << Ws.size() << " intersect: " << intersect << endl;
	return double(intersect)/(Qs.size()+Ws.size()-intersect);
	
/*	int lenQ = strlen(query);
	int lenW = strlen(word);
	string Q(query);
	string W(word);
	unordered_map<string, unsigned>	countGram1;
	unordered_map<string, unsigned>	countGram2;
	
	countGram1.clear();
	countGram2.clear();
		
	for(int i = 0; i < lenQ-q+1; i++)
	{
		string substr = Q.substr(i, q);
		if (countGram1.find(substr) == countGram1.end())
		{
			countGram1[substr] = 0;
		}
		else
		{
			int num = countGram1[substr];
			countGram1[substr]++;
			ostringstream sout;
			sout << substr << num;
			countGram1[sout.str()] = 0;
		}
	}
	
	for(int i = 0; i < lenW-q+1; i++)
	{
		
		string substr = W.substr(i, q);
		if (countGram2.find(substr) == countGram2.end())
		{
			countGram2[substr] = 0;
			if(countGram1.find(substr) != countGram1.end())
				intersect++;
		}
		else
		{
			int num = countGram2[substr];
			countGram2[substr]++;
			ostringstream sout;
			sout << substr << num;
			countGram2[sout.str()] = 0;
			if(countGram1.find(sout.str()) != countGram1.end())
				intersect++;
		}
	}
	int Gq = max(0, int(lenQ - q + 1)),  Gw = max(0, int(lenW - q + 1));
	return double(intersect) / (Gq + Gw - intersect);*/
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	
	unordered_map<string, vector<unsigned>> orGram;
	vector<pair<string, vector<unsigned>>> sortGram;
	unordered_map<string, unsigned>		countGram;	
	
	this->qGram = q;
	ifstream fin(filename);
	
	orGram.clear();
	sortGram.clear();
	countGram.clear();
	
	string  str;
	unsigned i = 0;
	while(getline(fin, str))
	{
	//	cout << i << endl; 
		wordList.push_back(str);
		int len = str.length();
		if(len < minLen)
			minLen = len;
			
		if(len < this->qGram)
		{
			tooshortList.push_back(i);
			continue;
		}
		
		int num = 0;
		countGram.clear(); 
		for(int j = 0; j < len-q+1; j++)
		{
			string substr = str.substr(j, this->qGram);
			//first find
			if (countGram.find(substr) == countGram.end())
			{
				orGram[substr].push_back(i);
				countGram[substr] = 0;
			}
			else
			{
				num = countGram[substr]++;
				ostringstream sout;
				sout << substr << num;
				orGram[sout.str()].push_back(i);
				countGram[sout.str()] = 0;
			}
		}
		i++; 
	}
	fin.close();
	countID.reserve(wordList.size());
	/* Sort the orGram by the length of the id list(vector) */
	for (unordered_map<string, vector<unsigned>>::iterator it(orGram.begin()); it != orGram.end(); ++it)
	{
		sortGram.push_back(*it);
	}
	sort(sortGram.begin(), sortGram.end(), gramCompare);

	/* Link the gram(string) with the vector index(unsigned) with unordered_map
	 * and store the final sorted gram list */
	for (vector<pair<string, vector<unsigned>>>::iterator it(sortGram.begin()); it != sortGram.end(); ++it)
	{
		sortGramList.push_back(it->second);
		gramId[it->first] = it - sortGram.begin();
	}
	maxLen = sortGramList.back().size();
	
	
	return SUCCESS;
}

void SimSearcher::MakeGrams(const char* query) //get possible List 
{
	
	int lenQ = strlen(query);
	string Q(query);
	unordered_map<string, unsigned> countGram;
	possibleList.clear();
	countGram.clear();
	
	for(int i = 0; i < lenQ-this->qGram+1; i++)
	{
		string substr = Q.substr(i, this->qGram);
		
		if(countGram.find(substr) == countGram.end())
		{
			countGram[substr] = 0;
			unordered_map<string, unsigned>::iterator it = this->gramId.find(substr);
			if(it != gramId.end())
			{
				this->possibleList.push_back(it->second);
			}	
		}
		else
		{
			int num = countGram[substr];
			countGram[substr]++;
			ostringstream sout;
			sout << substr << num;
			countGram[sout.str()] = 0;
			unordered_map<string, unsigned>::iterator it = this->gramId.find(sout.str());
			if(it != this->gramId.end())
			{
				this->possibleList.push_back(it->second);
			}	
		}
	} 
	sort(this->possibleList.begin(), this->possibleList.end());
}

void SimSearcher::MergeSkip(const char* query, unsigned th, int shortNum)
{

	vector<unsigned>  Pos;		// start position of each list
	vector<pair<unsigned, unsigned>> poppedLists;
	priority_queue<pair<unsigned, unsigned>, vector<pair<unsigned, unsigned>>, heapCompare> heap;
	poppedLists.clear();
	shortList.clear();
	
	unsigned topV;
	Pos.clear();
	Pos.resize(sortGramList.size());

	int num = 0;
	/* Initialize the heap */
	for (int i = 0; i < shortNum; i++)
		heap.push(make_pair(sortGramList[possibleList[i]].front(), possibleList[i]));

	/* MergeSkip */
	while (!heap.empty())
	{	
		topV = heap.top().first;
		num = 0;
		poppedLists.clear();
		while (!heap.empty() && heap.top().first == topV)
		{
			num++;
			poppedLists.push_back(heap.top());
			heap.pop();
		}
	
		if (num >= th)
		{
			shortList.insert(topV);
			countID[topV] = num;
			for (vector<pair<unsigned, unsigned>>::iterator it(poppedLists.begin()); it != poppedLists.end(); ++it)
			{
				vector<unsigned> &cList = sortGramList[it->second];	
				if (++Pos[it->second] < cList.size())
				{
					heap.push(make_pair(cList[Pos[it->second]], it->second));
				}
			}
		
		}
		else
		{
			for (int i = 0; !heap.empty() && i < th - 1 - num; ++i)
			{
				poppedLists.push_back(heap.top());
				heap.pop();
			}
			if (heap.empty())
			{
				break;
			}
			
			topV = heap.top().first;
			for (vector<pair<unsigned, unsigned>>::iterator it(poppedLists.begin()); it != poppedLists.end(); ++it)
			{
				vector<unsigned> &cList = sortGramList[it->second];
				
				vector<unsigned>::iterator it2 = lower_bound(cList.begin(), cList.end(), topV);
				if (it2 != cList.end())
				{
					heap.push(make_pair(*it2, it->second));
				}
				Pos[it->second] = it2 - cList.begin();
				
			}
		}
		
	}
	return;
}

void SimSearcher::MergeOpt(unsigned start, unsigned end, unsigned th)
{
	longList.clear();
	/* MergeOpt */
	for (unordered_set<unsigned>::iterator it(shortList.begin()); it != shortList.end(); it++)
	{
		for (int i = start; i < end; i++)
		{
			if (binary_search(sortGramList[possibleList[i]].begin(), sortGramList[possibleList[i]].end(), *it))
			{
				countID[*it]++;
			}
		}
		if (countID[*it] >= th)
		{
			longList.insert(*it);
		}
	}
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
//	cout << "Jaccard" << endl;
	result.clear();
	
	int Gq = max(0, int(strlen(query) - qGram + 1));
	int Gs = max(0, int(minLen - qGram + 1));
	int T = (int)max(threshold * Gq, (Gq + Gs) * threshold / (1 + threshold));
	const double u = 0.0085;
	
	if (T > 0)
	{
		unsigned L = T / (u * log10(double(maxLen)) + 1);		// important parameter in the DivideSkip algorithm
		int len = strlen(query);
		
		if (len > this->qGram)
		{
			MakeGrams(query);
			
			int shortNum = possibleList.size() - int(L);

			if (shortNum > 0)
			{	
			//	cout << "T:" << T << " L:" << L << " shortnum:" << shortNum << endl;
				MergeSkip(query, T - L, shortNum);
			//	return 0;
				MergeOpt(shortNum, possibleList.size(), L);
				
				/* Check the candidates and 'empty'(very short) words */
				double jac = 0.0;
				for (unordered_set<unsigned>::iterator it(longList.begin()); it != longList.end(); it++)
				{
					jac = getJaccard(query, wordList[*it].c_str(), this->qGram);
					if (jac >= threshold)
					{
						result.push_back(make_pair(*it,jac));
					}
				}
				sort(result.begin(), result.end(), resCompare);
				return SUCCESS;	

			} 
		}
	} 
	/* Just check it one by one */
	double jac = 0.0;
	unsigned wordLen(wordList.size());
	for (int i = 0; i < wordLen; i++)
	{
		jac = getJaccard(query, wordList[i].c_str(), this->qGram);
		if (jac >= threshold)
		{
			result.push_back(make_pair(i, jac));
		}
	}
	
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	
	int T = strlen(query) - qGram + 1 - threshold * qGram;
	const double u = 0.0085;

	/* Using MergeSkip algorithm */
	if (T > 0)
	{
		unsigned L = T / (u * log10(double(maxLen)) + 1);		// important parameter in the DivideSkip algorithm

		int len = strlen(query);
		/* Parse the grams if the query string is long enough */
		if (len > qGram && len >= 5)
		{
			MakeGrams(query);

			int shortNum = possibleList.size() - int(L);

			if (shortNum > 0)
			{
				/* Use MergeSkip algorithm on L_short set, if not empty */	
				MergeSkip(query, T - L, shortNum);
				
				/* Use MergeOpt algorithm on L_long set. */
				MergeOpt(shortNum, possibleList.size(), T);
				
				/* Check the candidates and 'empty'(very short) words */
				unsigned ed(0);
				for (unordered_set<unsigned>::iterator it(longList.begin()); it != longList.end(); it++)
				{
					ed = getED(query, wordList[*it].c_str());
					if (ed <= threshold)
					{
						result.push_back(make_pair(*it, ed));
					}
				}
				for (vector<unsigned>::iterator it(tooshortList.begin()); it != tooshortList.end(); ++it)
				{
					ed = getED(query, wordList[*it].c_str());
					if (ed <= threshold)
					{
						result.push_back(make_pair(*it, ed));
					}
				}
				sort(result.begin(), result.end(), resCompare);	
				return SUCCESS;
			} 
		}
	}
	unsigned ed(0), wordLen(wordList.size());
	for (int i = 0; i < wordLen; ++i)
	{
		ed = getED(query, wordList[i].c_str());
		if (ed <= threshold)
		{
			result.push_back(make_pair(i, ed));
		}
	}

	return SUCCESS;
}

