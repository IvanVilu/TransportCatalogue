
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"

#include "json.h"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_set>

using namespace std;

int main() {

    json::Document document_input = json::Load(std::cin);
    
   vector<AddBusQuery> add_bus_queries;
    vector<AddStopQuery> add_stop_queries;

    vector<StatRequest> stat_requests;
    renderer::RenderSettings render_settings;
    for (const auto& [key, value] : document_input.GetRoot().AsMap()) {
        if (key == "base_requests") {
            for (const auto& query : value.AsArray()) {
                auto query_as_map = query.AsMap();
                if (query_as_map.at("type").AsString() == "Bus") {
                    add_bus_queries.push_back(ReadBusQuery(query_as_map));
                }
                else {
                    add_stop_queries.push_back(ReadAddStopQuery(query_as_map));
                }
            }
        }
        else if (key == "stat_requests") {
            stat_requests = ReadStatRequests(value.AsArray());
            
        }
        else if (key == "render_settings") {
            render_settings = ParseRenderSettings(json::Document(value));
        }
    }

    TransportCatalogue catalogue;
    std::vector<geo::Coordinates> coordinares(add_stop_queries.size());

    std::transform(add_stop_queries.begin(), add_stop_queries.end(), coordinares.begin(),
        [](AddStopQuery& query) {
            return query.coordinates;
        });

    renderer::MapRenderer renderer(render_settings);
    RequestHandler handler(catalogue, renderer);


    for (const AddStopQuery& query : add_stop_queries) {
        catalogue.AddStop(ParseAddStopQuery(query));
    }

    for (const AddStopQuery& query : add_stop_queries) {
        catalogue.AddStopsDistances(query);
    }

    for (const AddBusQuery& query : add_bus_queries) {
        catalogue.AddBus(query);
    }


    auto response_doc = handler.ProcessStatRequests(stat_requests);
    json::Print(response_doc, std::cout);
}

