#pragma once
#include "Collision.h"
#include <deque>
#include <memory>
#include "Disk.h"

static const float HIGH_VALUE = FLT_MAX;
static const unsigned NO_VERTEX_FOUND = -1;

//The acquired search data per node when a search algorith is run
struct SearchData
{
	bool visited{};
	unsigned path_node{};
	float given_cost{};
	float heuristic{};
	float priority{};

	SearchData() { init(); }
	SearchData(const SearchData& n) = default;
	SearchData& operator=(const SearchData& n) = default;
	void init()
	{
		visited = false;
		path_node = NO_VERTEX_FOUND;
		given_cost = HIGH_VALUE;
		heuristic = 0;
		priority = HIGH_VALUE;
	}
};

//The search data for both directions of MM search
struct NodeSearchData
{
	SearchData start{};
	SearchData end{};

	NodeSearchData() { init(); };
	NodeSearchData(const NodeSearchData& n) = default;
	void init() { start.init(); end.init(); }
};

struct NodeLink;

//The data of each node in the movement graph
struct Node
{
	unsigned node_id{};
	unsigned disk_id{};
	Vector3 position;
	std::vector<NodeLink> node_links;

	Node(const unsigned node_id, const unsigned disk_id, const Vector3& pos)
		: node_id(node_id), disk_id(disk_id), position(pos), node_links({})
	{};
	Node() = default;
};

//The data of each edge between nodes in the movement graph
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

//A Graph of nodes between the disk of the world.
//Can perform pathfinding algorithms between nodes and return optimal paths
class MovementGraph
{
private:
	//Offset for collision checking to include disks almost touching
	const float collision_offset = 0.1f;
	const float node_offset = 0.7f;

	//The search data used for pathfinding
	std::vector<NodeSearchData> search_data{};

	//Counters to see how each algorithm performs
	unsigned a_star_visits{};
	unsigned dijkstra_visits{};
	unsigned mm_visits{};

	unsigned node_count{};
	unsigned node_link_count{};

	//The data after a search memorization is called
	std::vector<NodeSearchData> memorized_search_data{};
	unsigned memorized_a_star_visits{};
	unsigned memorized_dijkstra_visits{};
	unsigned memorized_mm_visits{};

	//The node ids organized per disk
	std::vector<std::vector<unsigned>> disk_node_list;

	//The list of nodes of the graph
	std::vector<Node> node_list;

public:

	MovementGraph() = default;

	//Initilize the Node and Node Links of the movement graph using the disks data
	void init(const std::vector<std::unique_ptr<Disk>>& disks);

	//Clean up the vectors of data and clear variables
	~MovementGraph();
	void destroy();


	//Performs dijstra's search algorithm to find optimal path between 2 nodes
	std::deque<unsigned> dijkstraSearch(unsigned node_start_id, unsigned node_end_id);

	//Performs A* search algorithm to find optimal path between 2 nodes
	//Uses the 3D distance between the nodes as the heuristic
	std::deque<unsigned> aStarSearch(unsigned node_start_id, unsigned node_end_id);

	//Performs double ended A* search algorithm to find optimal path between 2 nodes
	//Uses the 3D distance between the nodes as the heuristic
	std::deque<unsigned> mmSearch(unsigned node_start_id, unsigned node_end_id);

	//Returns the total cost given a path that was the result of a search
	float getPathCost(std::deque<unsigned> q) const;

	//Memorizes the search data of the last search that was completed
	void memorizeLastSearch();

	//Returns the search data of the last search that was completed
	const std::vector<NodeSearchData>& getMemorizedSearchData() const;

	//Returns the number of nodes that were visited (put on closed list) for each of the
	//search algorithms after they have been memorized
	unsigned getMemorizedAStarVisits() const;
	unsigned getMemorizedDijsktraVisits() const;
	unsigned getMemorizedmmVisits() const;

	//Returns the node list
	const std::vector<Node>& getNodeList() const;

	//Returns number of nodes (vertices) in the graph
	unsigned getNodeCount() const;

	//Returns number of node links (edges) in the graph 
	unsigned getNodeLinkCount() const;

private:
	//Helpers for Search functions

	//Builds and returns the path after a search has been performed
	std::deque<unsigned> getPath(unsigned node_start_id, unsigned node_end_id);
	std::deque<unsigned> getmmPath(unsigned node_start_id, unsigned node_meeting_id, unsigned node_end_id);

	//Clears the search data
	void resetSearchData();
	//Clears the search data and updates all of the nodes with their heuristic data
	//based on the start and end nodes
	void resetSearchDataWithHeuristics(unsigned node_start_id, unsigned node_end_id);

	//Returns the heurisitic cost which 3D distance between the nodes
	float heuristicCostEstimate(unsigned link_node_id, unsigned node_end_id);

	//Helpers to initialized Movement graph

	//Adds a node at a position for a disk
	unsigned addNode(const Vector3& position, unsigned disk_index);

	//Adds the link between two nodes with a given weight
	//A link is added on both nodes to the other node
	void addLink(unsigned disk_id_i, unsigned node_id_i, unsigned disk_id_j, unsigned node_id_j, float weight);

	//Returns the cost factor based on the disk type
	static float getCostFactor(const Disk& disk);

	//Returns the 3D position a node should have on disk_i if it is adjacent to disk_j
	//Will be at the disk_i radius - 0.7 in the direction of disk_j
	Vector3 calculateNodePosition(const Disk& disk_i, const Disk& disk_j) const;

	//Returns the weight calculated between two nodes on different disks
	float calculateWeightBetweenDisks(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i,
		unsigned node_id_j);

	//Returns the weight calculated between to nodes on different disks
	float calculateWeightSameDisk(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i, unsigned node_id_j);
};
