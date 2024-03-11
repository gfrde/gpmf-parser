//
// Created by georg on 11.02.24.
//

#ifndef GPMF_PARSER_CMETADATAENTRYSTRUCT_H
#define GPMF_PARSER_CMETADATAENTRYSTRUCT_H


#include <vector>
#include <string>
#include "IMetadataEntry.h"

class CMetadataEntryStruct: public IMetadataEntry {

public:
    explicit CMetadataEntryStruct(const std::string &key);
    friend std::ostream &operator<<(std::ostream &os, const CMetadataEntryStruct &entry);

protected:
    std::string key;
    std::vector<IMetadataEntry*> values;

};


#endif //GPMF_PARSER_CMETADATAENTRYSTRUCT_H
