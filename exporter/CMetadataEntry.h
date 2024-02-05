//
// Created by georg on 02.02.24.
//

#ifndef GPMF_PARSER_CMETADATAENTRY_H
#define GPMF_PARSER_CMETADATAENTRY_H


#include <map>
#include <vector>
#include <string>
#include <ostream>

class CMetadataEntry {

public:
    void addValue(std::string s);
    void addValue(double d);
    void addValue(long d);

    friend std::ostream &operator<<(std::ostream &os, const CMetadataEntry &entry);

protected:
    std::vector<std::string> values;
};


#endif //GPMF_PARSER_CMETADATAENTRY_H
