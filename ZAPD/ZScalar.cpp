#include "ZScalar.h"
#include "BitConverter.h"
#include "File.h"
#include "Globals.h"
#include "StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Scalar, ZScalar);

ZScalar::ZScalar(ZFile* nParent) : ZResource(nParent)
{
	memset(&scalarData, 0, sizeof(ZScalarData));
	scalarType = ZScalarType::ZSCALAR_NONE;
}

ZScalar::ZScalar(const ZScalarType scalarType, ZFile* nParent) : ZScalar(nParent)
{
	this->scalarType = scalarType;
}

void ZScalar::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                             const uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawData, nRawDataIndex);
}

void ZScalar::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string type = reader->Attribute("Type");
	scalarType = ZScalar::MapOutputTypeToScalarType(type);
}

ZScalarType ZScalar::MapOutputTypeToScalarType(const std::string& type)
{
	if (type == "s8")
	{
		return ZScalarType::ZSCALAR_S8;
	}
	else if (type == "u8")
	{
		return ZScalarType::ZSCALAR_U8;
	}
	else if (type == "s16")
	{
		return ZScalarType::ZSCALAR_S16;
	}
	else if (type == "u16")
	{
		return ZScalarType::ZSCALAR_U16;
	}
	else if (type == "s32")
	{
		return ZScalarType::ZSCALAR_S32;
	}
	else if (type == "u32")
	{
		return ZScalarType::ZSCALAR_U32;
	}
	else if (type == "s64")
	{
		return ZScalarType::ZSCALAR_S64;
	}
	else if (type == "u64")
	{
		return ZScalarType::ZSCALAR_U64;
	}
	else if (type == "f32")
	{
		return ZScalarType::ZSCALAR_F32;
	}
	else if (type == "f64")
	{
		return ZScalarType::ZSCALAR_F64;
	}

	return ZScalarType::ZSCALAR_NONE;
}

std::string ZScalar::MapScalarTypeToOutputType(const ZScalarType scalarType)
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return "s8";
	case ZScalarType::ZSCALAR_U8:
		return "u8";
	case ZScalarType::ZSCALAR_S16:
		return "s16";
	case ZScalarType::ZSCALAR_U16:
		return "u16";
	case ZScalarType::ZSCALAR_S32:
		return "s32";
	case ZScalarType::ZSCALAR_U32:
		return "u32";
	case ZScalarType::ZSCALAR_S64:
		return "s64";
	case ZScalarType::ZSCALAR_U64:
		return "u64";
	case ZScalarType::ZSCALAR_F32:
		return "f32";
	case ZScalarType::ZSCALAR_F64:
		return "f64";
	default:
		return "";
	}
}

size_t ZScalar::MapTypeToSize(const ZScalarType scalarType)
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return sizeof(scalarData.s8);
	case ZScalarType::ZSCALAR_U8:
		return sizeof(scalarData.u8);
	case ZScalarType::ZSCALAR_S16:
		return sizeof(scalarData.s16);
	case ZScalarType::ZSCALAR_U16:
		return sizeof(scalarData.u16);
	case ZScalarType::ZSCALAR_S32:
		return sizeof(scalarData.s32);
	case ZScalarType::ZSCALAR_U32:
		return sizeof(scalarData.u32);
	case ZScalarType::ZSCALAR_S64:
		return sizeof(scalarData.s64);
	case ZScalarType::ZSCALAR_U64:
		return sizeof(scalarData.u64);
	case ZScalarType::ZSCALAR_F32:
		return sizeof(scalarData.f32);
	case ZScalarType::ZSCALAR_F64:
		return sizeof(scalarData.f64);
	default:
		return 0;
	}
}

size_t ZScalar::GetRawDataSize() const
{
	return ZScalar::MapTypeToSize(scalarType);
}

void ZScalar::ParseRawData()
{
	ZScalar::ParseRawData(rawData, rawDataIndex);
}

void ZScalar::ParseRawData(const std::vector<uint8_t>& data, const uint32_t offset)
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		scalarData.s8 = BitConverter::ToInt8BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_U8:
		scalarData.u8 = BitConverter::ToUInt8BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_S16:
		scalarData.s16 = BitConverter::ToInt16BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_U16:
		scalarData.u16 = BitConverter::ToUInt16BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_S32:
		scalarData.s32 = BitConverter::ToInt32BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_U32:
		scalarData.u32 = BitConverter::ToUInt32BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_S64:
		scalarData.s64 = BitConverter::ToInt64BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_U64:
		scalarData.u64 = BitConverter::ToUInt64BE(data, offset);
		break;
	case ZScalarType::ZSCALAR_F32:
		scalarData.f32 = BitConverter::ToFloatBE(data, offset);
		break;
	case ZScalarType::ZSCALAR_F64:
		scalarData.f64 = BitConverter::ToDoubleBE(data, offset);
		break;
	case ZScalarType::ZSCALAR_NONE:
		fprintf(stderr, "Warning in ZScalar: Invalid type. %d %s %d\n", (int32_t)scalarType,
		        __FILE__, __LINE__);
		break;
	}
}

std::string ZScalar::GetSourceTypeName() const
{
	return ZScalar::MapScalarTypeToOutputType(scalarType);
}

std::string ZScalar::GetSourceValue() const
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return StringHelper::Sprintf("%hhd", scalarData.s8);
	case ZScalarType::ZSCALAR_U8:
		return StringHelper::Sprintf("%hhu", scalarData.u8);
	case ZScalarType::ZSCALAR_S16:
		return StringHelper::Sprintf("%hd", scalarData.s16);
	case ZScalarType::ZSCALAR_U16:
		return StringHelper::Sprintf("%hu", scalarData.u16);
	case ZScalarType::ZSCALAR_S32:
		return StringHelper::Sprintf("%d", scalarData.s32);
	case ZScalarType::ZSCALAR_U32:
		return StringHelper::Sprintf("%u", scalarData.u32);
	case ZScalarType::ZSCALAR_S64:
		return StringHelper::Sprintf("%lld", scalarData.s64);
	case ZScalarType::ZSCALAR_U64:
		return StringHelper::Sprintf("%llu", scalarData.u64);
	case ZScalarType::ZSCALAR_F32:
		return StringHelper::Sprintf("%f", scalarData.f32);
	case ZScalarType::ZSCALAR_F64:
		return StringHelper::Sprintf("%lf", scalarData.f64);
	default:
		return "";
	}
}

std::string ZScalar::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
		                       GetSourceTypeName(), GetName(), GetSourceValue());

	return "";
}

ZResourceType ZScalar::GetResourceType() const
{
	return ZResourceType::Scalar;
}

bool ZScalar::DoesSupportArray() const
{
	return true;
}
