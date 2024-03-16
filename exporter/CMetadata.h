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

    friend std::ostream &operator<<(std::ostream &os, const CMetadata &entry);

private:

    std::string sourceName;
    unsigned int index;
public:
    unsigned int getIndex() const;

    double getTimeStart() const;

    double getTimeEnd() const;

private:
    double timeStart;
    double timeEnd;

    std::map<std::string, std::vector<CMetadataEntry>> entriesPerType;

public:
    [[nodiscard]] const std::map<std::string, std::vector<CMetadataEntry>> &getEntriesPerType() const;
};


#endif //GPMF_PARSER_CMETADATA_H
