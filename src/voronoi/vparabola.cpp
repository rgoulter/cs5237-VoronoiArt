#include "vparabola.h"

#include "vpoint.h"



/*
    Constructors
*/

VParabola::VParabola() {
	site = 0;
	isLeaf = false;
	cEvent = 0;
	edge = 0;
	parent = 0;
}



VParabola::VParabola(VPoint * s) {
	site = s;
	isLeaf = true;
	cEvent = 0;
	edge = 0;
	parent = 0;
}



/*
    Tree operations (described in the header file)
*/

VParabola * VParabola::getLeft(VParabola *p) {
	return getLeftChild(getLeftParent(p));
}




VParabola * VParabola::getRight(VParabola *p) {
	return getRightChild(getRightParent(p));
}



VParabola * VParabola::getLeftParent(VParabola *p) {
	VParabola *par = p->parent;
	VParabola *pLast = p;

	while (par->left() == pLast) {
		if (!par->parent)
			return 0;

		pLast = par;
		par = par->parent;
	}

	return par;
}



VParabola * VParabola::getRightParent(VParabola *p) {
	VParabola *par = p->parent;
	VParabola *pLast = p;

	while (par->right() == pLast) {
		if (!par->parent)
			return 0;

		pLast = par;
		par = par->parent;
	}

	return par;
}



VParabola * VParabola::getLeftChild(VParabola *p) {
	if (!p)
		return 0;

	VParabola * par = p->left();

	while (!par->isLeaf)
		par = par->right();

	return par;
}



VParabola * VParabola::getRightChild(VParabola *p) {
	if (!p)
		return 0;

	VParabola *par = p->right();

	while (!par->isLeaf)
		par = par->left();

	return par;
}

