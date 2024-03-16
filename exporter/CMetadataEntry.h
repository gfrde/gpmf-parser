//
// Created by georg on 02.02.24.
//

#ifndef GPMF_PARSER_CMETADATAENTRY_H
#define GPMF_PARSER_CMETADATAENTRY_H


#include <map>
#include <vector>
#include <string>
#include <ostream>
#include "IMetadataEntry.h"


class CMetadataEntry: public IMetadataEntry {
public:


public:
    void addValue(std::string s, std::string unit);
    void addValue(double d, std::string unit);
    void addValue(long d, std::string unit);

    friend std::ostream &operator<<(std::ostream &os, const CMetadataEntry &entry);

protected:
    std::vector<std::string> values;
    std::vector<std::string> units;

public:
    [[nodiscard]] const std::vector<std::string> &getValues() const;
    [[nodiscard]] const std::vector<std::string> &getUnits() const;
};


#endif //GPMF_PARSER_CMETADATAENTRY_H
