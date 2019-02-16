#pragma once
#include "BrushMAP.hpp"

BrushMAP::BrushMAP()
{

}

BrushMAP::BrushMAP( Vector3 *planeDescriptionThreePoints, std::string const &texName, Vector2 const &texPositionOffset, float texRotation, Vector2 const &texScale )
{
	this->planeDescriptionPoints[0] = planeDescriptionThreePoints[0];
	this->planeDescriptionPoints[1] = planeDescriptionThreePoints[1];
	this->planeDescriptionPoints[2] = planeDescriptionThreePoints[2];

	this->textureName = texName;

	this->texturePositionOffset	= texPositionOffset;
	this->textureRotation		= texRotation;
	this->textureScale			= texScale;
}

