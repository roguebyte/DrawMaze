/*  A Maze generation and traversal program
**	by Ernesto Pavon.
**  runMaze.cpp
*/
#include <iostream>
#include <conio.h>
#include <cmath>
#include "disjointSet.h"
#include "maze.h"
#include <cstdlib>
#include <Windows.h>
#include <glut.h>		// Depending on installation, may be gl/glut.h instead 
#include <list>
using namespace cMaze;
using namespace std;

const double HEIGHT = 400;
const double WIDTH = 600;
const float PI = 3.141592;

unsigned int rows, cols;
GLfloat horDiv, vertDiv;
maze* myMaze;

struct cell
{
	int node, backPtr, gCost, hCost;
	bool inClosed;
	cell() : node(-1) {}
	cell(int a, int b) :node(a), backPtr(b), gCost(0), hCost(0), inClosed(false) {}
	int fCost() const { return gCost + hCost; }
	cell& operator =(const cell& b) 
	{
		node = b.node;	backPtr = b.backPtr; gCost = b.gCost; 
		hCost = b.hCost;
		return *this;
	}
};
/* This is the list for A* */
std::list<cell> aslist;
list<cell>::iterator it;		// Beginning of closed list of A* algorithm

/******* A* Algorithm for Path Finding ******
** Nodes are represented using the cell ADT, and range from 0 to rows*cols-1
** Ex:
**  ____________
** |_0_|_1_|_2_|
** |_3_|_4_|_5_|
** |_6_|_7_|_8_|
**  For a 9-node maze
**
** This algorithm will use a single array instead of the open and closed list 
** usually associated with A* 
** I will use a selection algorithm to have the minimum H value readily 
** available. The Selection algorithm runs in worst case O(n) time. 
** Correctness: It's easy to see that a node cannot be at both lists at once,
** therefore, the array can be recycled. 
*************************/

/* Heuristic
** get H cost for node using the Manhattan method 
** Given we can only move horizontally or vertically, this
** makes for a very fast Heuristic.
*/
int gethCost(int node)
{
	int endOfRow = (node/cols) * cols + (cols-1);
	return (endOfRow - node) + ((cols * rows - 1) - endOfRow) / cols;
}


/* Check if node n is in open list and return its iterator if it does */
list<cell>::iterator inoList(list<cell>& l, std::list<cell>::iterator it, int n)
{
	list<cell>::iterator i;
	for(i = l.begin(); i != it; ++i)
		if(i->node == n)
			return i;
	return l.end();
}

list<cell>::iterator incList(list<cell>& l, list<cell>::iterator it, int n)
{
	list<cell>::iterator i;
	for(i = it; i != l.end(); ++i)
		if(i->node == n)
			return i;
	return l.end();
}

/* Purely for debugging purposes */
void showList(list<cell>& l)
{
	for(list<cell>::iterator i = l.begin(); i != l.end(); ++i)
		cout << i->node << "	";
	cout << "\n\n";
}

/* Add newNode to the open list if it's not already there */
void addToOpenList(std::list<cell>& aslist, const std::list<cell>::iterator& it, int newNode)
{
	std::list<cell>::iterator curr;
	cell tmp;

	/* Check if it's in closed list */
	for(curr = it; curr != aslist.end(); ++curr)
		if(curr->node == newNode)
			return;

	/* If already in open list, check gCost */
	if((curr = inoList(aslist,it, newNode)) != aslist.end())
	{
		if(curr->gCost > it->gCost+1)
		{
			curr->backPtr = it->node;
			curr->gCost = it->gCost + 1;
		}
	}
	else
	{
		tmp.node = newNode;
		tmp.backPtr = it->node;
		tmp.gCost = it->gCost + 1;
		tmp.hCost = gethCost(tmp.node);
		aslist.push_front(tmp);
	}

	// showList(aslist);
}

