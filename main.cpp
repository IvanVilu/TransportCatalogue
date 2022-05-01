
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
    ifstream input("input1.json");
    if(!input.is_open()) {
        throw logic_error("File wasn't open");
    }
    json::Document document_input = json::Load(input);
   // json::Document document_input = json::Load(std::cin);
    
    
    vector<domain::AddBusQuery> add_bus_queries;
    vector<domain::AddStopQuery> add_stop_queries;

    vector<domain::StatRequest> stat_requests;
    renderer::RenderSettings render_settings;
    JSON_Reader reader;
    for (const auto& [key, value] : document_input.GetRoot().AsDict()) {
        if (key == "base_requests") {
            for (const auto& query : value.AsArray()) {
                auto query_as_map = query.AsDict();
                if (query_as_map.at("type").AsString() == "Bus") {
                    add_bus_queries.push_back(reader.ReadBusQuery(query_as_map));
                }
                else {
                    add_stop_queries.push_back(reader.ReadAddStopQuery(query_as_map));
                }
            }
        }
        else if (key == "stat_requests") {
            stat_requests = reader.ReadStatRequests(value.AsArray());
            
        }
        else if (key == "render_settings") {
            render_settings = reader.ParseRenderSettings(json::Document(value));
        }
    }

    TransportCatalogue catalogue;
    std::vector<geo::Coordinates> coordinares(add_stop_queries.size());

    std::transform(add_stop_queries.begin(), add_stop_queries.end(), coordinares.begin(),
                   [](domain::AddStopQuery& query) {
            return query.coordinates;
        });

    renderer::MapRenderer renderer(render_settings);
    RequestHandler handler(catalogue, renderer);


    for (const domain::AddStopQuery& query : add_stop_queries) {
        catalogue.AddStop(reader.ParseAddStopQuery(query));
    }

    for (const domain::AddStopQuery& query : add_stop_queries) {
        catalogue.AddStopsDistances(query);
    }

    for (const domain::AddBusQuery& query : add_bus_queries) {
        catalogue.AddBus(query);
    }

    auto response_doc = handler.ProcessStatRequests(stat_requests);
    json::Print(response_doc, std::cout);
}
