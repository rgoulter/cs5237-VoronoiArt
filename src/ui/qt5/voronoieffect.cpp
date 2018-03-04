#include "ui/qt5/voronoieffect.h"

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#include "geometry/polygon.h"

#include "ui/opengl/graphics.h"

#include "polypixel.h"

using std::vector;

using delaunay::LongInt;
using delaunay::PointSetArray;
using delaunay::TriRecord;

using geometry::Polygon;

using ui::qt5::EffectState;
using ui::qt5::ShowImageType;
using ui::qt5::VoronoiEffect;



void VoronoiEffect::setEffectState(EffectState state) {
	effectState_ = state;
	emit effectChanged();
}



void VoronoiEffect::setEffectShowType(ShowImageType kind) {
	EffectState tmp = getEffectState();
	tmp.showType = kind;
	setEffectState(tmp);
}



EffectState VoronoiEffect::getEffectState() {
	return effectState_;
}



void VoronoiEffect::setImageData(ImageData* imageData) {
	imageData_ = imageData;

	EffectState currentState = getEffectState();
	currentState.showType = IMAGE;
	setEffectState(currentState);

	// Maybe should distinguish between size changing, & just 'pls redraw'?
	emit imageLoaded();
	emit effectChanged();
}



ImageData* VoronoiEffect::getImageData() {
	return imageData_;
}



void VoronoiEffect::setVoronoiPolygons(const vector<Polygon>& polygons) {
	renderedPolygons_ = generateColoredPolygons(polygons, *imageData_);
}



void VoronoiEffect::paintGL() {
	switch (effectState_.showType) {
		case EFFECT:
			drawColoredPolygons(renderedPolygons_);
			break;

		case EDGE_RAW:
			pdfTextures_.edgesTexture->renderPlane();
			break;

		case EDGE_SHARP:
			pdfTextures_.edgesSharpTexture->renderPlane();
			break;

		case EDGE_BLUR:
			pdfTextures_.edgesBlurTexture->renderPlane();
			break;

		case PDF:
			pdfTextures_.pdfTexture->renderPlane();
			break;

		case IMAGE:
			imageData_->renderPlane();
			break;

		default:
		case NONE:
			break;
	}

	if (effectState_.showVertices && algorithm_ != nullptr) {
		const PointSetArray<LongInt>& points = algorithm_->allPoints();
		drawPointSetArray(points);
	}

	if (effectState_.showEdges && algorithm_ != nullptr && algorithm_->finished()) {
		// DELAUNAY (voronoiEdges)
		const vector<Polygon>& voronoiPolygons = algorithm_->getVoronoiPolygons();
		drawVoronoiPolygons(voronoiPolygons);
	}

	if (effectState_.showAlgorithm && algorithm_ != nullptr) {
		const PointSetArray<LongInt>& pointSet = algorithm_->allPoints();
		const vector<TriRecord>& leafNodes = algorithm_->getLeafNodes();
		drawDelaunayTriangles(pointSet, leafNodes);
	}
}



void VoronoiEffect::clearAll() {
	if (algorithm_ != nullptr) {
		delete algorithm_;
		algorithm_ = nullptr;
	}
	
	effectState_.showType = NONE;
	effectState_.showVertices = true;
	effectState_.showEdges = false;
	effectState_.showAlgorithm = true;

	renderedPolygons_.clear();

	delete pdfTextures_.edgesTexture;
	pdfTextures_.edgesTexture = nullptr;

	delete pdfTextures_.edgesSharpTexture;
	pdfTextures_.edgesSharpTexture = nullptr;

	delete pdfTextures_.edgesBlurTexture;
	pdfTextures_.edgesBlurTexture = nullptr;

	delete pdfTextures_.pdfTexture;
	pdfTextures_.pdfTexture = nullptr;
}
