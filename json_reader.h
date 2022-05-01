#pragma once

//TODO Rework

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <exception>
#include <cstdint>

#include "domain.h"
#include "map_renderer.h"
#include "json.h"

enum class StatRequestType {
    Stop,
    Bus,
    Map
};

struct Response {
    int id;
    StatRequestType type;
};

struct BusInfo : public Response {
    std::string_view name;
    int stops_count;
    int unique_stops_count;
    int route_lenght;
    double geo_dist;

    double GetCurvature() const;
};

struct StopInfo : public Response {
    std::string_view name;
    std::set<std::string_view> buses;

    bool is_found = true;
};

struct StatRequest {
    StatRequest(int id_, StatRequestType type_, const std::string& name_);
    
    int id;
    StatRequestType type;
    const std::string name;
};

StatRequestType StringToStatRequestType(std::string_view s);

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

struct AddBusQuery {
    std::string name;
    std::vector<std::string> stops;
    bool is_roundtrip;
};

struct AddStopQuery {
    std::string name;
    geo::Coordinates coordinates;
    std::unordered_map<std::string, int> distances;
};



domain::Stop ParseAddStopQuery(const AddStopQuery& query);

std::string_view ParseStopInfoQuery(const std::string_view query);

QueryType ConvertStringToQueryType(const std::string_view s);

renderer::RenderSettings ParseRenderSettings(json::Document render_settings);

svg::Color ParseColor(const json::Node& node);

AddBusQuery ReadBusQuery(json::Dict& query);

AddStopQuery ReadAddStopQuery(json::Dict& query);

std::vector<StatRequest> ReadStatRequests(const json::Array& query);
