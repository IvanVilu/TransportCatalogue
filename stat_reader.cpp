#include "stat_reader.h"


void PrintBusInfo(const BusInfo& bus_info) {
    if(bus_info.stops_count == 0) {
        std::cout << "Bus " << bus_info.name << ": not found" << std::endl;
    } else {
        
        std::cout << "Bus " << bus_info.name << ": ";
        std::cout << bus_info.stops_count << " stops on route, ";
        std::cout << bus_info.unique_stops_count << " unique stops, ";
        std::cout << std::setprecision(6) << bus_info.route_lenght << " route length, ";
        std::cout << std::setprecision(6) << bus_info.GetCurvature() << " curvature";

        std::cout << std::endl;
    }
}
void PrintStopInfo(const StopInfo& stop_info) {
     std::cout << "Stop ";
   if(!stop_info.is_found) {
        std::cout << stop_info.name << ": not found" << std::endl;
   } else {
       if(stop_info.buses.size() == 0) {
         std::cout << stop_info.name << ": no buses" << std::endl;
       } else {
           std::cout << stop_info.name << ": buses";
           
        for(const auto& busname : stop_info.buses) {
            std::cout << " " << busname;
        }
        std::cout << std::endl;
       }
   }
        
}