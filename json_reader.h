#pragma once

//TODO Rework

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <exception>
#include <cstdint>

#include "domain.h"
#include "map_renderer.h"
#include "json.h"

class JSON_Reader {
public:
    

    domain::Stop ParseAddStopQuery(const domain::AddStopQuery& query);

    renderer::RenderSettings ParseRenderSettings(json::Document render_settings);

    

    domain::AddBusQuery ReadBusQuery(json::Dict& query);

    domain::AddStopQuery ReadAddStopQuery(json::Dict& query);

    std::vector<domain::StatRequest> ReadStatRequests(const json::Array& query);
private:
    
    svg::Color ParseColor(const json::Node& node);
    
    domain::StatRequestType StringToStatRequestType(std::string_view s);

};
