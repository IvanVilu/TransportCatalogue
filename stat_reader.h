#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>

struct BusInfo {
  std::string_view name;
  int stops_count;
  int unique_stops_count;
  int route_lenght;
  double geo_dist;
  
double GetCurvature() const {
      return  route_lenght / geo_dist;
  }
};

struct StopInfo {
    std::string_view name;
    std::set<std::string_view> buses;
    
    bool is_found = true;
};

void PrintBusInfo(const BusInfo& bus_info);
void PrintStopInfo(const StopInfo& stop_info);

