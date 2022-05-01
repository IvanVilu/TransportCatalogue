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

    for (const StatRequest& request : requests) {
        if (request.type == StatRequestType::Bus) {
            auto bus_stat = GetBusStat(request.name);
            if (bus_stat) {
                BusInfo stat = bus_stat.value();
                json::Node n = json::Builder{}.StartDict().
                Key("curvature").Value(stat.GetCurvature()).
                Key("request_id").Value(request.id).
                Key("route_length").Value(stat.route_lenght).
                Key("stop_count").Value(stat.stops_count).
                Key("unique_stop_count").Value(stat.unique_stops_count).
                EndDict().Build();
                root_array.push_back(n);
            }
            else {
                json::Node n = json::Builder{}.StartDict().
                Key("request_id").Value(request.id).
                Key("error_message").Value("not found").
                EndDict().Build();
                
                root_array.push_back(n);
            }


        }
        else if (request.type == StatRequestType::Stop) {
            auto stop_stat = GetStopStat(request.name);
            if (stop_stat) {
                StopInfo stat = stop_stat.value();
                json::Array buses;
                for (const std::string_view& bus : stat.buses) {
                    std::string bus_name(bus.begin(), bus.end());
                    buses.push_back(json::Node(std::move(bus_name)));
                }
                root_array.push_back(json::Builder{}.StartDict().
                                     Key("request_id").Value(request.id).
                                     Key("buses").Value(buses).EndDict().Build());
            } else {
                root_array.push_back(json::Builder{}.StartDict().
                                     Key("request_id").Value(request.id).
                                     Key("error_message").Value("not found").EndDict().Build());
            }
        }
        else if (request.type == StatRequestType::Map) {
           svg::Document map = RenderMap();
           std::ostringstream strm;
           map.Render(strm);
           root_array.push_back(json::Builder{}.StartDict().
                                Key("request_id").Value(request.id).
                                Key("map").Value(strm.str()).EndDict().Build());
        }
    }
    return json::Document(root_array);
}


svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSvgDocument(db_.GetBusnameToBus(), db_.GetStopToBusnames());
}


