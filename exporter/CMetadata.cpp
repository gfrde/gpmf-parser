//
// Created by georg on 02.02.24.
//

#include "CMetadata.h"

CMetadata::CMetadata(const std::string &sourceName, unsigned int index, double timeStart, double timeEnd) : sourceName(
        sourceName), index(index), timeStart(timeStart), timeEnd(timeEnd) {}

void CMetadata::addEntry(const std::string &type, CMetadataEntry &entry) {

    auto el = entriesPerType.find(type);
    if (el == entriesPerType.end()) {
        auto l = std::vector<CMetadataEntry>();
        l.push_back(entry);
        entriesPerType.insert(std::make_pair(type, l));
    } else {
        el->second.push_back(entry);
    }
}


