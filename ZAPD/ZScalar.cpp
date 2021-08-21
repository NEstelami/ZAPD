#include "ZScalar.h"
#include <map>
#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Scalar, ZScalar);

ZScalar::ZScalar(ZFile* nParent) : ZResource(nParent)
{
	memset(&scalarData, 0, sizeof(ZScalarData));
	scalarType = ZScalarType::ZSCALAR_NONE;
	RegisterRequiredAttribute("Type");
}

ZScalar::ZScalar(const ZScalarType scalarType, ZFile* nParent) : ZScalar(nParent)
{
	this->scalarType = scalarType;
}

void ZScalar::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	scalarType = ZScalar::MapOutputTypeToScalarType(registeredAttributes.at("Type").value);
}

ZScalarType ZScalar::MapOutputTypeToScalarType(const std::string& type)
{
	static const std::map<std::string, ZScalarType> ZScalarTypeDictionary = {
		{"s8", ZScalarType::ZSCALAR_S8},   {"u8", ZScalarType::ZSCALAR_U8},
		{"x8", ZScalarType::ZSCALAR_X8},   {"s16", ZScalarType::ZSCALAR_S16},
		{"u16", ZScalarType::ZSCALAR_U16}, {"x16", ZScalarType::ZSCALAR_X16},
		{"s32", ZScalarType::ZSCALAR_S32}, {"u32", ZScalarType::ZSCALAR_U32},
		{"x32", ZScalarType::ZSCALAR_X32}, {"s64", ZScalarType::ZSCALAR_S64},
		{"u64", ZScalarType::ZSCALAR_U64}, {"x64", ZScalarType::ZSCALAR_X64},
		{"f32", ZScalarType::ZSCALAR_F32}, {"f64", ZScalarType::ZSCALAR_F64},
	};

	auto it = ZScalarTypeDictionary.find(type);
	if (it == ZScalarTypeDictionary.end())
		return ZScalarType::ZSCALAR_NONE;
	
	return it->second;
	
}

std::string ZScalar::MapScalarTypeToOutputType(const ZScalarType scalarType)
{
	switch (scalarType)
	{
		case ZScalarType::ZSCALAR_S8:
			return "s8";

		case ZScalarType::ZSCALAR_U8:
		case ZScalarType::ZSCALAR_X8:
			return "u8";

		case ZScalarType::ZSCALAR_S16:
			return "s16";

		case ZScalarType::ZSCALAR_U16:
		case ZScalarType::ZSCALAR_X16:
			return "u16";

		case ZScalarType::ZSCALAR_S32:
			return "s32";

		case ZScalarType::ZSCALAR_U32:
		case ZScalarType::ZSCALAR_X32:
			return "u32";

		case ZScalarType::ZSCALAR_S64:
			return "s64";

		case ZScalarType::ZSCALAR_U64:
		case ZScalarType::ZSCALAR_X64:
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
		case ZScalarType::ZSCALAR_X8:
			return sizeof(scalarData.u8);

		case ZScalarType::ZSCALAR_S16:
			return sizeof(scalarData.s16);

		case ZScalarType::ZSCALAR_U16:
		case ZScalarType::ZSCALAR_X16:
			return sizeof(scalarData.u16);

		case ZScalarType::ZSCALAR_S32:
			return sizeof(scalarData.s32);

		case ZScalarType::ZSCALAR_U32:
		case ZScalarType::ZSCALAR_X32:
			return sizeof(scalarData.u32);

		case ZScalarType::ZSCALAR_S64:
			return sizeof(scalarData.s64);

		case ZScalarType::ZSCALAR_U64:
		case ZScalarType::ZSCALAR_X64:
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
	const auto& rawData = parent->GetRawData();
	switch (scalarType)
	{
		case ZScalarType::ZSCALAR_S8:
			scalarData.s8 = BitConverter::ToInt8BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_U8:
		case ZScalarType::ZSCALAR_X8:
			scalarData.u8 = BitConverter::ToUInt8BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_S16:
			scalarData.s16 = BitConverter::ToInt16BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_U16:
		case ZScalarType::ZSCALAR_X16:
			scalarData.u16 = BitConverter::ToUInt16BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_S32:
			scalarData.s32 = BitConverter::ToInt32BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_U32:
		case ZScalarType::ZSCALAR_X32:
			scalarData.u32 = BitConverter::ToUInt32BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_S64:
			scalarData.s64 = BitConverter::ToInt64BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_U64:
		case ZScalarType::ZSCALAR_X64:
			scalarData.u64 = BitConverter::ToUInt64BE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_F32:
			scalarData.f32 = BitConverter::ToFloatBE(rawData, rawDataIndex);
			break;

		case ZScalarType::ZSCALAR_F64:
			scalarData.f64 = BitConverter::ToDoubleBE(rawData, rawDataIndex);
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

std::string ZScalar::GetBodySourceCode() const
{
	switch (scalarType)
	{
		case ZScalarType::ZSCALAR_S8:
			return StringHelper::Sprintf("%hhd", scalarData.s8);
		case ZScalarType::ZSCALAR_U8:
			return StringHelper::Sprintf("%hhu", scalarData.u8);
		case ZScalarType::ZSCALAR_X8:
			return StringHelper::Sprintf("0x%02X", scalarData.u8);
		case ZScalarType::ZSCALAR_S16:
			return StringHelper::Sprintf("%hd", scalarData.s16);
		case ZScalarType::ZSCALAR_U16:
			return StringHelper::Sprintf("%hu", scalarData.u16);
		case ZScalarType::ZSCALAR_X16:
			return StringHelper::Sprintf("0x%04X", scalarData.u16);
		case ZScalarType::ZSCALAR_S32:
			return StringHelper::Sprintf("%d", scalarData.s32);
		case ZScalarType::ZSCALAR_U32:
			return StringHelper::Sprintf("%u", scalarData.u32);
		case ZScalarType::ZSCALAR_X32:
			return StringHelper::Sprintf("0x%08X", scalarData.u32);
		case ZScalarType::ZSCALAR_S64:
			return StringHelper::Sprintf("%lld", scalarData.s64);
		case ZScalarType::ZSCALAR_U64:
			return StringHelper::Sprintf("%llu", scalarData.u64);
		case ZScalarType::ZSCALAR_X64:
			return StringHelper::Sprintf("0x%016X", scalarData.u64);
		case ZScalarType::ZSCALAR_F32:
			return StringHelper::Sprintf("%f", scalarData.f32);
		case ZScalarType::ZSCALAR_F64:
			return StringHelper::Sprintf("%lf", scalarData.f64);
		default:
			return "SCALAR_ERROR";
	}
}

std::string ZScalar::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
		                       GetSourceTypeName(), GetName(), GetBodySourceCode());

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
