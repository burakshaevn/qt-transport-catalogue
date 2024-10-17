#include <iostream>
#include <sstream>
#include "json_reader.h"
#include "request_handler.h" 
 
int main() {
    TransportCatalogue catalogue;
    
    JsonReader json_doc(std::cin);

    json_doc.PullCatalogue(catalogue);
     
    const auto& stat_requests = json_doc.GetStatRequests();
    
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.PullRenderSettings(render_settings);

    const auto& routing_array = json_doc.GetRoutingSettings().AsDict();
    auto routing_settings = json_doc.PullRoutingSettings(routing_array, catalogue);
    const auto& router = TransportRouter{ std::move(routing_settings.GetBusWaitTime()), std::move(routing_settings.GetBusVelocity()), catalogue};

    RequestHandler rh(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, rh);
} 