#include "geo.h"

namespace geo {
    Coordinates::Coordinates(const double lat_,const double lng_) :
        lat(lat_),
        lng(lng_) {}

    bool Coordinates::operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }

    bool Coordinates::operator!=(const Coordinates& other) const {
        return !(*this == other);
    }

} // namespace geo
