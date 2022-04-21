#include "request_handler.h"

RequestHandler::RequestHandler(const TransportCatalogue& db, 
    const renderer::MapRenderer& renderer) :
    db_(db),
    renderer_(renderer){}

std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {

    auto bus = db_.FindBus(bus_name);
    if (bus == std::nullopt)
        return std::nullopt;

    BusInfo bus_info;
    bus_info.name = bus_name;

    bus_info.stops_count = bus.value()->stops.size();

    bus_info.unique_stops_count = bus.value()->GetUniqueStopCount();

    bus_info.route_lenght = bus.value()->route_lenght;
    bus_info.geo_dist = bus.value()->geo_dist;


    return bus_info;
    return std::optional<BusInfo>();
}

std::optional<StopInfo> RequestHandler::GetStopStat(const std::string_view& stopname) const
{
    auto stop = db_.FindStop(stopname);
    if (stop == std::nullopt)
        return std::nullopt;

    StopInfo stop_info;
    stop_info.name = stop.value()->name;
    stop_info.buses = db_.GetBusesForStop(stopname);

    return stop_info;
}

json::Document RequestHandler::ProcessStatRequests(const std::vector<StatRequest> requests) {
    using namespace std::string_literals;

    json::Array root_array;
    root_array.reserve(requests.size());

    std::pair<std::string, json::Node> err_not_found = 
        std::make_pair("error_message", json::Node("not found"s));

    for (const StatRequest& request : requests) {
        if (request.type == StatRequestType::Bus) {
            auto bus_stat = GetBusStat(request.name);
            if (bus_stat) {
                BusInfo stat = bus_stat.value();
                json::Dict bus;
                bus.insert({ "curvature", json::Node(stat.GetCurvature()) });
                bus.insert({ "request_id", json::Node(request.id) });
                bus.insert({ "route_length", json::Node(static_cast<double>(stat.route_lenght)) });
                bus.insert({ "stop_count", json::Node(stat.stops_count) });
                bus.insert({ "unique_stop_count", json::Node(stat.unique_stops_count) });
                root_array.push_back(json::Node(std::move(bus)));
            }
            else {
                json::Dict error;
                error.insert({ "request_id", request.id });
                error.insert(err_not_found);
                root_array.push_back(json::Node(std::move(error)));
            }


        }
        else if (request.type == StatRequestType::Stop) {
            auto stop_stat = GetStopStat(request.name);
            if (stop_stat) {
                StopInfo stat = stop_stat.value();
                json::Dict stop;
                stop.insert({ "request_id", request.id });
                json::Array buses;
                for (const std::string_view& bus : stat.buses) {
                    std::string bus_name(bus.begin(), bus.end());
                    buses.push_back(json::Node(std::move(bus_name)));
                }
                stop.insert({ "buses", json::Node(std::move(buses)) });
                root_array.push_back(json::Node(std::move(stop)));
            } else {
                json::Dict error;
                error.insert({ "request_id", request.id });
                error.insert(err_not_found);
                root_array.push_back(json::Node(std::move(error)));
            }
        }
        else if (request.type == StatRequestType::Map) {
           json::Dict map_responce;
           svg::Document map = RenderMap();
           std::ostringstream strm;
           map.Render(strm);
           map_responce.insert({ "request_id", request.id });
           map_responce.insert({ "map", strm.str() });
           root_array.push_back(map_responce);
        }
    }
    return json::Document(root_array);
}


svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSvgDocument(db_.GetBusnameToBus(), db_.GetStopToBusnames());
}


