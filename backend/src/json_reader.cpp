/*
 * ��� ��������� �������� � ���� � ������������ ������� ������� � ������� JSON
 */

#include "json_reader.h" 

const json::Node& JsonReader::GetBaseRequests() const {
    const auto& root_map = input_.GetRoot().AsDict();
    auto it = root_map.find("base_requests");
    if (it == root_map.end()) { 
        static json::Node empty_node;  
        return empty_node;
    }
    return it->second;
}

const json::Node& JsonReader::GetRoutingSettings() const {
    const auto& root_map = input_.GetRoot().AsDict();
    auto it = root_map.find("routing_settings");
    if (it == root_map.end()) {
        static json::Node empty_node;
        return empty_node;
    }
    return it->second;
}

const json::Node& JsonReader::GetRenderSettings() const {
    const auto& root_map = input_.GetRoot().AsDict();
    auto it = root_map.find("render_settings");
    if (it == root_map.end()) {
        static json::Node empty_node;
        return empty_node;
    }
    return it->second;
}

const json::Node& JsonReader::GetStatRequests() const { 
    const auto& root_map = input_.GetRoot().AsDict();
    auto it = root_map.find("stat_requests");
    if (it == root_map.end()) { 
        static json::Node empty_node; 
        return empty_node;
    }
    return it->second;
} 

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, rh).AsDict());
        }
        else if (type == "Bus") {
            result.push_back(PrintBus(request_map, rh).AsDict());
        }
        else if (type == "Map") {
            result.push_back(PrintMap(request_map, rh).AsDict());
        }
        else if (type == "Route") {
            result.push_back(PrintRoute(request_map, rh).AsDict());
        }
    }

    json::Print(json::Document{ result }, std::cout);
} 

void JsonReader::PullCatalogue(TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    ProcessStopRequests(arr, catalogue);
    PullStopDistances(catalogue);
    ProcessBusRequests(arr, catalogue);
} 

void JsonReader::ProcessStopRequests(const json::Array& arr, TransportCatalogue& catalogue) {
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = PullStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates);
        }
    }
}

void JsonReader::ProcessBusRequests(const json::Array& arr, TransportCatalogue& catalogue) {
    for (const auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops, circular_route, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price] = PullBus(request_bus_map, catalogue);
            catalogue.AddBus(bus_number, stops, circular_route, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price);
        }
    }
}

std::tuple<std::string_view, detail::Coordinates, std::map<std::string_view, int>> JsonReader::PullStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    detail::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsDict();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return { stop_name, coordinates, stop_distances };
}

void JsonReader::PullStopDistances(TransportCatalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = PullStop(request_stops_map);
            for (auto& [to_name, dist] : stop_distances) {
                auto from = catalogue.FindStop(stop_name);
                auto to = catalogue.FindStop(to_name);
                catalogue.SetDistance(from, to, dist);
            }
        }
    }
}

// bus_number, stops, circular_route, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price
std::tuple<std::string_view, std::vector<const Stop*>, bool, size_t, std::array<uint8_t, 3>, BusType, size_t, bool, bool, bool, bool, uint8_t> JsonReader::PullBus(const json::Dict& request_map, TransportCatalogue& catalogue) const {
    std::string_view bus_number = request_map.at("name").AsString();
    std::vector<const Stop*> stops;
    for (const auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(catalogue.FindStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();

    size_t color_index = static_cast<size_t>(request_map.at("color_index").AsInt());
    size_t capacity = static_cast<size_t>(request_map.at("capacity").AsInt());

    std::array<uint8_t, 3> rgb = {0, 0, 0};

    auto StringToBusType = [](std::string_view type_str) {
        static const std::unordered_map<std::string_view, BusType> string_to_bus_type = {
            {"autobus", BusType::Autobus},
            {"electrobus", BusType::Electrobus},
            {"trolleybus", BusType::Trolleybus}
        };

        auto it = string_to_bus_type.find(type_str);
        if (it != string_to_bus_type.end()) {
            return it->second;
        } else {
            throw std::invalid_argument("Unknown bus type: " + std::string(type_str));
        }
    };

    BusType bus_type = StringToBusType(request_map.at("bus_type").AsString());

    bool has_wifi = request_map.at("has_wifi").AsBool();
    bool has_sockets = request_map.at("has_sockets").AsBool();
    bool is_night = request_map.at("is_night").AsBool();

    bool is_available = request_map.at("is_available").AsBool();
    uint8_t price = static_cast<uint8_t>(request_map.at("price").AsInt());

    return { bus_number, stops, circular_route, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price };
}

// std::tuple<std::string_view, std::vector<const Stop*>, bool, size_t> JsonReader::PullBus(const json::Dict& request_map, TransportCatalogue& catalogue) const {
//     std::string_view bus_number = request_map.at("name").AsString();
//     std::vector<const Stop*> stops;
//     for (auto& stop : request_map.at("stops").AsArray()) {
//         stops.push_back(catalogue.FindStop(stop.AsString()));
//     }
//     bool circular_route = request_map.at("is_roundtrip").AsBool();
//     const size_t color_index = static_cast<size_t>(request_map.at("color_index").AsInt());
//     //return { bus_number, stops, circular_route };
//     return { bus_number, stops, circular_route, color_index };
// }

renderer::MapRenderer JsonReader::PullRenderSettings(const json::Dict& request_map) const {
    renderer::RenderSettings render_settings;

    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() }; 
    render_settings.underlayer_color = PullColor(request_map.at("underlayer_color"));
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();
    const json::Array& color_palette = request_map.at("color_palette").AsArray();

    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) {
            render_settings.color_palette.emplace_back(color_element.AsString());
        }
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.emplace_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.emplace_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else {
                throw std::logic_error("wrong color_palette type");
            }
        }
        else {
            throw std::logic_error("wrong color_palette");
        }
    }

    return render_settings;
} 

