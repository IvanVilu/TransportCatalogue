#pragma once
#include <string>
#include <vector>
#include <unordered_set>

namespace domain {
    struct Stop {
        std::string name;
        double latitude;
        double longitude;
    };

    struct Bus {
        Bus(const std::string& name_, int route_lenght_, double geo_dist_,
            std::vector<Stop*> stops_, bool is_roundtrip_);

        size_t GetUniqueStopCount();

        std::string name;
        int route_lenght = 0;
        double geo_dist = 0;
        std::vector<Stop*> stops;
        bool is_roundtrip;
    };
}

