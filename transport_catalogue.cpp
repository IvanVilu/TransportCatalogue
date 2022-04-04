#include "transport_catalogue.h"
#include "geo.h"

#include <cassert>
#include <algorithm>
#include <iomanip>
#include <unordered_set>

size_t StopHash::operator()(const Stop& stop) const
{
    std::hash<std::string> hasher;
    return hasher(stop.name);
}

Bus::Bus(const std::string& name_, int route_lenght_, double geo_dist_, std::vector<Stop*> stops_) :
    name(name_),
    route_lenght(route_lenght_),
    geo_dist(geo_dist_),
    stops(stops_) {}

size_t TransportCatalogue::GetUniqueStopCount(const Bus* bus) {
    std::unordered_set<std::string_view> res;
    for(const Stop* stop : bus->stops)
        res.insert(stop->name);
    return res.size();
}

void TransportCatalogue::AddStop(const Stop& stop) {
    
    auto& stop_it = stops_.emplace_back(std::move(stop));
    stopname_to_stop_[stop_it.name] = &stop_it;
    stop_to_busnames_.insert(std::make_pair(&stop_it, std::set<std::string_view>{}));
    
    
}

Stop* TransportCatalogue::FindStop(const std::string_view stop_name) {
    assert(stopname_to_stop_.count(stop_name) != 0);
    return stopname_to_stop_.at(stop_name);
}

void TransportCatalogue::AddBus(std::string& bus_name, const std::vector<std::string_view> stops_names) {

    auto& bus_it = buses_.emplace_back(Bus(std::move(bus_name), 0, 0.0, {}));

    for (size_t i = 0; i < stops_names.size() - 1; ++i) {

        buses_.back().stops.push_back(stopname_to_stop_.at(stops_names[i]));
        stop_to_busnames_[stopname_to_stop_.at(stops_names[i])].insert(buses_.back().name);

        // ведёмм подсчет расстояний до i < stops_names.size() - 1, чтобыы не учитывать последнюю остновку
        if (i != stops_names.size() - 1) {
            auto stop_to_stop = stop_to_stop_distance_.find(std::make_pair(stopname_to_stop_.at(stops_names[i]),
                stopname_to_stop_.at(stops_names[i + 1])));
            if (stop_to_stop != stop_to_stop_distance_.end()) {
                bus_it.route_lenght += (*stop_to_stop).second;
            }
            else {
                stop_to_stop = stop_to_stop_distance_.find(std::make_pair(stopname_to_stop_.at(stops_names[i + 1]),
                    stopname_to_stop_.at(stops_names[i])));
                if (stop_to_stop != stop_to_stop_distance_.end()) {
                    bus_it.route_lenght += (*stop_to_stop).second;
                }
            }

            Coordinates lhs = { stopname_to_stop_.at(stops_names[i])->latitude, stopname_to_stop_.at(stops_names[i])->longitude };
            Coordinates rhs = { stopname_to_stop_.at(stops_names[i + 1])->latitude, stopname_to_stop_.at(stops_names[i + 1])->longitude };

            bus_it.geo_dist += ComputeDistance(lhs, rhs);
        }
    }


    busname_to_bus_[bus_it.name] = &bus_it;
}

Bus* TransportCatalogue::FindBus(const std::string_view bus_name) {
    return busname_to_bus_.at(bus_name);
}

const BusInfo TransportCatalogue::GetBusInfo(const std::string_view bus_name) {
    BusInfo bus_info;
    bus_info.name = bus_name;
    if(busname_to_bus_.count(bus_name) == 0) {
        bus_info.stops_count = 0;
        return bus_info;
    } else {
        
        auto bus = FindBus(bus_name);
        bus_info.stops_count = bus->stops.size();
        
        bus_info.unique_stops_count = GetUniqueStopCount(bus);
        
        bus_info.route_lenght = bus->route_lenght;
        bus_info.geo_dist = bus->geo_dist;
        
    }
    return bus_info;
}

const StopInfo TransportCatalogue::GetStopInfo(const std::string_view stop_name) {
    StopInfo stop_info;
    stop_info.name = stop_name;
    if(stopname_to_stop_.count(stop_name) == 0) {
        stop_info.is_found = false;
        return stop_info;
    } else if (stop_to_busnames_.at(stopname_to_stop_.at(stop_name)).empty()) {
        return stop_info;
    } else {
        stop_info.buses = stop_to_busnames_.at(stopname_to_stop_.at(stop_name));
    }
    return stop_info;
}

void TransportCatalogue::AddStopsDistances(const std::string_view query) {
    
    size_t first_stop_from_pos = query.find("Stop") + 4;
    first_stop_from_pos = query.find_first_not_of(' ', first_stop_from_pos);
    size_t colon_pos = query.find(':');
    Stop* stop_from = stopname_to_stop_.at(query.substr(first_stop_from_pos, colon_pos - first_stop_from_pos));
    
    size_t start_pos = query.find_first_of(',', colon_pos);
    start_pos = query.find_first_of(',', start_pos+1);
    start_pos = query.find_first_not_of(' ', start_pos + 1);
    while(true) {
    // запрос без добавления расстояний
    if(start_pos == 0) break;
        
    size_t m_pos = query.find('m', start_pos);
    int distance =  stoi(std::string(query.substr(start_pos, m_pos - start_pos)));
    
    // позиция после to
    size_t after_to_pos = query.find("to ", m_pos) + 3;
    
    // позиция первой буквы остановки
    size_t first_stop_pos = query.find_first_not_of(' ', after_to_pos);
    
    // позиция последней буквы остановки
    size_t last_stop_pos = query.find_first_of(',', first_stop_pos + 1);
    
    Stop* stop_to;
    if(last_stop_pos != query.npos) {
        stop_to = stopname_to_stop_.at(query.substr(first_stop_pos, last_stop_pos - first_stop_pos));
        stop_to_stop_distance_.insert(std::make_pair(std::make_pair(stop_from, stop_to), distance));
    } else {
        std::string to_name = std::string(query.substr(first_stop_pos, query.size() - first_stop_pos));
        stop_to = stopname_to_stop_.at(to_name);
        stop_to_stop_distance_.insert(std::make_pair(std::make_pair(stop_from, stop_to), distance));
        break;
    }
        
        start_pos = query.find_first_not_of(' ', last_stop_pos + 1);
    }
}
