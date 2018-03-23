#pragma once
#include "Collision.h"
#include "MathHelper.h"
#include <deque>
#include <memory>
#include "UpdatablePriorityQueue.h"
#include "Disk.h"

static const float HIGH_VALUE = FLT_MAX;
static const unsigned NO_VERTEX_FOUND = -1;

struct SearchData
{
	bool on_path;
	bool visited;
	unsigned path_node;
	float given_cost;
	float heuristic;
	float priority;

	SearchData() { init(); }
	SearchData(const SearchData& n) = default;

	void init()
	{
		on_path = false;
		visited = false;
		path_node = NO_VERTEX_FOUND;
		given_cost = HIGH_VALUE;
		heuristic = 0;
		priority = HIGH_VALUE;
	}
};

struct NodeSearchData
{
	SearchData start;
	SearchData end;

	NodeSearchData()
	{
		init();
	};

	void init()
	{
		start.init();
		end.init();
	}

	NodeSearchData(const NodeSearchData& n) = default;
};

struct NodeLink;

struct Node
{
	unsigned node_id;
	unsigned disk_id;
	Vector3 position;
	std::vector<NodeLink> node_links;

	Node(const unsigned node_id, const unsigned disk_id, const Vector3 pos)
		: node_id(node_id), disk_id(disk_id), position(pos), node_links({})
	{};

	Node()
	{};
};

struct NodeLink
{
	unsigned source_node_id;
	unsigned dest_node_id;
	unsigned disk_id;
	float weight;

	NodeLink(unsigned source_id, unsigned dest_id, unsigned disk_id, float weight)
		: source_node_id(source_id), dest_node_id(dest_id), disk_id(disk_id), weight(weight)
	{};
	NodeLink() = default;
};

class MovementGraph
{
private:
	//Offset for collision checking to include disks almost touching
	const float collision_offset = 0.1f;
	const float node_offset = 0.7f;
	std::vector<NodeSearchData> memorized_search_data;
	std::vector<NodeSearchData> search_data;
		//Counters to see how each algorithm performs
	unsigned a_star_visits{};
	unsigned dijkstra_visits{};
	unsigned mm_visits{};
	unsigned memorized_a_star_visits{};
	unsigned memorized_dijkstra_visits{};
	unsigned memorized_mm_visits{};
	unsigned node_count{};
	unsigned node_link_count{};

public:
	std::vector<Node> node_list;
	std::vector<std::vector<unsigned>> disk_node_list;




	MovementGraph() = default;
	void init(const std::vector<std::unique_ptr<Disk>>& disks);
	~MovementGraph();
	void destroy();


	std::deque<unsigned> dijkstraSearch(unsigned node_start_id, unsigned node_end_id);
	std::deque<unsigned> aStarSearch(unsigned node_start_id, unsigned node_end_id);
	std::deque<unsigned> mmSearch(unsigned node_start_id, unsigned node_end_id);

	float getPathCost(std::deque<unsigned> q) const;
	void memorizeLastSearch();

	const std::vector<NodeSearchData>& getMemorizedSearchData() const;
	unsigned getMemorizedAStarVisits() const;
	unsigned getMemorizedDijsktraVisits() const;
	unsigned getMemorizedmmVisits() const;
	unsigned getNodeCount() const;
	unsigned getNodeLinkCount() const;

private:
	//Helpers for Search functions
	std::deque<unsigned> getPath(unsigned node_start_id, unsigned node_end_id);
	std::deque<unsigned> getmmPath(unsigned node_start_id, unsigned node_meeting_id, unsigned node_end_id);
	void resetSearchData();
	void resetSearchDataWithHeuristics(unsigned node_start_id, unsigned node_end_id);
	float heuristicCostEstimate(unsigned link_node_id, unsigned node_end_id);

	//Helpers to initialized Movement graph
	unsigned addNode(const Vector3& position, unsigned disk_index);
	void addLink(unsigned disk_id_i, unsigned node_id_i, unsigned disk_id_j, unsigned node_id_j, float weight);
	static float getCostFactor(const Disk& disk);
	Vector3 calculateNodePosition(const Disk& disk_i, const Disk& disk_j) const;

	float calculateWeightBetweenDisks(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i,
		unsigned node_id_j);
	float calculateWeightSameDisk(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i, unsigned node_id_j);
};
