#pragma once

#include "json_reader.h"
#include "domain.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string_view>
#include <set>
#include <optional>



struct StopHash {
    size_t operator() (const domain::Stop& stop) const;
};


struct PairHash {
public:
    template<typename T>
    std::size_t operator()(const std::pair<T*, T*> x) const;
private:
    std::hash<const void*> hasher_;
};

class TransportCatalogue {
public:

    void AddStop(const domain::Stop& stop);
   
    void AddBus(const AddBusQuery& query);

    const std::optional<domain::Bus*> FindBus(const std::string_view bus_name) const;
    const std::optional<domain::Stop*> FindStop(const std::string_view& stopname) const;
   
    const StopInfo GetStopInfo(const std::string_view stop_name);
   
    void AddStopsDistances(const AddStopQuery& query);

    const std::unordered_map<std::string_view, domain::Bus*> GetBusnameToBus() const;

    const std::unordered_map<domain::Stop*, std::set<std::string_view>> GetStopToBusnames() const;

    const std::set<std::string_view> GetBusesForStop(const std::string_view& stop) const;

private:
    std::deque<domain::Stop> stops_;
    std::deque<domain::Bus> buses_;
    std::unordered_map<std::string_view, domain::Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, domain::Bus*> busname_to_bus_;
    std::unordered_map<domain::Stop*, std::set<std::string_view>> stop_to_busnames_;
    std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, PairHash> stop_to_stop_distance_;

};