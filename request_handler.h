#pragma once

#include <optional>
#include <unordered_set>
#include <set>
#include <memory>
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "domain.h"
#include "json.h"

class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;
    std::optional<StopInfo> GetStopStat(const std::string_view& stopname) const;
    json::Document ProcessStatRequests(const std::vector<StatRequest> requests);

    svg::Document RenderMap() const;

private:
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
