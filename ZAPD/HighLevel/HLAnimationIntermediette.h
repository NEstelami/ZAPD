#pragma once

#include <stdint.h>
#include "../ZAnimation.h"
#include "HLFileIntermediette.h"
#include "tinyxml2.h"

/*
 * An intermediette format for animations. Going to use XML.
 * Goes from FBX->XML->C
 * Note: At the moment this is a very direct representation of the output format.
 * Optimally we can determine where the keyframes are and remove redundant information.
 */

class HLAnimationIntermediette
{
public:
	std::vector<uint16_t> rotationValues;
	std::vector<RotationIndex> rotationIndices;
	int16_t frameCount;
	int16_t limit;
	int16_t limbCount;

	HLAnimationIntermediette();
	~HLAnimationIntermediette();

	std::string OutputXML();
	ZAnimation* ToZAnimation();

	static HLAnimationIntermediette* FromXML(const std::string& xmlPath);
	static HLAnimationIntermediette* FromZAnimation(ZAnimation* zAnim);
};