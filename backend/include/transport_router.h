﻿#pragma once
#include "json.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
#include <utility>
#include <memory>

class TransportRouter {
public:

	using Graph = graph::DirectedWeightedGraph<double>;
	using Router = std::unique_ptr<graph::Router<double>>;

	TransportRouter() = default;

	explicit TransportRouter(const int wait_time, const double velocity, const TransportCatalogue& catalogue)
		: bus_wait_time_(wait_time)
		, bus_velocity_(velocity)
		, catalogue_(catalogue)
	{
		BuildGraph();
	}

	int GetBusWaitTime() const {
		return bus_wait_time_;
	}

	double GetBusVelocity() const {
		return bus_velocity_;
	}

	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

private:
	friend class RequestHandler;

	// ����� �������� �������� �� ���������, � �������
	int bus_wait_time_ = 0;

	// �������� ��������, � ��/�
	double bus_velocity_ = 0.0;

	Graph graph_;
	Router router_;
	TransportCatalogue catalogue_;
	std::unordered_map<std::string, graph::VertexId> stop_name_to_vertex_id_;

	void BuildGraph();
	void AddStopsToGraph();
	void AddBusesToGraph();

	graph::VertexId GetVertexId(const std::string_view stop_name) const {
		auto it = stop_name_to_vertex_id_.find(std::string(stop_name));
		if (it != stop_name_to_vertex_id_.end()) {
			return it->second;
		}
		throw std::invalid_argument("Stop name not found");
	}

};
