//
// Created by georg on 11.03.24.
//

#ifndef GPMF_PARSER_IEXPORTER_H
#define GPMF_PARSER_IEXPORTER_H


#include <map>
#include <vector>
#include "CMetadata.h"

extern std::map<std::string, std::vector<std::string>> mappingTypePosition2Element;

class IExporter {

public:
    explicit IExporter(const std::string &filename) : filename(filename) {}

    virtual ~IExporter() {

    }

    virtual void create(const CMetadata& data) = 0;
    virtual void close() = 0;

    virtual void metadataStart(const CMetadata& data) = 0;
    virtual void metadataStop(const CMetadata& data) = 0;

    virtual void typeStart(const CMetadata& data, const std::string &type) = 0;
    virtual void typeStop(const CMetadata& data, const std::string &type) = 0;

    virtual void write(const CMetadata &meta, const std::string &type, long countEntries, long idx,
               const CMetadataEntry &data) = 0;

protected:
    std::string filename;
    std::ofstream out;

};


#endif //GPMF_PARSER_IEXPORTER_H
