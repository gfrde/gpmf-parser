//
// Created by georg on 11.03.24.
//

#ifndef GPMF_PARSER_CEXPORTERXML_H
#define GPMF_PARSER_CEXPORTERXML_H


#include <string>
#include <fstream>
#include "IExporter.h"
#include "CMetadata.h"

class CExporterXml: public IExporter {

public:

    explicit CExporterXml(const std::string &filename);

    virtual ~CExporterXml();

    void create(const CMetadata& data) override;
    void close() override;

    void metadataStart(const CMetadata& data) override;
    void metadataStop(const CMetadata& data) override;

    void typeStart(const CMetadata& data, const std::string &type) override;
    void typeStop(const CMetadata& data, const std::string &type) override;

    void write(const CMetadata &meta, const std::string &type, long countEntries, long idx,
               const CMetadataEntry &data) override;

};


#endif //GPMF_PARSER_CEXPORTERXML_H
