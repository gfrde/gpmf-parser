//
// Created by georg on 11.03.24.
//

#include <fstream>
#include "CExporterXml.h"

std::map<std::string, std::vector<std::string>> mappingTypePosition2Element = {
        {"GPS5", {"lat", "lon", "altitude", "speed2d", "speed3d"}},
        {"GPS9", {"lat", "lon", "altitude", "speed2d", "speed3d", "day", "seconds", "precision", "fix"}},
        {"ACCL", {"y", "x_neg", "z"}},
        {"GYRO", {"y", "x_neg", "z"}},
        {"ISOG", {"gain"}},
        {"SHUT", {"exposure"}},
        {"STMP", {"microsec"}},
        {"MSKP", {"frameskipMain"}},
        {"LSKP", {"frameskipLow"}},
        {"CORI", {"q1", "q2", "q3", "q4"}},
        {"IORI", {"q1", "q2", "q3", "q4"}},
        {"AALP", {"audiolevel", "peak"}},
        {"SCEN", {"prob"}},
        {"MAGN", {"v1"}},
        {"GRAV", {"y", "x_neg", "z"}},
        {"FACE", {
            "version0", "confidence0", "id0", "x0", "y0", "w0", "h0", "smile0", "blink0",
            "version1", "confidence1", "id1", "x1", "y1", "w1", "h1", "smile1", "blink1",
            "version2", "confidence2", "id2", "x2", "y2", "w2", "h2", "smile2", "blink2",
            "version3", "confidence3", "id3", "x3", "y3", "w3", "h3", "smile3", "blink3",
            "version4", "confidence4", "id4", "x4", "y4", "w4", "h4", "smile4", "blink4",
        }},
};

CExporterXml::CExporterXml(const std::string &filename) : IExporter(filename) {
}

CExporterXml::~CExporterXml() {
    if (out.is_open()) {
        close();
    }
}

void CExporterXml::create(const CMetadata &data) {
    out.open(filename, std::ios_base::trunc | std::ios_base::binary);

    out << "<video_meta>" << std::endl;
}

void CExporterXml::close() {
    out << "</video_meta>";
    if (out.is_open()) {
        out.close();
    }
}

void CExporterXml::metadataStart(const CMetadata &data) {
    out << "  <index pos=\"" << data.getIndex() << "\">" << std::endl;
}

void CExporterXml::metadataStop(const CMetadata &data) {
    out << "  </index>" << std::endl;
}

void CExporterXml::typeStart(const CMetadata &meta, const std::string &type) {
    out << "    <entry type=\"" << type << "\"" << " start=\"" << meta.getTimeStart() << "\"" << " end=\"" << meta.getTimeEnd() << "\"" << ">" << std::endl;
}

void CExporterXml::typeStop(const CMetadata &data, const std::string &type) {
    out << "    </entry>" << std::endl;
}

void CExporterXml::write(const CMetadata &meta, const std::string &type, long countEntries, long idx,
                         const CMetadataEntry &data) {

    std::vector<std::string> mappings;
    if (mappingTypePosition2Element.find(type) != mappingTypePosition2Element.end()) {
        mappings = mappingTypePosition2Element.find(type)->second;
    }

    double time = meta.getTimeStart() + idx * (meta.getTimeEnd() - meta.getTimeStart()) / countEntries;
    out << "      <values" << " index=\"" << idx << "\"" << " timestamp=\"" << time << "\"" << ">" << std::endl;
    auto it_values = data.getValues().begin();
    auto it_unit = data.getUnits().begin();
    int pos = -1;
    while (it_values != data.getValues().end()) {
        pos ++;
        std::string element = "value";
        if (pos < mappings.size()) {
            element = mappings.at(pos);
        }

        out << "        <" << element;
        if (it_unit != data.getUnits().end() && !it_unit->empty()) {
            out << " unit=\"" << *it_unit << "\"";
        }
        out << ">" << *it_values << "</" << element << ">" << std::endl;

        it_values ++;
        if (it_unit != data.getUnits().end()) {
            it_unit ++;
        }
    }
    out << "      </values>" << std::endl;
}

