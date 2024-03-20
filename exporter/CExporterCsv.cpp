//
// Created by georg on 17.03.24.
//

#include <algorithm>
#include "CExporterCsv.h"


bool isNumber(const std::string& s) {
//    atof(s.c_str());

    char *endptr = NULL;
    double d = strtod(s.c_str(), &endptr);
    if (endptr == nullptr) {
        return false;
    }
    return true;
}


CExporterCsv::CExporterCsv(const std::string &filename, const std::vector<std::string> &attrToColumn, bool mergeTimes) : IExporter(
        filename), attrToColumn(attrToColumn), mergeTimes(mergeTimes) {}

CExporterCsv::~CExporterCsv() {
    if (out.is_open()) {
        close();
    }
}

void CExporterCsv::create(const CMetadata &data) {
    out.open(filename, std::ios_base::trunc | std::ios_base::binary);

    // write CSV header
    auto col =  attrToColumn.begin();
    out << "\"time\"" << ";\"frame\"" << ";\"index\"";
    while (col != attrToColumn.end()) {
        out << ";\"" << *col << "\"";
        col ++;
    }
    out << std::endl;
}

void CExporterCsv::close() {
    if (out.is_open()) {
        out.close();
    }
}

void CExporterCsv::metadataStart(const CMetadata &data) {
//    out << "  <index pos=\"" << data.getIndex() << "\">" << std::endl;

    combinedData.clear();
}

void CExporterCsv::metadataStop(const CMetadata &data) {
//    out << "  </index>" << std::endl;
    if (mergeTimes) {
        for (const auto &timedata: combinedData) {
            out << timedata.first << ";" << data.getIndex() << ";";

            for (const auto &item: timedata.second) {
                out << ";";
                if (item.empty()) {
                    continue;
                }

                if (isNumber(item)) {
                    out << item;
                } else {
                    out << "\"" << item << "\"";
                }
            }

            out << std::endl;
        }
    }
}

void CExporterCsv::typeStart(const CMetadata &meta, const std::string &type) {
//    out << "    <entry type=\"" << type << "\"" << " start=\"" << meta.getTimeStart() << "\"" << " end=\"" << meta.getTimeEnd() << "\"" << ">" << std::endl;
}

void CExporterCsv::typeStop(const CMetadata &data, const std::string &type) {
//    out << std::endl;
}

void CExporterCsv::write(const CMetadata &meta, const std::string &type, long countEntries, long idx,
                         const CMetadataEntry &data) {

    std::vector<std::string> mappings;
    if (mappingTypePosition2Element.find(type) != mappingTypePosition2Element.end()) {
        mappings = mappingTypePosition2Element.find(type)->second;
    }

    double time = meta.getTimeStart() + idx * (meta.getTimeEnd() - meta.getTimeStart()) / countEntries;
    std::vector<std::string> emptyCells(attrToColumn.size(), "");
    std::vector<std::string> *cells = &emptyCells;
    if (mergeTimes) {
        auto existingEntry = combinedData.find(time);
        if (existingEntry != combinedData.end()) {
            cells = &existingEntry->second;
        } else {
            auto newEntry = combinedData.insert(std::pair(time, emptyCells));
            cells = &newEntry.first->second;
        }
    } else {
        out << time << ";" << meta.getIndex() << ";" << idx;
    }

    int pos = -1;
    for (const auto &item: data.getValues()) {
        pos ++;
        std::string element = "value";
        if (pos < mappings.size()) {
            element = mappings.at(pos);
        }

        std::string c = type;
        c += "_" + element;
        std::transform(c.begin(), c.end(), c.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        auto foundEl = std::find(attrToColumn.begin(), attrToColumn.end(), c);
        if (foundEl != attrToColumn.end()) {
            int colPos = std::distance(attrToColumn.begin(), foundEl);

            if ((*cells)[colPos].empty()) {
                (*cells)[colPos] = item;
            } else {
                (*cells)[colPos] += "," + item;
            }
        } else {
            printf("unknown col: %s\n", c.c_str());
        }
    }

    if (!mergeTimes) {
        for (const auto &item: *cells) {
            out << ";";
            if (item.empty()) {
                continue;
            }

            if (isNumber(item)) {
                out << item;
            } else {
                out << "\"" << item << "\"";
            }
        }
        out << std::endl;
    }
}

