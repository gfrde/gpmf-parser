//
// Created by georg on 11.02.24.
//

#include "CMetadataEntryString.h"

CMetadataEntryString::CMetadataEntryString(const std::string &value) : value(value) {}


std::ostream &operator<<(std::ostream &os, const CMetadataEntryString &entry) {
    os << "values: ";
    os << entry.value << ",";
    os << "|";
    return os;
}
