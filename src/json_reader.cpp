#include "json_reader.h"

domain::StatRequestType JSON_Reader::StringToStatRequestType(std::string_view s) {
    if (s == "Bus")
        return domain::StatRequestType::Bus;
    else if (s == "Stop")
        return domain::StatRequestType::Stop;
    else if (s == "Map")
        return domain::StatRequestType::Map;

    throw std::invalid_argument("Cannot convert string request to StatRequestType");
}

domain::Stop JSON_Reader::ParseAddStopQuery(const domain::AddStopQuery& query)
{
    return { std::move(query.name),
        query.coordinates.lat, query.coordinates.lng };
}

renderer::RenderSettings JSON_Reader::ParseRenderSettings(json::Document render_settings)
{
    renderer::RenderSettings res;
    auto settings_map = render_settings.GetRoot().AsDict();
    if (settings_map.count("width"))
        res.width = settings_map.at("width").AsDouble();

    if(settings_map.count("height"))
        res.height = settings_map.at("height").AsDouble();

    if (settings_map.count("padding"))
        res.padding = settings_map.at("padding").AsDouble();

    if (settings_map.count("line_width"))
        res.line_width = settings_map.at("line_width").AsDouble();

    if (settings_map.count("stop_radius"))
        res.stop_radius = settings_map.at("stop_radius").AsDouble();

    if (settings_map.count("bus_label_font_size"))
        res.bus_label_font_size = settings_map.at("bus_label_font_size").AsInt();

    if (settings_map.count("bus_label_offset"))
        res.bus_label_offset = { settings_map.at("bus_label_offset").AsArray()[0].AsDouble(), 
        settings_map.at("bus_label_offset").AsArray()[1].AsDouble() };

    if (settings_map.count("stop_label_font_size"))
        res.stop_label_font_size = settings_map.at("stop_label_font_size").AsInt();

    if (settings_map.count("stop_label_offset"))
        res.stop_label_offset = { settings_map.at("stop_label_offset").AsArray()[0].AsDouble(),
                                  settings_map.at("stop_label_offset").AsArray()[1].AsDouble() };

    if (settings_map.count("underlayer_color")) 
        res.underlayer_color = ParseColor(settings_map.at("underlayer_color"));

    if (settings_map.count("underlayer_width"))
        res.underlayer_width = settings_map.at("underlayer_width").AsDouble();

    if (settings_map.count("color_palette")) {
        std::vector<svg::Color> colors;
        for (const json::Node& node : settings_map.at("color_palette").AsArray()) {
            colors.push_back(ParseColor(node));
        }
        res.color_palette = std::move(colors);
    }
        
    return res;
}

svg::Color JSON_Reader::ParseColor(const json::Node& node) {
    if (node.IsArray()) {
        if (node.AsArray().size() == 3) {
            return svg::Rgb{ static_cast<uint8_t>(node.AsArray()[0].AsInt()),
                             static_cast<uint8_t>(node.AsArray()[1].AsInt()),
                             static_cast<uint8_t>(node.AsArray()[2].AsInt()) };
        }
        else {
            return svg::Rgba{ static_cast<uint8_t>(node.AsArray()[0].AsInt()),
                              static_cast<uint8_t>(node.AsArray()[1].AsInt()),
                              static_cast<uint8_t>(node.AsArray()[2].AsInt()),
                              node.AsArray()[3].AsDouble() };
        }
    }
    else if (node.IsString()) {
        return node.AsString();
    }
    else {
        throw std::invalid_argument("Cannot convert Node to Color");
    }
}

domain::AddBusQuery JSON_Reader::ReadBusQuery(json::Dict& query) {
    domain::AddBusQuery bus_query;

    bus_query.stops.resize(query.at("is_roundtrip").AsBool() ?
        query.at("stops").AsArray().size() :
        query.at("stops").AsArray().size() * 2 - 1, "");

    bus_query.name = std::move(query.at("name").AsString());
    bus_query.is_roundtrip = query.at("is_roundtrip").AsBool();

    for (size_t i = 0; i < query.at("stops").AsArray().size(); ++i) {
        bus_query.stops[i] = query.at("stops").AsArray()[i].AsString();
        if (!query.at("is_roundtrip").AsBool()) {
            size_t back = query.at("stops").AsArray().size() - 1 - i;
            back = query.at("stops").AsArray().size() - 1 + back;
            bus_query.stops[back] = (std::move(query.at("stops").AsArray()[i].AsString()));
        }
    }
    return bus_query;
}

domain::AddStopQuery JSON_Reader::ReadAddStopQuery(json::Dict& query) {
    domain::AddStopQuery stop_query;
    stop_query.name = std::move(query.at("name").AsString());
    stop_query.coordinates.lat = query.at("latitude").AsDouble();
    stop_query.coordinates.lng = query.at("longitude").AsDouble();
    if (query.count("road_distances") != 0) {
        for (const auto& [stop_name, dist] : query.at("road_distances").AsDict()) {
            stop_query.distances.insert({ stop_name, dist.AsInt() });
        }
    }
    
    return stop_query;
}

std::vector<domain::StatRequest> JSON_Reader::ReadStatRequests(const json::Array& query) {
    std::vector<domain::StatRequest> res;
    for (const auto& query : query) {
        auto query_as_map = query.AsDict();
        domain::StatRequest stat_request(query_as_map.at("id").AsInt(),
            StringToStatRequestType(query_as_map.at("type").AsString()),
            query_as_map.find("name") != query_as_map.end() ?
            query_as_map.at("name").AsString() : ""); // Empty name field for Map Request

        res.push_back(std::move(stat_request));
    }
    return res;
}