/* Traverse maze using the A* algorithm */
bool traverse()
{
	int numOfNodes = rows*cols;
	cell tmp;
	int goal = rows*cols-1;					// end state in maze, once reached, end loop
	std::list<cell>::iterator curr, min;	
	

	cell start(0,0);
	start.inClosed = true;
	aslist.push_front(start);
	it = aslist.begin();					// add start to closed list

	while(true) 
	{
		/* Look left */
		if(it->node % cols != 0 && !myMaze->isWall(it->node,it->node - 1)) 
			addToOpenList(aslist,it, it->node-1);
		/* Look right */
		if((it->node + 1) % cols != 0 && !myMaze->isWall(it->node, it->node + 1))
			addToOpenList(aslist,it, it->node+1);
		/* Look up */
		if(it->node / cols != 0 && !myMaze->isWall(it->node, it->node - cols))
			addToOpenList(aslist,it, it->node-cols);
		/* Look down */
		if(it->node/cols != rows-1 && !myMaze->isWall(it->node, it->node + cols))
			addToOpenList(aslist,it,it->node+cols);

		/* Pick node with lowest F cost from open list */
		min = aslist.begin();
		for(std::list<cell>::iterator i = min; i != it; ++i)
		{
			if(i->fCost() < min->fCost())
				min = i;
		}

		// If no path found, return false
		if(it == aslist.begin())
			return false;

		/* Add node with minimum fCost to closed list and make it 'it' */
		it--;
		iter_swap(it,min);

		/* Check for end state */
		if(it->node == goal) 
			break;
	}

	return true;
}


/* OpenGL drawing Code */
void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	glBegin(GL_LINES);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	glEnd();
}

void drawCircle( int node )
{
	int numPoints = 200;
	float offsetX = (horDiv/2) * ((node%cols) * 2 + 1);
	float offsetY = (vertDiv/2) * ((rows - 1 - (node/cols)) * 2 + 1);
	float Radius = min(horDiv,vertDiv) * 0.25;	// Make radius 25% of the min(horLine, vertLine)
	glBegin( GL_LINE_STRIP );
	for( int i=0; i<numPoints; i++ )
	 {
		  float Angle = i * (2.0*PI/numPoints); 
		  float X = cos( Angle )*Radius;		
		  float Y = sin( Angle )*Radius;
		  X += offsetX;
		  Y += offsetY;
		  glVertex2f( X, Y );
	 }
	glEnd();
}

void drawPath()
{
	cell tmp = *it;
	while(tmp.node != 0)
	{
		drawCircle(tmp.node);
		tmp = *incList(aslist, it, tmp.backPtr);
	}
	drawCircle(0);
}

void display()
{
	glColor3f(0.9,.9,.9);

	GLfloat x1, y1, x2, y2;
	int firstCell, secondCell;

	glClear(GL_COLOR_BUFFER_BIT);

	/****** Draw outside borders *******/
	drawLine(horDiv, 399., 600., 399.);
	drawLine(599.5, 400., 599.5, vertDiv);
	drawLine(0.5,0.,0.5,400.-vertDiv);
	drawLine(0.,0.5, 600.-horDiv, 0.5);

	/******** Draw Maze ***************/
	for(int i = 0; i < myMaze->numOfWalls(); ++i)
	{
		firstCell = myMaze->wallFP(i);
		secondCell = myMaze->wallSP(i);
		if(std::abs(firstCell - secondCell) == 1)
		{	// Draw vertical line
			x1 = (secondCell % cols) * horDiv;
			y1 = HEIGHT - (firstCell/cols * vertDiv);
			x2 = x1;
			y2 = y1 - vertDiv;
		}
		else
		{	// Draw horizontal line
			x1 = (secondCell % cols) * horDiv;
			y1 = HEIGHT - (secondCell/cols * vertDiv);
			x2 = x1 + horDiv;
			y2 = y1;
		}
		drawLine(x1, y1, x2, y2);
	}

	glColor3f(1,0,0);
	drawPath();

	glFlush();
}

void myinit()
{
	horDiv = WIDTH/cols;
	vertDiv = HEIGHT/rows;

	// Initialize OpenGL State attributes
	glLineWidth(2.);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glColor3f(0.9f,0.9f,0.9f);
	glPointSize(1.5);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
}

int main(int argc, char** argv)
{
	// Get dimensions
	std::cout << "Enter two ints, rows and columns of the maze\n";
	std::cin >> rows >> cols;

	myMaze = new maze(rows,cols);
	myMaze->createMaze();
	// myMaze->printMaze();  // Enable to see maze representation in std output
	traverse();

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Maze");
	glutDisplayFunc(display);
	myinit();
	glutMainLoop();

	_getch();
	return 0;
}