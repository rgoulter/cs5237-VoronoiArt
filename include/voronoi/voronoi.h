#ifndef VORONOI_Voronoi_h
#define VORONOI_Voronoi_h

#include <functional>
#include <list>
#include <queue>
#include <set>

#include "voronoi/vpoint.h"
#include "voronoi/vedge.h"
#include "voronoi/vparabola.h"
#include "voronoi/vevent.h"



namespace voronoi {

/*
    Useful data containers for Vertices (places) and Edges of Voronoi diagram
*/
typedef std::list<VPoint *> Vertices;
typedef std::list<VEdge *> Edges;



/*
    Class for generating the Voronoi diagram
*/

class Voronoi {
public:
	/*
	    Constructor - without any parameters
	*/
	Voronoi();

	/*
	    The only public function for generating a diagram

	    input:
	        v : Vertices - places for drawing a diagram
	        w : width  of the result (top left corner is (0, 0))
	        h : height of the result
	    output:
	        pointer to list of edges

	    All the data structures are managed by this class
	*/
	Edges * getEdges(Vertices *v, int w, int h);

private:
	/*
	    places : container of places with which we work
	    edges : container of edges which will be teh result
	    width : width of the diagram
	    height : height of the diagram
	    root : the root of the tree, that represents a beachline sequence
	    ly : current "y" position of the line (see Fortune's algorithm)
	*/

	Vertices *places;
	Edges *edges;
	double width, height;
	VParabola *root;
	double ly;

	/*
	    deleted : set  of deleted (false) Events (since we can not delete from PriorityQueue
	    points : list of all new points that were created during the algorithm
	    queue : priority queue with events to process
	*/
	std::set<VEvent *> deleted;
	std::list<VPoint *> points;
	std::priority_queue<VEvent *, std::vector<VEvent *>, VEvent::CompareEvent> queue;

	/*
	    InsertParabola : processing the place event
	    RemoveParabola : processing the circle event
	    FinishEdge : recursively finishes all infinite edges in the tree
	    GetXOfEdge : returns the current x position of an intersection point of left and right parabolas
	    GetParabolaByX : returns the Parabola that is under this "x" position in the current beachline
	    CheckCircle : checks the circle event (disappearing) of this parabola
	    GetEdgeInterse
	*/

	void insertParabola(VPoint *p);
	void removeParabola(VEvent *e);
	void finishEdge(VParabola *n);
	double getXOfEdge(VParabola *par, double y);
	VParabola * getParabolaByX(double xx);
	double getY(VPoint *p, double x);
	void checkCircle(VParabola *b);
	VPoint * getEdgeIntersection(VEdge *a, VEdge *b);
};

}

#endif
