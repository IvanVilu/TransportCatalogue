#pragma once

#include <optional>
#include <algorithm>
#include <unordered_map>
#include <set>

#include "svg.h"
#include "geo.h"
#include "domain.h"

namespace renderer {
/*
* Вспомогательный класс для проецирования координат geo::Coordinates в пространство координат svg
*/
	class SphereProjector {
	public:
		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
			double max_height, double padding);

		svg::Point operator()(geo::Coordinates coords) const;

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	};

	struct RenderSettings {
		std::optional<double> width;
		std::optional<double> height;
		std::optional<double> padding;
		std::optional<double> line_width;
		std::optional<double> stop_radius;
		std::optional<int> bus_label_font_size;
		std::optional<svg::Point> bus_label_offset;
		std::optional<int> stop_label_font_size;
		std::optional<svg::Point> stop_label_offset;
		std::optional<svg::Color> underlayer_color;
		std::optional<double> underlayer_width;
		std::optional<std::vector<svg::Color>> color_palette;
	};

	class MapRenderer {
	public:
		MapRenderer(RenderSettings& render_settings);

		template <typename Container>
		void DrawPicture(const Container& container, svg::ObjectContainer& target);

		std::vector<svg::Polyline> GetBusLines(const std::vector<domain::Bus*>& buses, const SphereProjector& sp) const;

		std::vector<svg::Text> GetBusLabels(const std::vector<domain::Bus*>& buses, const SphereProjector& sp) const;

		std::vector<svg::Circle> GetStopPoints(const std::vector<domain::Stop*>& stops, const SphereProjector& sp) const;

		std::vector<svg::Text> GetStopLabels(const std::vector<domain::Stop*>& stops, const SphereProjector& sp) const;
		 
		svg::Document GetSvgDocument(const std::unordered_map<std::string_view, domain::Bus*>& buses,
			const std::unordered_map<domain::Stop*, std::set<std::string_view>>& stops) const;
		

	private:

		RenderSettings render_settings_;
	};

	
	template<typename Container>
	inline void MapRenderer::DrawPicture(const Container& container, svg::ObjectContainer& target) {
		using namespace std;
		DrawPicture(begin(container), end(container), target);
	}
}


