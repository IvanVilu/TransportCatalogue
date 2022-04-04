#pragma once

#include "stat_reader.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string_view>
#include <set>

struct Stop {
    std::string name;
    double latitude;
    double longitude;
};

struct StopHash {
    
   size_t operator() (const Stop& stop) const {
    std::hash<std::string> hasher;
     return hasher(stop.name);
   }
};

struct Bus {
    Bus(const std::string& name_, std::vector<Stop*> stops_) :
    name(name_),
    stops(stops_) {
        
    };
    
    std::string name;
    std::vector<Stop*> stops;
};

struct PairHash {
public:
  template<typename T>
  std::size_t operator()(const std::pair<T*, T*> x) const
  {
    return hasher_(x.first) + hasher_(x.second) * 37;
  }
private:
    std::hash<const void*> hasher_;
};

class TransportCatalogue {
public:
    
    void AddStop(const Stop& stop);
    Stop* FindStop(const std::string_view stop_name);
    void AddBus(std::string& bus_name, const std::vector<std::string_view> stops_names);
    Bus* FindBus(const std::string_view bus_name);
    const BusInfo GetBusInfo(const std::string_view bus_name);
    const StopInfo GetStopInfo(const std::string_view stop_name);
    void AddStopsDistances(const std::string_view query);

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<Stop*, std::set<std::string_view>> stop_to_busnames_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, PairHash> stop_to_stop_distance_;
    
    size_t GetUniqueStopCount(const Bus* bus);
    
};
