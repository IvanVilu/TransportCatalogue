#include "input_reader.h"

// trim from start (in place)
void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}



// trim from both ends (in place)
void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

Stop ParseAddStopQuery(const std::string_view stop) {

    size_t first_name_pos = stop.find("Stop ") + 5;
    first_name_pos = stop.find_first_not_of(' ', first_name_pos);
    size_t colon_pos = stop.find_first_of(':');

    size_t latitude_first = stop.find_first_not_of(' ', colon_pos + 1);
    size_t latitude_last = stop.find_first_of(',', latitude_first + 1);

    size_t longitude_first = stop.find_first_not_of(' ', latitude_last + 1);
    
    size_t longitude_last = stop.find(',', longitude_first + 1);

    std::string stop_name = std::string(stop.substr(first_name_pos, colon_pos - first_name_pos));
    trim(stop_name);

    return { std::move(stop_name),
        stod(std::string(stop.substr(latitude_first, latitude_last - latitude_first))),
        stod(std::string(stop.substr(longitude_first, longitude_last - longitude_first))) };
}

std::tuple<std::string, std::vector<std::string_view>> ParseAddBusQuery(const std::string_view bus) {
    size_t first_name_pos = bus.find("Bus ") + 4;
    first_name_pos = bus.find_first_not_of(' ', first_name_pos);
    size_t colon_pos = bus.find_first_of(':');

    std::string name = std::string(bus.substr(first_name_pos, colon_pos - first_name_pos));

    const int64_t pos_end = bus.npos;
    std::vector<std::string_view> stops;

    size_t first_stop_pos = bus.find_first_not_of(' ', colon_pos + 1);

    const char divide_sym = bus.find('-') != pos_end ? '-' : '>';

    while (true) {
        int64_t divide_sym_pos = bus.find(divide_sym, first_stop_pos);

        if (divide_sym_pos == pos_end) {
            auto s = bus.substr(first_stop_pos, bus.size() - first_stop_pos);
            while (s[s.size() - 1] == ' ') s.remove_suffix(1);
            stops.push_back(s);
        }
        else {
            auto s = bus.substr(first_stop_pos, divide_sym_pos - 1 - first_stop_pos);
            while (s[s.size() - 1] == ' ') s.remove_suffix(1);
            stops.push_back(s);
        }

        if (divide_sym_pos == pos_end) {
            break;
        }

        first_stop_pos = bus.find_first_not_of(' ', divide_sym_pos + 1);
    }

    if (divide_sym == '-') {
        int stops_size = stops.size();
        for (int i = stops_size - 2; i >= 0; --i) {
            stops.push_back(stops[i]);
        }
    }

    trim(name);

    return { name, stops };
}

std::string_view ParseBusInfoQuery(const std::string_view query) {
    size_t name_first_pos = query.find("Bus ") + 4;
    name_first_pos = query.find_first_not_of(' ', name_first_pos);
    return query.substr(name_first_pos, query.size() - name_first_pos);
}

std::string_view ParseStopInfoQuery(const std::string_view query) {
    size_t name_first_pos = query.find("Stop ") + 5;
    name_first_pos = query.find_first_not_of(' ', name_first_pos);
    return query.substr(name_first_pos, query.size() - name_first_pos);
}

QueryType ConvertStringToQueryType(const std::string_view s) {
    if (s.find("Stop") != s.npos) {
        if(s.find(":") != s.npos) {
            return QueryType::AddStop;
        } else {
            return QueryType::GetStopInfo;
        }
        
    } else if (s.find("Bus") != s.npos) {
        if (s.find(':') != s.npos) {
            return QueryType::AddBus;
        }
        else {
            return QueryType::GetBusInfo;
        }
    }

    return QueryType::AddBus;
}

std::vector<Query> ReadInput(std::istream& input) {

    std::string line;
    std::getline(input, line);
    
    int query_count;
    query_count = std::stoi(line);
    std::vector<Query> res;
    res.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
    
        std::getline(input, line);
        trim(line);
        res.push_back({ ConvertStringToQueryType(line), std::move(line) });
    }
    return res;
}
