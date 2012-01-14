#include "disjointSet.h"
#include <iostream>
namespace cDisjointSet
{
	// This is the constructor, it will initialize all disjoint sets with -1
	DisjSets::DisjSets(int elements) : s(elements)
	{
		for(unsigned int i = 0; i < s.size(); ++i)
			s[i] = -1;
	}

	DisjSets::DisjSets(const DisjSets& source)
	{
		for(unsigned int i = 0; i < source.s.size(); ++i)
			s.push_back(source.s[i]);
	}

	void DisjSets::resize(int n)
	{
		s.resize(n);

		for(unsigned int i = 0; i < s.size(); ++i)
			s[i] = -1;
	}

	// Union of two disjoint sets
	// This will do union by size
	// root1 and root2 are root of set 1 and set 2 respectively.
	void DisjSets::unionSets( int set1, int set2 )
	{
		int root1 = find(set1);
		int root2 = find(set2);

		if( s[root1] <= s[root2] )
		{
			s[root1] += s[root2];
			s[root2] = root1;
		}
		else
		{
			s[root2] += s[root1];
			s[root1] = root2;
		}
	}

	// This recursive function will find the root of a given item
	int DisjSets::find(int x) const
	{
		if(s[x] < 0)
			return x;
		else 
			return find(s[x]);
	}

	// This is the find algorithm using path compression
	int DisjSets::find(int x)
	{
		if(s[x] < 0)
			return x;
		else
			return s[x] = find( s[x] );
	}

	// Prints everything
	void DisjSets::printAll() const
	{
		for(unsigned int i = 0; i < s.size(); ++i)
			std::cout << s[i] << "	";
		std::cout << "\n";
	}

	// Prints only members of the set
	void DisjSets::printSet(int item) const
	{
		int z = find(item);
		std::cout << "{ ";
		for(unsigned int i = 0; i < s.size(); ++i)
			if(find(i) == z)
				std::cout << i << ", ";
		std::cout << " }\n";
	}

	void DisjSets::remove(int x)
	{
		if(s[x] == -1)
			return;
		
		if(s[x] < -1)
		{	//The root will be removed
			int j = 0;
			while(s[j] != x) { ++j; }
			for(unsigned int i = j+1; i < s.size(); ++i)
				if(s[i] == x)
					s[i] = j;
			s[j] = s[x] + 1;
			s[x] = -1;
		}
		else
		{
			int z = find(x);
			s[z]++;
			for(unsigned int i = 0; i < s.size(); ++i)
				if(find(i) == z && i != z)
					s[i] = z;
		}
		s[x] = -1;
	}

	// Return true if there is only one set, false otherwise
	bool DisjSets::allConnected() const
	{
		int roots = 0;
		for(unsigned int i = 0; i < s.size(); ++i)
			if(s[i] < 0)
			{
				++roots;
				if(roots > 1) 
					return false;
			}
		
		return true;
	}
}