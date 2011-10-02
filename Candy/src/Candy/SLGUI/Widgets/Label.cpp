/*
 * Label.cpp
 *
 *  Created on: 29.11.2009
 *      Author: david
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Label.h"
#include "../Painter.h"
//#include <Danvil/SimpleEngine/Primitives.h>
//#include <Danvil/Images/ImageOps.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace Candy {
namespace SLGUI {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Label::Label() {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setText("test");
	borderWidth = 0;
}

Label::Label(const std::string& _text) {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setText(_text);
	borderWidth = 0;
}

Label::Label(const Image3ub& img) {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setImage(img);
	borderWidth = 0;
}

Label::Label(const PTR(Candy::Texture2)& tex) {
	horizontalAlignment = Left;
	verticalAlignment = Mid;
	setTexture(tex);
	borderWidth = 0;
}

Label::~Label() {
}

void Label::setText(const std::string& _text) {
	text = _text;
	Painter::Singleton->size(Painter::Normal, text, size.x(), size.y());
	size.x() += 6;
	size.y() += 4;
}

void Label::setImage(const Image3ub& img) {
	//Danvil::Images::Image3ubPtr img_flipped = Danvil::Images::ImageOps::FlipY(img);
	//Danvil::Images::Image3ubPtr img_glsize = Danvil::Images::ImageOps::ConvertToOpenGl(img_flipped);
	if(!texture_) {
		texture_.reset(new Candy::Texture2());
	}
	texture_->Load(img);
	backgroundSize = Point2(texture_->width(), texture_->height());
	size = backgroundSize;
}

void Label::setTexture(const PTR(Candy::Texture2)& tex) {
	texture_ = tex;
	backgroundSize = Point2(tex->width(), tex->height());
	size = backgroundSize;
}

void Label::render() {
	// render image
	if(texture_) {
		// render frame
		Frame::render();
		// get dimensions
/*		unsigned int texSize = texture_->width();
		// The image is painted with maximal possible size.
		// Note that the texture_ size may me larger than the image size, because
		// textures must have a size of 2^n in OpenGL.
		// Compute maximal possible scaling factor of image
		float fx = (float)backgroundSize.x() / (float)texSize;
		float fy = (float)backgroundSize.y() / (float)texSize;*/
		// render quad
		texture_->Bind();
		Painter::Singleton->setColor(Colorf(1.0f, 1.0f, 1.0f, opacity));
		Painter::Singleton->paintQuadWithTexture(backgroundSize, 1.0f, 1.0f); // fx, fy);
		texture_->DisableTexture();
	}
	// render text
	if(!text.empty()) {
		Point2 text_dim = Painter::Singleton->size(Painter::Normal, text);
		float px = 0.0f;
		switch(horizontalAlignment) {
		case Left:
			px = 2;
			break;
		case Center:
			px = 0.5f*(size.x() - text_dim.x());
			break;
		case Right:
			px = size.x() - text_dim.x() - 2;
			break;
		}
		float py = 0.0f;
		switch(verticalAlignment) {
		case Up:
			py = 2;
			break;
		case Center:
			py = 0.5f*(size.y() - text_dim.y());
			break;
		case Bottom:
			py = size.y() - text_dim.y() - 2;
			break;
		}
		Painter::Singleton->print(Painter::Normal, text, px, py, getTextColor());
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
