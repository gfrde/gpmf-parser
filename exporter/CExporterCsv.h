//
// Created by georg on 17.03.24.
//

#ifndef GPMF_PARSER_CEXPORTERCSV_H
#define GPMF_PARSER_CEXPORTERCSV_H


#include <string>
#include <fstream>
#include "IExporter.h"
#include "CMetadata.h"

class CExporterCsv: public IExporter {

public:
    CExporterCsv(const std::string &filename, const std::vector<std::string> &attrToColumn, bool mergeTimes);

    virtual ~CExporterCsv();

    void create(const CMetadata& data) override;
    void close() override;

    void metadataStart(const CMetadata& data) override;
    void metadataStop(const CMetadata& data) override;

    void typeStart(const CMetadata& data, const std::string &type) override;
    void typeStop(const CMetadata& data, const std::string &type) override;

    void write(const CMetadata &meta, const std::string &type, long countEntries, long idx,
               const CMetadataEntry &data) override;

protected:
    const bool mergeTimes;
    std::string delimiter = ";";
public:
    void setDelimiter(const std::string &delimiter);

protected:
    std::vector<std::string> attrToColumn;
    std::map<double, std::vector<std::string>> combinedData;
};


#endif //GPMF_PARSER_CEXPORTERCSV_H
