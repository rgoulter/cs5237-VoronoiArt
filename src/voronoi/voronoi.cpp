#include "voronoi/voronoi.h"

#include <cmath>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>

#include "stopwatch.h"

using std::vector;



namespace voronoi {

Voronoi::Voronoi() {
	edges = 0;
}



Edges * Voronoi::getEdges(Vertices *v, int w, int h) {
	places = v;
	width = w;
	height = h;
	root = 0;

	if (!edges)
		edges = new Edges();
	else {
		for (Vertices::iterator i = points.begin(); i != points.end(); ++i)
			delete (*i);
		for(Edges::iterator i = edges->begin(); i != edges->end(); ++i)
			delete (*i);

		points.clear();
		edges->clear();
	}

	for (Vertices::iterator i = places->begin(); i != places->end(); ++i) {
		queue.push(new VEvent(*i, true));
	}

	VEvent *e;

	while (!queue.empty()) {
		e = queue.top();
		queue.pop();
		ly = e->point->y;

		if (deleted.find(e) != deleted.end()) {
			delete(e);
			deleted.erase(e);
			continue;
		}

		if (e->pe)
			insertParabola(e->point);
		else
			removeParabola(e);

		delete(e);
	}

	finishEdge(root);

	for (Edges::iterator i = edges->begin(); i != edges->end(); ++i) {
		if ((*i)->neighbour) {
			(*i)->start = (*i)->neighbour->end;
			delete (*i)->neighbour;
		}
	}

	return edges;
}



void Voronoi::insertParabola(VPoint *p) {
	if (!root) {
		root = new VParabola(p);
		return;
	}

	// TRANSLATEME degenerovaný pøípad - obì spodní místa ve stejné výšce
	if (root->isLeaf && root->site->y - p->y < 0.1) {
		VPoint *fp = root->site;
		root->isLeaf = false;
		root->setLeft(new VParabola(fp));
		root->setRight(new VParabola(p));

		VPoint *s = new VPoint((p->x + fp->x) / 2, height); // TRANSLATEME zaèátek hrany uprostøed míst
		points.push_back(s);

		if (p->x > fp->x)
		    root->edge = new VEdge(s, fp, p); // TRANSLATEME rozhodnu, který vlevo, který vpravo
		else
		    root->edge = new VEdge(s, p, fp);

		edges->push_back(root->edge);

		return;
	}

	VParabola *par = getParabolaByX(p->x);

	if (par->cEvent) {
		deleted.insert(par->cEvent);
		par->cEvent = 0;
	}

	VPoint *start = new VPoint(p->x, getY(par->site, p->x));
	points.push_back(start);

	VEdge *el = new VEdge(start, par->site, p);
	VEdge *er = new VEdge(start, p, par->site);

	el->neighbour = er;
	edges->push_back(el);

	// TRANSLATEME pøestavuju strom .. vkládám novou parabolu
	par->edge = er;
	par->isLeaf = false;

	VParabola *p0 = new VParabola(par->site);
	VParabola *p1 = new VParabola(p);
	VParabola *p2 = new VParabola(par->site);

	par->setRight(p2);
	par->setLeft(new VParabola());
	par->left()->edge = el;

	par->left()->setLeft(p0);
	par->left()->setRight(p1);

	checkCircle(p0);
	checkCircle(p2);
}



void Voronoi::removeParabola(VEvent *e) {
	VParabola *p1 = e->arch;

	VParabola *xl = VParabola::getLeftParent(p1);
	VParabola *xr = VParabola::getRightParent(p1);

	VParabola *p0 = VParabola::getLeftChild(xl);
	VParabola *p2 = VParabola::getRightChild(xr);

	if (p0 == p2)
		std::cout << "TRANSLATEME chyba - pravá a levá parabola má stejné ohnisko!\n";

	if (p0->cEvent) {
		deleted.insert(p0->cEvent);
		p0->cEvent = 0;
	}

	if (p2->cEvent) {
		deleted.insert(p2->cEvent);
		p2->cEvent = 0;
	}

	VPoint *p = new VPoint(e->point->x, getY(p1->site, e->point->x));
	points.push_back(p);

	xl->edge->end = p;
	xr->edge->end = p;

	VParabola *higher;
	VParabola *par = p1;

	while (par != root) {
		par = par->parent;
		if (par == xl)
			higher = xl;
		if (par == xr)
			higher = xr;
	}

	higher->edge = new VEdge(p, p0->site, p2->site);
	edges->push_back(higher->edge);

	VParabola *gparent = p1->parent->parent;

	if (p1->parent->left() == p1) {
		if (gparent->left()  == p1->parent)
			gparent->setLeft(p1->parent->right());
		if (gparent->right() == p1->parent)
			gparent->setRight(p1->parent->right());
	} else {
		if (gparent->left()  == p1->parent)
			gparent->setLeft(p1->parent->left());
		if (gparent->right() == p1->parent)
			gparent->setRight(p1->parent->left());
	}

	delete p1->parent;
	delete p1;

	checkCircle(p0);
	checkCircle(p2);
}



void Voronoi::finishEdge(VParabola *n) {
	if (n->isLeaf) {
		delete n;
		return;
	}

	double mx;

	if (n->edge->direction->x > 0.0)
		mx = std::max(width, n->edge->start->x + 10);
	else
		mx = std::min(0.0, n->edge->start->x - 10);

	VPoint *end = new VPoint(mx, mx * n->edge->f + n->edge->g);
	n->edge->end = end;
	points.push_back(end);

	finishEdge(n->left());
	finishEdge(n->right());

	delete n;
}



double Voronoi::getXOfEdge(VParabola *par, double y) {
	VParabola *left  = VParabola::getLeftChild(par);
	VParabola *right = VParabola::getRightChild(par);

	VPoint *p = left->site;
	VPoint *r = right->site;

	double dp = 2.0 * (p->y - y);

	double a1 = 1.0 / dp;
	double b1 = -2.0 * p->x / dp;
	double c1 = y + dp / 4 + p->x * p->x / dp;

	dp = 2.0 * (r->y - y);

	double a2 = 1.0 / dp;
	double b2 = -2.0 * r->x / dp;
	double c2 = ly + dp / 4 + r->x * r->x / dp;

	double a = a1 - a2;
	double b = b1 - b2;
	double c = c1 - c2;

	double disc = b * b - 4 * a * c;
	double x1 = (-b + std::sqrt(disc)) / (2*a);
	double x2 = (-b - std::sqrt(disc)) / (2*a);

	double ry;
	if (p->y < r->y)
		ry = std::max(x1, x2);
	else
		ry = std::min(x1, x2);

	return ry;
}



VParabola * Voronoi::getParabolaByX(double xx) {
	VParabola *par = root;
	double x = 0.0;

	// TRANSLATEME projdu stromem dokud nenarazím na vhodný list
	while (!par->isLeaf) {
		x = getXOfEdge(par, ly);

		if (x > xx)
			par = par->left();
		else
			par = par->right();
	}

	return par;
}



// TRANSLATEME ohnisko, x-souøadnice
double Voronoi::getY(VPoint *p, double x) {
	double dp = 2 * (p->y - ly);
	double a1 = 1 / dp;
	double b1 = -2 * p->x / dp;
	double c1 = ly + dp / 4 + p->x * p->x / dp;

	return (a1*x*x + b1*x + c1);
}



void Voronoi::checkCircle(VParabola *b) {
	VParabola *lp = VParabola::getLeftParent (b);
	VParabola *rp = VParabola::getRightParent(b);

	VParabola *a  = VParabola::getLeftChild (lp);
	VParabola *c  = VParabola::getRightChild(rp);

	if (!a || !c || a->site == c->site)
		return;

	VPoint *s = 0;
	s = getEdgeIntersection(lp->edge, rp->edge);

	if (s == 0)
		return;

	double dx = a->site->x - s->x;
	double dy = a->site->y - s->y;

	double d = std::sqrt((dx * dx) + (dy * dy));

	if (s->y - d >= ly) {
		return;
	}

	VEvent *e = new VEvent(new VPoint(s->x, s->y - d), false);
	points.push_back(e->point);
	b->cEvent = e;
	e->arch = b;
	queue.push(e);
}

VPoint * Voronoi::getEdgeIntersection(VEdge *a, VEdge *b) {
	double x = (b->g - a->g) / (a->f - b->f);
	double y = a->f * x + a->g;

	//Test Code
	//if(a->f - b->f >=0)
	//{if(b->g-a->g>=0) x= 1000000.0;
	//else x = -1000000.0;}


	if ((x - a->start->x) / a->direction->x < 0)
		return 0;
	if ((y - a->start->y) / a->direction->y < 0)
		return 0;

	if ((x - b->start->x) / b->direction->x < 0)
		return 0;
	if ((y - b->start->y) / b->direction->y < 0)
		return 0;

	VPoint *p = new VPoint(x, y);
	points.push_back(p);

	return p;
}



// Helper function
void addPointWithJitter(Vertices* vertices, double x, double y) {
	// Program hangs if jitter size is too small, oddly.
	// (Because of limitation in the Fortune algorithm/implementation).

	// const double kJitterSize = 15.0;
	const double kJitterSize = 5;
	double dx = ((double)rand() * kJitterSize / (double)RAND_MAX);
	double dy = ((double)rand() * kJitterSize / (double)RAND_MAX);

	VPoint *vp = new VPoint(x + dx, y + dy);
	vertices->push_back(vp);
}



Vertices* verticesForInputPoints(const delaunay::PointSetArray& inputPoints) {
	Vertices *voronoiVertices = new Vertices();

	// Iterate over PointSetArray's points
	delaunay::LongInt x, y;
	for (int ptIdx = 1; ptIdx <= inputPoints.noPt(); ++ptIdx) {
		inputPoints.getPoint(ptIdx, x, y);

		addPointWithJitter(voronoiVertices, x.doubleValue(), y.doubleValue());
	}

	// Bounding Points for Fortune's
	addPointWithJitter(voronoiVertices, -10000.0,  10000.0);
	addPointWithJitter(voronoiVertices,  10000.0,  10000.0);
	addPointWithJitter(voronoiVertices,  10000.0, -10000.0);
	addPointWithJitter(voronoiVertices, -10000.0, -10000.0);

	return voronoiVertices;
}



// POLYREP:POINTSETARRAY
/// Use the constructed Voronoi edges to make Polygons to display
vector<delaunay::PointSetArray> createPolygonsFortune(Edges *voronoiedges) {
	using delaunay::LongInt;
	using delaunay::PointSetArray;

	vector<PointSetArray> voronoiEdges;

	// The dictionary is indexed by the voronoi points and gives the polygon for each voronoi.
	// The border polygons are unbounded, so need to be careful.
	std::map< VPoint *, vector<VEdge *> > dictionary;

	// XXX This should be its own function.
	// Given a list of VEdges,
	// construct reverse-lookup of VEdges associated with each VPoint.
	for (Edges::iterator i = voronoiedges->begin(); i != voronoiedges->end(); ++i) {
		VEdge *edge = *i;
		VPoint *leftPt = edge->left;
		VPoint *rightPt = edge->right;

		//Check if the dictionary has leftpoint
		std::map< VPoint *, vector<VEdge *> >::iterator it = dictionary.find(leftPt);

		if (it != dictionary.end()) {
			it->second.push_back(edge);
		} else {
			vector<VEdge *> listofedges;
			listofedges.push_back(edge);
			dictionary.insert(std::map<VPoint *,vector<VEdge *> >::value_type(leftPt,listofedges));
		}

		//Check if the dictionary has rightpoint
		std::map< VPoint *, vector<VEdge *> >::iterator it2 = dictionary.find(rightPt);

		if (it2 != dictionary.end()) {
			it2->second.push_back(edge);
		} else {
			vector<VEdge *> listofedges2;
			listofedges2.push_back(edge);
			dictionary.insert(std::map<VPoint *,vector<VEdge *> >::value_type(rightPt,listofedges2));
		}
	}


	// What follows is *supposed* to yield polygons from edges,
	// but doesn't manage to always do what it's intended to.

	// Convert each of the dictionary values (unsorted edges associated with vertex)
	// into ordered list of PointSetArray vertex set.
	std::map< VPoint *, vector<VEdge *> >::iterator dictioniter;

	for (dictioniter = dictionary.begin(); dictioniter != dictionary.end(); ++dictioniter) {
		vector<VEdge *> polygonEdges = dictioniter->second;
		PointSetArray polygonvertices;
		vector<VPoint *> revvector;

		// polygonEdges has a set of edges, create an ordered list of points from this.
		// Take the first edge, store its start and end points into pointsetarray.
		VEdge * firstedge = *polygonEdges.begin();
		polygonvertices.addPoint( (LongInt)firstedge->start->x, (LongInt)firstedge->start->y );
		polygonvertices.addPoint( (LongInt)firstedge->end->x, (LongInt)firstedge->end->y );

		// Store the start and end point of this edge for later use.
		// When the last edge is found, its end edge will be the same as
		// the first edge's starting point. This signifies completion of polygon.
		//
		// The ending point will be updated as each subsequent edge is found.
		VPoint *startpoint = new VPoint(firstedge->start->x, firstedge->start->y);
		VPoint *endpoint   = new VPoint(firstedge->end->x, firstedge->end->y); // XXX CpyCtor
		polygonEdges.erase(polygonEdges.begin()); // NB remove firstedge, as we've consumed it

		int edgeCount = polygonEdges.size(); // Why would *this* matter? WRONG, surely.


		while (edgeCount > 0) {
			vector<VEdge *>::iterator polygonedgeiter;
			int exitflag = 0;

			// Iterate throught the remaining list of edges to find the subsequent edge
			for (polygonedgeiter = polygonEdges.begin(); polygonedgeiter != polygonEdges.end(); ++polygonedgeiter) {
				VEdge * eachedge = *polygonedgeiter;

				if (eachedge->start->x == endpoint->x &&
				    eachedge->start->y == endpoint->y) {
					polygonvertices.addPoint( (LongInt)eachedge->end->x, (LongInt)eachedge->end->y );
					endpoint->x = eachedge->end->x; // Check if this pointer assignment works...
					endpoint->y = eachedge->end->y; // XXX Copy-Assignment operator
					polygonEdges.erase(polygonedgeiter); // Delete the edge that has been added to the pointset array
					edgeCount--;
					exitflag = 1;

					break;
				} else if (eachedge->end->x == endpoint->x &&
				           eachedge->end->y == endpoint->y) {
					polygonvertices.addPoint( (LongInt)eachedge->start->x, (LongInt)eachedge->start->y );
					endpoint->x = eachedge->start->x;
					endpoint->y = eachedge->start->y;
					polygonEdges.erase(polygonedgeiter); // Delete the edge that has been added to the pointset array
					edgeCount--;
					exitflag = 1;

					break;
				}
			}

			//If edge has not been found, the following break executes and while exits with incomplete polygon(border condition).
			if (edgeCount > 0 && exitflag == 0) {
				//PointSetArray revpolygonvertices;
				for (polygonedgeiter = polygonEdges.begin(); polygonedgeiter != polygonEdges.end(); ++polygonedgeiter) {
					VEdge * eachedge = *polygonedgeiter;

					if (eachedge->start->x == startpoint->x &&
					    eachedge->start->y == startpoint->y) {
						//revpolygonvertices.addPoint( (LongInt)eachedge->end->x, (LongInt)eachedge->end->y );
						revvector.push_back(eachedge->end);
						startpoint->x = eachedge->end->x; // Check if this pointer assignment works...
						startpoint->y = eachedge->end->y;
						polygonEdges.erase(polygonedgeiter);
						edgeCount--;
						exitflag = 1;

						break;
					} else if (eachedge->end->x == startpoint->x &&
					           eachedge->end->y == startpoint->y) {
						//revpolygonvertices.addPoint( (LongInt)eachedge->start->x, (LongInt)eachedge->start->y );
						revvector.push_back(eachedge->start);
						startpoint->x = eachedge->start->x;
						startpoint->y = eachedge->start->y;
						polygonEdges.erase(polygonedgeiter);
						edgeCount--;
						exitflag = 1;

						break;
					}
				} // End of for
			}
		} //End of while- Current polygon has been found

		//Push the polygon into the list of polygons.
		while (revvector.size() != 0) {
			VPoint * point = revvector.back();
			polygonvertices.addPoint( (LongInt)point->x, (LongInt)point->y );
			revvector.pop_back();
		}

		voronoiEdges.push_back(polygonvertices);
	}// All polygons have been found

	return voronoiEdges;
}



// POLYREP:POINTSETARRAY

vector<delaunay::PointSetArray> runVoronoiAlgorithm(const delaunay::PointSetArray& inputPoints) {
	StopWatch voroSW;
	voroSW.reset();
	voroSW.resume();

	Vertices *voronoiVertices_ = verticesForInputPoints(inputPoints);

	Voronoi voronoi;
	Edges *voronoiEdges = voronoi.getEdges(voronoiVertices_, 10000, 10000);
	voroSW.pause();
	double timeFortune = voroSW.ms();
	std::cout << "TIME: voronoi->GetEdges(..) is " << timeFortune << std::endl;
	voroSW.reset();
	voroSW.resume();

	vector<delaunay::PointSetArray> voronoiPolygons =
	    createPolygonsFortune(voronoiEdges);

	voroSW.pause();
	double timePolyConstruct = voroSW.ms();
	std::cout << "TIME: createPolygonsFortune() is " << timePolyConstruct << std::endl;;
	voroSW.reset();
	voroSW.resume();

	// XXX cleanup voronoiVertices, etc.

	return voronoiPolygons;
}



}

