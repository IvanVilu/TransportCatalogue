#include "transport_catalogue.h"
#include "geo.h"

#include <cassert>
#include <algorithm>
#include <iomanip>
#include <unordered_set>

size_t StopHash::operator()(const domain::Stop& stop) const {
    std::hash<std::string> hasher;
    return hasher(stop.name);
}

template<typename T>
inline std::size_t PairHash::operator()(const std::pair<T*, T*> x) const {
    return hasher_(x.first) + hasher_(x.second) * 37;
}

void TransportCatalogue::AddStop(const domain::Stop& stop) {

    auto& stop_it = stops_.emplace_back(std::move(stop));
    stopname_to_stop_[stop_it.name] = &stop_it;
    stop_to_busnames_.insert(std::make_pair(&stop_it, std::set<std::string_view>{}));

}

void TransportCatalogue::AddBus(const domain::AddBusQuery& query) {
    auto& bus_it = buses_.emplace_back(domain::Bus(std::move(query.name), 0, 0.0, {}, query.is_roundtrip));

    for (size_t i = 0; i < query.stops.size(); ++i) {

        buses_.back().stops.push_back(stopname_to_stop_.at(query.stops[i]));
        stop_to_busnames_[stopname_to_stop_.at(query.stops[i])].insert(buses_.back().name);

        // ведём подсчет расстояний до i < stops_names.size() - 1, чтобыы не учитывать последнюю остновку
        if (i != query.stops.size() - 1) {
            auto stop_to_stop = stop_to_stop_distance_.find(std::make_pair(stopname_to_stop_.at(query.stops[i]),
                stopname_to_stop_.at(query.stops[i + 1])));
            if (stop_to_stop != stop_to_stop_distance_.end()) {
                bus_it.route_lenght += (*stop_to_stop).second;
            }
            else {
                stop_to_stop = stop_to_stop_distance_.find(std::make_pair(stopname_to_stop_.at(query.stops[i + 1]),
                    stopname_to_stop_.at(query.stops[i])));
                if (stop_to_stop != stop_to_stop_distance_.end()) {
                    bus_it.route_lenght += (*stop_to_stop).second;
                }
            }

            geo::Coordinates lhs = { stopname_to_stop_.at(query.stops[i])->latitude, stopname_to_stop_.at(query.stops[i])->longitude };
            geo::Coordinates rhs = { stopname_to_stop_.at(query.stops[i + 1])->latitude, stopname_to_stop_.at(query.stops[i + 1])->longitude };

            bus_it.geo_dist += geo::ComputeDistance(lhs, rhs);
        }
    }

    busname_to_bus_[bus_it.name] = &bus_it;

}

const std::optional<domain::Bus*> TransportCatalogue::FindBus(const std::string_view bus_name) const {
    if (busname_to_bus_.count(bus_name) == 0)
        return std::nullopt;
    return busname_to_bus_.at(bus_name);
}


const domain::StopInfo TransportCatalogue::GetStopInfo(const std::string_view stop_name) {
    domain::StopInfo stop_info;
    stop_info.name = stop_name;
    if (stopname_to_stop_.count(stop_name) == 0) {
        stop_info.is_found = false;
        return stop_info;
    }
    else if (stop_to_busnames_.at(stopname_to_stop_.at(stop_name)).empty()) {
        return stop_info;
    }
    else {
        stop_info.buses = stop_to_busnames_.at(stopname_to_stop_.at(stop_name));
    }
    return stop_info;
}


void TransportCatalogue::AddStopsDistances(const domain::AddStopQuery& query) {
    domain::Stop* stop_from = stopname_to_stop_.at(query.name);
    for (const auto& [stop_to_name, distance] : query.distances) {
        domain::Stop* stop_to;
        stop_to = stopname_to_stop_.at(stop_to_name);
        stop_to_stop_distance_.insert(std::make_pair(std::make_pair(stop_from, stop_to), distance));
    }

}

const std::unordered_map<std::string_view, domain::Bus*> TransportCatalogue::GetBusnameToBus() const {
    return busname_to_bus_;
}

const std::unordered_map<domain::Stop*, std::set<std::string_view>> TransportCatalogue::GetStopToBusnames() const
{
    return stop_to_busnames_;
}

const std::set<std::string_view> TransportCatalogue::GetBusesForStop(const std::string_view& stop) const {
    if (stop_to_busnames_.count(stopname_to_stop_.at(stop)) == 0)
        return std::set<std::string_view>();
    return stop_to_busnames_.at(stopname_to_stop_.at(stop));
}

const std::optional<domain::Stop*> TransportCatalogue::FindStop(const std::string_view& stopname) const {
    if (stopname_to_stop_.count(stopname) == 0)
        return std::nullopt;
    return stopname_to_stop_.at(stopname);
}
