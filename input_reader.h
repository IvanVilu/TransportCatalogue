#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include "transport_catalogue.h"


enum class QueryType {
    GetBusInfo,
    GetStopInfo,
    AddBus,
    AddStop
};

struct Query {
    QueryType type;
    std::string request;
};

// trim from start (in place)
void ltrim(std::string& s);

// trim from end (in place)
void rtrim(std::string& s);

// trim from both ends (in place)
void trim(std::string& s);

Stop ParseAddStopQuery(const std::string_view stop);

std::tuple<std::string, std::vector<std::string_view>> ParseAddBusQuery(const std::string_view bus);

std::string_view ParseBusInfoQuery(const std::string_view query);

std::string_view ParseStopInfoQuery(const std::string_view query);

QueryType ConvertStringToQueryType(const std::string_view s);

std::vector<Query> ReadInput(std::istream& input);
