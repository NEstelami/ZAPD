#include "ZVector.h"
#include <assert.h>
#include "BitConverter.h"
#include "File.h"
#include "Globals.h"
#include "StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Vector, ZVector);

ZVector::ZVector(ZFile* nParent) : ZResource(nParent)
{
	this->scalarType = ZSCALAR_NONE;
	this->dimensions = 0;
	RegisterRequiredAttribute("Type");
	RegisterRequiredAttribute("Dimensions");
}

ZVector::~ZVector()
{
	ClearScalars();
}

void ZVector::ClearScalars()
{
	for (auto s : scalars)
		delete s;

	scalars.clear();
}

void ZVector::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	this->scalarType = ZScalar::MapOutputTypeToScalarType(registeredAttributes.at("Type").value);

	this->dimensions = StringHelper::StrToL(registeredAttributes.at("Dimensions").value, 16);
}

void ZVector::ParseRawData()
{
	int32_t currentRawDataIndex = this->rawDataIndex;

	ClearScalars();

	for (uint32_t i = 0; i < this->dimensions; i++)
	{
		ZScalar* scalar = new ZScalar(this->scalarType, this->parent);
		scalar->rawDataIndex = currentRawDataIndex;
		scalar->rawData = this->rawData;
		scalar->ParseRawData();
		currentRawDataIndex += scalar->GetRawDataSize();

		this->scalars.push_back(scalar);
	}

	// Ensure the scalars vector has the same number of elements as the vector dimension.
	assert(this->scalars.size() == this->dimensions);
}

size_t ZVector::GetRawDataSize() const
{
	size_t size = 0;

	for (size_t i = 0; i < this->scalars.size(); i++)
		size += this->scalars.at(i)->GetRawDataSize();

	return size;
}

bool ZVector::DoesSupportArray() const
{
	return true;
}

std::string ZVector::GetSourceTypeName() const
{
	if (dimensions == 3 && scalarType == ZSCALAR_F32)
		return "Vec3f";
	else if (dimensions == 3 && scalarType == ZSCALAR_S16)
		return "Vec3s";
	else if (dimensions == 3 && scalarType == ZSCALAR_S32)
		return "Vec3i";
	else
	{
		std::string output = StringHelper::Sprintf(
			"Encountered unsupported vector type: %d dimensions, %s type", dimensions,
			ZScalar::MapScalarTypeToOutputType(scalarType).c_str());

		if (Globals::Instance->verbosity >= VERBOSITY_DEBUG)
			printf("%s\n", output.c_str());

		throw std::runtime_error(output);
	}
}

std::string ZVector::GetBodySourceCode() const
{
	std::vector<std::string> strings = std::vector<std::string>();

	for (size_t i = 0; i < this->scalars.size(); i++)
		strings.push_back(scalars[i]->GetBodySourceCode());

	return "{ " + StringHelper::Implode(strings, ", ") + " }";
}

std::string ZVector::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
		                       GetSourceTypeName(), GetName(), GetBodySourceCode());

	return "";
}

ZResourceType ZVector::GetResourceType() const
{
	return ZResourceType::Vector;
}