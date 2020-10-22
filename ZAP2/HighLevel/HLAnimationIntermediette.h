#pragma once

#include <stdint.h>
#include "../ZAnimation.h"
#include "../tinyxml2.h"
#include "HLFileIntermediette.h"

/*
 * An intermediette format for animations. Going to use XML.
 * Goes from FBX->XML->C
 */

class HLAnimationIntermediette
{
public:
	int16_t frameCount;
	int16_t limit;
	int16_t limbCount;
	std::vector<uint16_t> rotationValues;
	std::vector<RotationIndex> rotationIndices;

	HLAnimationIntermediette();
	~HLAnimationIntermediette();
	
	std::string OutputXML();
	ZAnimation* ToZAnimation();

	static HLAnimationIntermediette* FromXML(std::string xmlPath);
	static HLAnimationIntermediette* FromZAnimation(ZAnimation* zAnim);
};