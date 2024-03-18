//
// Created by georg on 17.03.24.
//

#include <algorithm>
#include "CExporterCsv.h"


CExporterCsv::CExporterCsv(const std::string &filename, const std::vector<std::string> &attrToColumn) : IExporter(
        filename), attrToColumn(attrToColumn) {}

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
}

void CExporterCsv::metadataStop(const CMetadata &data) {
//    out << "  </index>" << std::endl;
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
    out << time << ";" << meta.getIndex() << ";" << idx;

    int pos = -1;
    std::vector<std::string> cells(attrToColumn.size(), "");
    for (const auto &item: data.getValues()) {
        pos ++;
        std::string element = "value";
        if (pos < mappings.size()) {
            element = mappings.at(pos);
        }

        std::string c = type + "_" + element;
        std::transform(c.begin(), c.end(), c.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        auto foundEl = std::find(attrToColumn.begin(), attrToColumn.end(), c);
        if (foundEl != attrToColumn.end()) {
            int colPos = std::distance(attrToColumn.begin(), foundEl);

            cells[colPos] = item;
        } else {
            printf("unknown col: %s\n", c.c_str());
        }
    }

    for (const auto &item: cells) {
        out << ";";
        if (item.empty()) {
            continue;
        }
        out << "\"" << item << "\"";
    }
//    int pos = -1;
//    while (it_values != data.getValues().end()) {
//        pos ++;
//        std::string element = "value";
//        if (pos < mappings.size()) {
//            element = mappings.at(pos);
//        }
//
//        out << "        <" << element;
//        if (it_unit != data.getUnits().end() && !it_unit->empty()) {
//            out << " unit=\"" << *it_unit << "\"";
//        }
//        out << ">" << *it_values << "</" << element << ">" << std::endl;
//
//        it_values ++;
//        if (it_unit != data.getUnits().end()) {
//            it_unit ++;
//        }
//    }
    out << std::endl;
}

