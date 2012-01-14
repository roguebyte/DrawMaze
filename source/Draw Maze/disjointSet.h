//disjointSet.h
#include <vector>
#ifndef DISJOINTSET
#define DISJOINTSET

namespace cDisjointSet
{
	class DisjSets
	{
	public:
		explicit DisjSets(int elements);
		DisjSets(const DisjSets& source);
		int find(int x) const;
		int find(int x);
		void unionSets( int root1, int root2);
		void printSet(int item) const;
		void remove(int x);
		void printAll() const;
		void resize(int n);
		bool allConnected() const;
		inline int numOfElements() const { return s.size(); }

	private:
		std::vector<int> s;
	};
}
#endif