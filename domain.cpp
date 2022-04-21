#include "domain.h"

namespace domain {
    Bus::Bus(const std::string& name_, int route_lenght_, double geo_dist_,
        std::vector<Stop*> stops_, bool is_roundtrip_) :
        name(name_),
        route_lenght(route_lenght_),
        geo_dist(geo_dist_),
        stops(stops_),
        is_roundtrip(is_roundtrip_) {}

    size_t Bus::GetUniqueStopCount() {
        std::unordered_set<std::string_view> res;
        for (const domain::Stop* stop : stops)
            res.insert(stop->name);
        return res.size();
    }

}
