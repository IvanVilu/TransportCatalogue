
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
#include <filesystem>

using namespace std;

int main(int argc, char** argv) {
    try {
        if (argc < 4)
            throw logic_error("You have to pass input and output files as an argument");
        std::string input_name(argv[1]);

        ifstream input(input_name);

        if (!input.is_open()) {
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

        //JSON response
        std::string output_name(argv[2]);
        std::ofstream output(output_name);
        auto response_doc = handler.ProcessStatRequests(stat_requests);
        json::Print(response_doc, output);
        output.close();

        //SVG response
        std::string output_map_name(argv[3]);
        std::ofstream output_map(output_map_name);
        svg::Document map = handler.RenderMap();
        map.Render(output_map);
        output_map.close();
    }
    catch (std::logic_error& e) {
        std::cout << e.what() << std::endl;
    }
    catch (std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    }
    catch (json::ParsingError& e) {
        std::cout << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Unknown error" << std::endl;
    }
   
    return 0;
}
