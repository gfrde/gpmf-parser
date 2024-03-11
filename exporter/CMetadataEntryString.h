//
// Created by georg on 11.02.24.
//

#ifndef GPMF_PARSER_CMETADATAENTRYSTRING_H
#define GPMF_PARSER_CMETADATAENTRYSTRING_H


#include <string>
#include "IMetadataEntry.h"

class CMetadataEntryString: public IMetadataEntry {
public:
    explicit CMetadataEntryString(const std::string &value);

    friend std::ostream &operator<<(std::ostream &os, const CMetadataEntryString &entry);

protected:
    std::string value;
};


#endif //GPMF_PARSER_CMETADATAENTRYSTRING_H
