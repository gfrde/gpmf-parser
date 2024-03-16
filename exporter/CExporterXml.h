//
// Created by georg on 11.03.24.
//

#ifndef GPMF_PARSER_CEXPORTERXML_H
#define GPMF_PARSER_CEXPORTERXML_H


#include <string>
#include <fstream>
#include "IExporter.h"
#include "CMetadata.h"

class CExporterXml: IExporter {

public:
    explicit CExporterXml(const std::string &filename);

    virtual ~CExporterXml();

    void create(const CMetadata& data);
    void close();

    void metadataStart(const CMetadata& data);
    void metadataStop(const CMetadata& data);

    void typeStart(const CMetadata& data, const std::string &type);
    void typeStop(const CMetadata& data, const std::string &type);

    void write(const CMetadata &meta, const std::string &type, const CMetadataEntry &data);

protected:
    std::string filename;
    std::ofstream out;
};


#endif //GPMF_PARSER_CEXPORTERXML_H
