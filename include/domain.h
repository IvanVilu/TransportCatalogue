#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <set>

#include "geo.h"

namespace domain {

    struct Stop {
        std::string name;
        double latitude;
        double longitude;
    };
    
    struct Bus {
        Bus(const std::string& name_, int route_lenght_, double     geo_dist_,
            std::vector<Stop*> stops_, bool is_roundtrip_);
    
        size_t GetUniqueStopCount();
    
        std::string name;
        int route_lenght = 0;
        double geo_dist = 0;
        std::vector<Stop*> stops;
        bool is_roundtrip;
    };

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


}

