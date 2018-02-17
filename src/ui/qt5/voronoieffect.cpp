#include "ui/qt5/voronoieffect.h"

#include "ui/opengl/graphics.h"

#include "polypixel.h"

using std::vector;

using ui::qt5::EffectState;
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



void VoronoiEffect::setVoronoiPolygons(const vector<geometry::Polygon>& polygons) {
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

	// if (showVoronoiEdges_) {
	// 	// DELAUNAY (voronoiEdges)
	// 	// drawVoronoiPolygons(voronoiPolygons_);
	// }

	// if (showVoronoiSites_) {
	// 	// drawPointSetArray(inputPointSet_);
	// }
}
