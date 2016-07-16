#ifndef DELAUNAY_POINTSETIOH
#define DELAUNAY_POINTSETIOH

#include <string>
#include <vector>
#include <utility>

namespace delaunay {

// Limitation of this interface is can't indicate error.
std::vector< std::pair<int,int> > parsePointList(const std::string& s);



std::vector< std::pair<int,int> > parsePointListFile(const std::string& fn);

}

#endif
