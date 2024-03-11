//
// Created by georg on 02.02.24.
//

#include "CMetadataEntry.h"

void CMetadataEntry::addValue(double d, std::string unit) {
    values.push_back(std::to_string(d));
    units.push_back(unit);
}

void CMetadataEntry::addValue(long d, std::string unit) {
    values.push_back(std::to_string(d));
    units.push_back(unit);
}

void CMetadataEntry::addValue(const std::string s, std::string unit) {
    values.push_back(s);
    units.push_back(unit);
}

std::ostream &operator<<(std::ostream &os, const CMetadataEntry &entry) {
    os << "values: ";
    int cnt = 0;
    for(const std::string& i : entry.values) {
        os << i;

        const std::string& u = entry.units.at(cnt);
        if (!u.empty()) {
            os << " (" << u << ")";
        }
        os << ",";

        cnt ++;
    }
    os << "|";
    return os;
}
