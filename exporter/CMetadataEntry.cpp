//
// Created by georg on 02.02.24.
//

#include "CMetadataEntry.h"

void CMetadataEntry::addValue(double d) {
    values.push_back(std::to_string(d));
}

void CMetadataEntry::addValue(long d) {
    values.push_back(std::to_string(d));
}

void CMetadataEntry::addValue(const std::string s) {
    values.push_back(s);
}

std::ostream &operator<<(std::ostream &os, const CMetadataEntry &entry) {
    os << "values: ";
    for(std::string i : entry.values) {
        os << i << ",";
    }
    os << "|";
    return os;
}
