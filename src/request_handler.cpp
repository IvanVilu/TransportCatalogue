#include "request_handler.h"

RequestHandler::RequestHandler(const TransportCatalogue& db, 
    const renderer::MapRenderer& renderer) :
    db_(db),
    renderer_(renderer){}

std::optional<domain::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {

    auto bus = db_.FindBus(bus_name);
    if (bus == std::nullopt)
        return std::nullopt;

    domain::BusInfo bus_info;
    bus_info.name = bus_name;

    bus_info.stops_count = bus.value()->stops.size();

    bus_info.unique_stops_count = bus.value()->GetUniqueStopCount();

    bus_info.route_lenght = bus.value()->route_lenght;
    bus_info.geo_dist = bus.value()->geo_dist;


    return bus_info;
    return std::optional<domain::BusInfo>();
}

std::optional<domain::StopInfo> RequestHandler::GetStopStat(const std::string_view& stopname) const
{
    auto stop = db_.FindStop(stopname);
    if (stop == std::nullopt)
        return std::nullopt;

    domain::StopInfo stop_info;
    stop_info.name = stop.value()->name;
    stop_info.buses = db_.GetBusesForStop(stopname);

    return stop_info;
}

json::Node RequestHandler::MakeErrorDict(const int id) {
    return json::Builder{}.StartDict().
    Key("request_id").Value(id).
    Key("error_message").Value("not found").
    EndDict().Build();
}

json::Node RequestHandler::GetBusResponce(const domain::StatRequest& request) {
    auto bus_stat = GetBusStat(request.name);
    if (bus_stat) {
        domain::BusInfo stat = bus_stat.value();
        json::Node n = json::Builder{}.StartDict().
       // Key("curvature").Value(stat.GetCurvature()).
        Key("request_id").Value(request.id).
        Key("route_length").Value(stat.route_lenght).
        Key("stop_count").Value(stat.stops_count).
        Key("unique_stop_count").Value(stat.unique_stops_count).
        EndDict().Build();
        return n;
    }
    
    return MakeErrorDict(request.id);
}

json::Node RequestHandler::GetStopResponce(const domain::StatRequest& request) {
    auto stop_stat = GetStopStat(request.name);
    if (stop_stat) {
        domain::StopInfo stat = stop_stat.value();
        json::Array buses;
        for (const std::string_view& bus : stat.buses) {
            std::string bus_name(bus.begin(), bus.end());
            buses.push_back(json::Node(std::move(bus_name)));
        }
        return json::Builder{}.StartDict().
                             Key("request_id").Value(request.id).
                             Key("buses").Value(buses).
                                EndDict().
                        Build();
    }
    return MakeErrorDict(request.id);
}

json::Node RequestHandler::GetMapResponce(const domain::StatRequest& request) {
    svg::Document map = RenderMap();
    std::ostringstream strm;
    map.Render(strm);
    return json::Builder{}.StartDict().
                            Key("request_id").Value(request.id).
                            Key("map").Value(strm.str()).
                        EndDict().
                Build();
}

json::Document RequestHandler::ProcessStatRequests(const std::vector<domain::StatRequest> requests) {
    using namespace std::string_literals;

    json::Array root_array;
    root_array.reserve(requests.size());

    for (const domain::StatRequest& request : requests) {
        
        if (request.type == domain::StatRequestType::Bus) {
            root_array.push_back(GetBusResponce(request));
            
        } else if (request.type == domain::StatRequestType::Stop) {
            root_array.push_back(GetStopResponce(request));
            
        } else if (request.type == domain::StatRequestType::Map) {
            root_array.push_back(GetMapResponce(request));
      }
    }
    return json::Document(root_array);
}


svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSvgDocument(db_.GetBusnameToBus(), db_.GetStopToBusnames());
}





