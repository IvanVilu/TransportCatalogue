#include "map_renderer.h"

inline const double EPSILON = 1e-6;

namespace renderer {

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    template <typename PointInputIt>
    SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
        double max_height, double padding)
        : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, right_it]
            = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
            return lhs.lng < rhs.lng;
                });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it]
            = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
            return lhs.lat < rhs.lat;
                });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
    }

    MapRenderer::MapRenderer(RenderSettings& render_settings) :
		render_settings_(std::move(render_settings)) {}

    std::vector<svg::Polyline> MapRenderer::GetBusLines(const std::vector<domain::Bus*>& buses, const SphereProjector& sp) const {
        std::vector<svg::Polyline> res;
        std::vector<svg::Color> palette = render_settings_.color_palette.value(); // TODO Check optional
        unsigned color_index = 0;
        for (const domain::Bus* bus : buses) {

            svg::Polyline polyline = svg::Polyline().SetFillColor(svg::NoneColor).SetStrokeColor(palette[color_index])
                .SetStrokeWidth(render_settings_.line_width.value()).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            for (const domain::Stop* stop : bus->stops) {
                polyline.AddPoint(sp({ stop->latitude, stop->longitude }));
            }

            res.push_back(polyline);

            if (color_index == palette.size() - 1) {
                color_index = 0;
            }
            else {
                ++color_index;
            }
        }
        return res;
    }

    std::vector<svg::Text> MapRenderer::GetBusLabels(const std::vector<domain::Bus*>& buses, const SphereProjector& sp) const {
        std::vector<svg::Text> res;
        std::vector<svg::Color> palette = render_settings_.color_palette.value();
        unsigned color_index = 0;
        for (const domain::Bus* bus : buses) {
            svg::Text text = svg::Text().SetFillColor(palette[color_index]).
                SetOffset(render_settings_.bus_label_offset.value()).
                SetFontSize(render_settings_.bus_label_font_size.value()).
                SetFontFamily("Verdana").
                SetFontWeight("bold").SetData(bus->name).
                SetPosition(sp({ bus->stops.front()->latitude, bus->stops.front()->longitude }));

            auto under_text = text;
            under_text.SetFillColor(render_settings_.underlayer_color.value()).
                SetStrokeColor(render_settings_.underlayer_color.value()).
                SetStrokeWidth(render_settings_.underlayer_width.value()).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            
            res.push_back(under_text);
            res.push_back(text);
            
            if (color_index == palette.size() - 1) {
                color_index = 0;
            }
            else {
                ++color_index;
            }

            if (!bus->is_roundtrip) {
                if (bus->stops.front() == bus->stops[bus->stops.size() / 2]) continue;
                geo::Coordinates second_coords = { bus->stops[bus->stops.size() / 2]->latitude,
                                             bus->stops[bus->stops.size() / 2]->longitude };
                auto second_text = text;
                second_text.SetPosition(sp(second_coords));

                auto second_under_text = under_text;
                second_under_text.SetPosition(sp(second_coords));

                res.push_back(second_under_text);
                res.push_back(second_text);
            }

           
        }
        return res;
    }

    std::vector<svg::Circle> MapRenderer::GetStopPoints(const std::vector<domain::Stop*>& stops, const SphereProjector& sp) const {
        std::vector<svg::Circle> res;

        for (const auto& stop : stops) {
                svg::Circle circle = svg::Circle().SetCenter(sp({ stop->latitude, stop->longitude })).
                    SetRadius(render_settings_.stop_radius.value()).SetFillColor("white");
                res.push_back(circle);
           
        }
        return res;
    }

    std::vector<svg::Text> MapRenderer::GetStopLabels(const std::vector<domain::Stop*>& stops, const SphereProjector& sp) const {
        std::vector<svg::Text> res;

        for (const auto& stop : stops) {
             svg::Text text = svg::Text().SetFillColor("black").
                 SetPosition(sp({ stop->latitude, stop->longitude })).
                 SetOffset(render_settings_.stop_label_offset.value()).
                 SetFontSize(render_settings_.stop_label_font_size.value()).
                 SetFontFamily("Verdana").
                 SetData(stop->name);

             svg::Text under_text = text;

             under_text.SetFillColor(render_settings_.underlayer_color.value()).
                 SetStrokeColor(render_settings_.underlayer_color.value()).
                 SetStrokeWidth(render_settings_.underlayer_width.value()).
                 SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                 SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

             res.push_back(under_text);
             res.push_back(text);
        }

        return res;
    }

    svg::Document MapRenderer::GetSvgDocument(const std::unordered_map<std::string_view, domain::Bus*>& buses,
        const std::unordered_map<domain::Stop*, std::set<std::string_view>>& stops) const
    {
        std::vector<domain::Bus*> buses_sorted;
        buses_sorted.reserve(buses.size());

        for (const auto& [busname, bus] : buses) {
            if (!bus->stops.empty())
                buses_sorted.push_back(bus);
        }

        std::sort(buses_sorted.begin(), buses_sorted.end(), [](const domain::Bus* lhs, const domain::Bus* rhs) {
            return lhs->name < rhs->name;
            });

        std::vector<domain::Stop*> stops_sorted;
        for (const auto& [stop, busnames] : stops) {
            if(!busnames.empty())
                stops_sorted.push_back(stop);
        }

        std::sort(stops_sorted.begin(), stops_sorted.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->name < rhs->name;
        });

        std::vector<geo::Coordinates> coordinates(stops_sorted.size());
        std::transform(stops_sorted.begin(), stops_sorted.end(), coordinates.begin(),
            [](const domain::Stop* stop) { return geo::Coordinates(stop->latitude, stop->longitude); });

        SphereProjector sp(coordinates.begin(), coordinates.end(),
            render_settings_.width.value(), render_settings_.height.value(), render_settings_.padding.value());

        svg::Document result;
        
        for (const auto& line : GetBusLines(buses_sorted, sp)) {
            result.Add(line);
        }
        for (const auto& text : GetBusLabels(buses_sorted, sp)) {
            result.Add(text);
        }
        for (const auto& circle : GetStopPoints(stops_sorted, sp)) {
            result.Add(circle);
        }
        for (const auto& text : GetStopLabels(stops_sorted, sp)) {
            result.Add(text);
        }
        return result;
    }  
       
}



