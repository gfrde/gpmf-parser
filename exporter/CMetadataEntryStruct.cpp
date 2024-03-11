//
// Created by georg on 11.02.24.
//

#include "CMetadataEntryStruct.h"

CMetadataEntryStruct::CMetadataEntryStruct(const std::string &key) : key(key) {}

std::ostream &operator<<(std::ostream &os, const CMetadataEntryStruct &entry) {
    os << "values: ";
    for(IMetadataEntry* i : entry.values) {
        os << i << ",";
    }
    os << "|";
    return os;
}
