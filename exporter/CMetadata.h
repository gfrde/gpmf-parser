//
// Created by georg on 02.02.24.
//

#ifndef GPMF_PARSER_CMETADATA_H
#define GPMF_PARSER_CMETADATA_H


#include <string>
#include <map>
#include <vector>
#include "CMetadataEntry.h"

class CMetadata {

public:
    CMetadata(const std::string &sourceName, unsigned int index, double timeStart, double timeEnd);

    void addEntry(const std::string &type, CMetadataEntry& entry);

private:

    std::string sourceName;
    unsigned int index;
    double timeStart;
    double timeEnd;

    std::map<std::string, std::vector<CMetadataEntry>> entriesPerType;
};


#endif //GPMF_PARSER_CMETADATA_H
