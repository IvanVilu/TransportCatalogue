#pragma once

#include "input_reader.h"

#include <cassert>

void TestConvertStringToQueryType() {

	assert(ConvertStringToQueryType("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka") == QueryType::AddBus);
	assert(ConvertStringToQueryType("Stop Rasskazovka: 55.632761, 37.333324") == QueryType::AddStop);
}

void TestParseAddStopQuery() {
	const double EPSILON = 1e-6;
	Stop stop = ParseAddStopQuery("Stop Rasskazovka: 55.632761, 37.333324");
	assert(stop.name == "Rasskazovka");
	assert(std::abs(stop.latitude - 55.632761) < EPSILON);
	assert(std::abs(stop.longitude - 37.333324) < EPSILON);
}
/*
void TestBusStops() {
	using namespace std::literals;
	std::string bus = "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka";
	assert(ParseAddBusQuery(bus) == std::vector<std::string_view>({"Tolstopaltsevo"sv, "Marushkino"sv, "Rasskazovka"sv, "Tolstopaltsevo"sv }));

	bus = "Bus 750: Tolstopaltsevo > Marushkino > Rasskazovka";
	assert(ParseBusStops(bus) == std::vector<std::string_view>({ "Tolstopaltsevo"sv, "Marushkino"sv, "Rasskazovka"sv, "Marushkino"sv, "Tolstopaltsevo"sv}));
}
*/
void RunTests() {

}
