	#pragma once

	#include "Globals.h"
	#include "ZResource.h"

	class ZTextureHeader : public ZResource
	{
		public:
		/* void */ ZTextureHeader(ZFile* );

		void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex);

		void ParseXML(tinyxml2::XMLElement* reader) override;
		void ParseRawData() override;
		void DeclareReferences(const std::string& prefix) override;

		std::string GetBodySourceCode() const;
		std::string GetSourceOutputCode(const std::string& prefix) override;

		std::string GetSourceTypeName() const override;
		ZResourceType GetResourceType() const override;

		size_t GetRawDataSize() const override;

	};

