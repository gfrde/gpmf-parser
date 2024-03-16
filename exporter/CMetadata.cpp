//
// Created by georg on 02.02.24.
//

#include "CMetadata.h"

CMetadata::CMetadata(const std::string &sourceName, unsigned int index, double timeStart, double timeEnd) : sourceName(
        sourceName), index(index), timeStart(timeStart), timeEnd(timeEnd) {}

void CMetadata::addEntry(const std::string &type, CMetadataEntry& entry) {

    auto el = entriesPerType.find(type);
    if (el == entriesPerType.end()) {
        auto l = std::vector<CMetadataEntry>();
        l.push_back(entry);
        entriesPerType.insert(std::make_pair(type, l));
    } else {
        el->second.push_back(entry);
    }
}


std::ostream &operator<<(std::ostream &os, const CMetadata &entry) {
    os << "---- meta" << std::endl;
    int cnt = 0;
//    for(const std::string& i : entry.entriesPerType) {
    for(const std::pair<std::string, std::vector<CMetadataEntry>> i : entry.entriesPerType) {
        os << "  key=" << i.first << std::endl;

        for (const CMetadataEntry& e: i.second) {
            os << "    " << e << std::endl;
        }

        cnt ++;
    }
    return os;
}

const std::map<std::string, std::vector<CMetadataEntry>> &CMetadata::getEntriesPerType() const {
    return entriesPerType;
}

unsigned int CMetadata::getIndex() const {
    return index;
}

double CMetadata::getTimeStart() const {
    return timeStart;
}

double CMetadata::getTimeEnd() const {
    return timeEnd;
}

