#ifndef UI_QT5_VORONOIEFFECTH
#define UI_QT5_VORONOIEFFECTH

#include <string>
#include <vector>

#include <QObject>

#include "delaunay/longint/li.h"

#include "delaunay/delaunay.h"

#include "geometry/polygon.h"

#include "generatepoints.h"  // for PDFTextures
#include "imagedata.h"
#include "polypixel.h"       // for ColoredPolygon

namespace ui {

namespace qt5 {



enum ShowImageType {
	IMAGE,
	EDGE_RAW,
	EDGE_SHARP,
	EDGE_BLUR,
	PDF,
	EFFECT,
	NONE
};



class EffectState {
public:
	ShowImageType showType;
	bool showVertices;
	bool showEdges;
	bool showAlgorithm;
};



///
class VoronoiEffect : public QObject {
	Q_OBJECT
public:
	VoronoiEffect(QObject *parent = 0) : QObject(parent) {
		effectState_.showType = NONE;
		effectState_.showVertices = true;
		effectState_.showEdges = false;
		effectState_.showAlgorithm = true;
	};

	// XXX:#24: property: Algorithm e.g. Delaunay
	// XXX:#24: property: image, drawState, etc.
	// XXX:#24: and a method to *draw* this.

	void setImageData(ImageData* imageData);

	ImageData* getImageData();

	EffectState getEffectState();

	void setEffectShowType(ShowImageType kind);

	void setPDFTextures(PDFTextures textures) { pdfTextures_ = textures; };

	void setDelaunayAlgorithm(delaunay::DelaunayAlgorithm<delaunay::LongInt>* algorithm) { algorithm_ = algorithm; };

	void setVoronoiPolygons(const std::vector<geometry::Polygon>& polygons);

	void paintGL();

signals:
	/// This signal exists so that the mainqt knows to update enabled/disabled state of its button
	void imageLoaded();
	/// Indicates that the PDF computation was completed; ergo, updates UI in mainqt
	void setUsePDF(bool);
	/// Indicates that the algorithm has finished computing; ergo, updates UI in mainqt
	void setVoronoiComputed(bool);

	/// Indicates that the canvas needs to be redrawn.
	void effectChanged();

public slots:
	void setEffectState(EffectState state);

	// XXX but, like, surely we can use sig + slots here?

private:
	delaunay::DelaunayAlgorithm<delaunay::LongInt>* algorithm_ = nullptr;
	EffectState effectState_;
	// DELAUNAY / Rendering.
	// XXX:#24:Should move this to ... algorithm state, or something.
	/// The `ColoredPolygon`s we use to render the "stain-glass" effect.
	std::vector<ColoredPolygon> renderedPolygons_;

	// XXX:#24:Should move these to ... VisualEffectState or something?
	std::string loadedImageFilename_ = "";
	ImageData *imageData_ = NULL;
	PDFTextures pdfTextures_;
};



}

}

#endif