TransportRouter JsonReader::PullRoutingSettings(const json::Node& settings_map, const TransportCatalogue& catalogue) const {
    return TransportRouter{ settings_map.AsDict().at("bus_wait_time").AsInt(), settings_map.AsDict().at("bus_velocity").AsDouble(), catalogue };
}

const json::Node JsonReader::PrintBus(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!rh.GetCatalogue().FindBus(route_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        const auto& bus = rh.GetBusStat(route_number);
        result["curvature"] = bus->curvature;
        result["route_length"] = bus->len;
        result["stop_count"] = static_cast<int>(bus->count_stops);
        result["unique_stop_count"] = static_cast<int>(bus->unique_count_stops);
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!rh.GetCatalogue().FindStop(stop_name)) {
        result["error_message"] = std::string("not found");
    }
    else {
        json::Array buses;
        std::set<std::string> sorted_buses;
        for (const auto& bus : rh.GetCatalogue().GetBusesForStop(stop_name)) {
            sorted_buses.insert(bus->name);
        }

        json::Array buses_array;
        for (const auto& bus_name : sorted_buses) {
            buses_array.emplace_back(bus_name);
        }
        result["buses"] = buses_array;
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap("");
    map.Render(strm);
    result["map"] = strm.str();

    return json::Node{ result };
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const int id = request_map.at("id"s).AsInt();
    const std::string_view stop_from = request_map.at("from"s).AsString();
    const std::string_view stop_to = request_map.at("to"s).AsString();
    const auto& [optimal_route_opt, graph] = rh.GetOptimalRoute(stop_from, stop_to);

    if (!optimal_route_opt) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id"s).Value(id)
            .Key("error_message"s).Value("not found"s)
            .EndDict()
            .Build();
    }
    else { 
        
        json::Array items;
        double total_time = 0.0;
        items.reserve(optimal_route_opt.value().edges.size());
        for (auto& edge_id : optimal_route_opt.value().edges) {
            const graph::Edge<double> edge = graph.GetEdge(edge_id);  
            if (edge.quality == 0) {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                    .Key("stop_name"s).Value(edge.name)
                    .Key("time"s).Value(edge.weight)
                    .Key("type"s).Value("Wait"s)
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                    .Key("bus"s).Value(edge.name)
                    .Key("span_count"s).Value(static_cast<int>(edge.quality))
                    .Key("time"s).Value(edge.weight)
                    .Key("type"s).Value("Bus"s)
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
        }

        result = json::Builder{}
            .StartDict()
            .Key("request_id"s).Value(id)
            .Key("total_time"s).Value(total_time)
            .Key("items"s).Value(items)
            .EndDict()
            .Build();
    }

    return result;
}

svg::Color JsonReader::PullColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();  
    }
    else if (color_node.IsArray()) {
        const json::Array& color_array = color_node.AsArray();
        if (color_array.size() == 3) {
            return svg::Rgb(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt());
        }
        else if (color_array.size() == 4) {
            return svg::Rgba(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt(), color_array[3].AsDouble());
        }
        else {
            throw std::logic_error("Wrong color format in JSON");
        }
    }
    else {
        throw std::logic_error("Unexpected type for color in JSON");
    }
}
