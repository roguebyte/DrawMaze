//maze.cpp
#include "maze.h"

namespace cMaze
{
	maze::maze(int n, int m) : rows(n), cols(m) 
	{
		cels = new cDisjointSet::DisjSets(rows*cols);
	}

	// Copy constructor
	maze::maze(const maze& source) : rows(source.rows), cols(source.cols)
	{	
		for(unsigned int i = 0; i < source.walls.size(); ++i)
			walls.push_back(source.walls[i]);
		cels = new cDisjointSet::DisjSets(*source.cels);
	}

	void maze::createMaze()
	{
		walls.resize(rows*(cols-1) + cols*(rows-1));
		makeWalls();
		destroyWalls();
	}

	// This method will create the maze initially with walls everywhere.
	void maze::makeWalls()
	{
		int index;
		//Make horizontal walls
		for(int i = 0; i < rows; ++i)
		{
			for(int j = 0; j < cols-1; ++j)
			{
				index = (cols-1)*i + j;
				walls[index].x = cols*i + j;
				walls[index].y = cols*i + j + 1;
			}
		}

		int z = rows*(cols-1);		//This is the offset to begin on vert walls

		//Create vertical walls
		for(int i = 0; i < cols; ++i)
		{
			for(int j = 0; j < rows-1; ++j)
			{
				index = z + (rows-1) * i + j;
				walls[index].x = cols * j + i;
				walls[index].y = cols * j + i + cols;
			}
		}

	}

	// This method will randomly remove walls if the cells that the wall 
	// seperates are not already connected to each other. 
	void maze::destroyWalls()
	{
		int d, range = walls.size();
		srand ( time(NULL) );
		while(!(cels->allConnected()))
		{	// while you can't get to exit from entrance
			d = rand()%range;
			//std::cout << d << "\n";	// DEBUG PURPOSES
			if(cels->find(walls[d].x) != cels->find(walls[d].y))
			{	// if walls are in diff sets, unite cells into 1 set and remove wall
				cels->unionSets(walls[d].x, walls[d].y);
				walls.erase(walls.begin()+d);
				range--;
			}
		}
	}

	void maze::printMaze() 
	{
		for(unsigned int i = 0; i < walls.size(); ++i)
			walls[i].print();
	}

	int maze::numOfWalls() const
	{
		return walls.size();
	}

	// Given two cells, find out of there is a wall between them
	// Note: This can be considerably optimized.
	bool maze::isWall(int a, int b) const
	{
		wall tmp(a,b);
		for(unsigned int i = 0; i < walls.size(); ++i)
			if(walls[i].isEqual(tmp))
				return true;
		tmp.x = b; tmp.y = a;
		for(unsigned int i = 0; i < walls.size(); ++i)
			if(walls[i].isEqual(tmp))
				return true;
		return false;
	}
}