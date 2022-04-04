#include "stat_reader.h"


void PrintBusInfo(std::ostream& output, const BusInfo& bus_info) {
    if(bus_info.stops_count == 0) {
        output << "Bus " << bus_info.name << ": not found" << std::endl;
    } else {
        
        output << "Bus " << bus_info.name << ": ";
        output << bus_info.stops_count << " stops on route, ";
        output << bus_info.unique_stops_count << " unique stops, ";
        output << std::setprecision(6) << bus_info.route_lenght << " route length, ";
        output << std::setprecision(6) << bus_info.GetCurvature() << " curvature";

        output << std::endl;
    }
}
void PrintStopInfo(std::ostream& output, const StopInfo& stop_info) {
    output << "Stop ";
   if(!stop_info.is_found) {
       output << stop_info.name << ": not found" << std::endl;
   } else {
       if(stop_info.buses.size() == 0) {
           output << stop_info.name << ": no buses" << std::endl;
       } else {
           output << stop_info.name << ": buses";
           
        for(const auto& busname : stop_info.buses) {
            output << " " << busname;
        }
        output << std::endl;
       }
   }
        
}