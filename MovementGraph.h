#pragma once
#include "Collision.h"
#include "MathHelper.h"
#include <deque>
#include <memory>
#include "UpdatablePriorityQueue.h"
#include "Disk.h"

static const float HIGH_VALUE = FLT_MAX;

static const unsigned NO_VERTEX_FOUND = -1;

struct NodeLink;
struct Node
{
	unsigned node_id;
	unsigned disk_id;
	Vector3 position;


	bool visited_from_start;
	bool visited_from_end;
	unsigned path_node_to_start;
	unsigned path_node_to_end;
	float gcost_from_start;
	float gcost_from_end;
	std::vector<NodeLink> node_links;
	Node(const unsigned node_id, const unsigned disk_id, const Vector3 pos)
		:node_id(node_id), disk_id(disk_id), position(pos)
	{
		node_links = {};
		visited_from_start = false;
		visited_from_end = false;
		path_node_to_start = NO_VERTEX_FOUND;
		path_node_to_end = NO_VERTEX_FOUND;
		gcost_from_start = HIGH_VALUE;
		gcost_from_end = HIGH_VALUE;
	};
	Node() {};
};

struct NodeLink
{
	unsigned source_node_id;
	unsigned dest_node_id;
	unsigned disk_id;
	float weight;

	NodeLink(unsigned source_id, unsigned dest_id, unsigned disk_id, float weight)
		:source_node_id(source_id), dest_node_id(dest_id), disk_id(disk_id), weight(weight)
	{};
	NodeLink() = default;
};

class MovementGraph
{
public:
	//Offset for collision checking to include disks almost touching
	const float collision_offset = 0.1f;
	const float node_offset = 0.7f;
	std::vector<Node> node_list;
	std::vector<std::vector<unsigned>> disk_node_list;
		unsigned a_star_visits;
	unsigned dijkstra_visits;

	MovementGraph() = default;

	void destroy()
	{
		node_list.clear();
		disk_node_list.clear();
	}

	~MovementGraph()
	{
		destroy();
	}

	void init(const std::vector<std::unique_ptr<Disk>>& disks)
	{

		unsigned size = disks.size();
		a_star_visits = 0;
		dijkstra_visits = 0;
		node_list.resize(0);
		disk_node_list.resize(disks.size());

		for (auto& node : disk_node_list)
			node = {};

		for (unsigned i = 0; i < size; i++)
		{
			for (unsigned j = i + 1; j < size; j++)
			{
				const Disk& disk_i = *disks[i];
				const Disk& disk_j = *disks[j];
				//No collision
				if (!Collision::circleIntersection(
					float(disk_i.position.x), float(disk_i.position.z), disk_i.radius + collision_offset,
					float(disk_j.position.x), float(disk_j.position.z), disk_j.radius + collision_offset))
					continue;

				const Vector3 position_i = calculateNodePosition(disk_i, disk_j);
				const Vector3 position_j = calculateNodePosition(disk_j, disk_i);

				unsigned node_id_i = addNode(position_i, i);
				unsigned node_id_j = addNode(position_j, j);

				float weight_ij = calculateWeightBetweenDisks(disks, node_id_i, node_id_j);
				addLink(i, node_id_i, j, node_id_j, weight_ij);

				for (auto& node_id_k : disk_node_list[i])
				{
					float weight_ik = calculateWeightSameDisk(disks, node_id_i, node_id_k);
					addLink(i, node_id_i, node_list[node_id_k].disk_id, node_id_k, weight_ik);
				}
				for (auto& node_id_k : disk_node_list[j])
				{
					float weight_jk = calculateWeightSameDisk(disks, node_id_j, node_id_k);
					addLink(j, node_id_j, node_list[node_id_k].disk_id, node_id_k, weight_jk);
				}

				disk_node_list[i].push_back(node_id_i);
				disk_node_list[j].push_back(node_id_j);
			}
		}

	}
public:

	void addLink(unsigned disk_id_i, unsigned node_id_i, unsigned disk_id_j, unsigned node_id_j, float weight)
	{
		node_list[node_id_i].node_links.emplace_back(node_id_i, node_id_j, disk_id_j, weight);
		node_list[node_id_j].node_links.emplace_back(node_id_j, node_id_i, disk_id_i, weight);
	}

	Vector3 calculateNodePosition(const Disk& disk_i, const Disk& disk_j) const
	{
		//dir
		Vector3 dir = (disk_j.position - disk_i.position).getNormalized();
		//Position of node is edge of disk in the direction
		Vector3 pos = disk_i.position + dir * disk_i.radius;
		//Move nodes 0.5 metres inward from edge of disk
		pos -= dir * node_offset;
		return pos;
	}

	unsigned addNode(const Vector3& position, unsigned disk_index)
	{
		const unsigned id = node_list.size();
		node_list.emplace_back(id, disk_index, position);
		return id;
	}

	float calculateWeightBetweenDisks(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i, unsigned node_id_j)
	{
		const Disk& disk_i = *disks[node_list[node_id_i].disk_id];
		const Disk& disk_j = *disks[node_list[node_id_j].disk_id];
		const Node& node_i = node_list[node_id_i];
		const Node& node_j = node_list[node_id_j];

		float distance = float(node_i.position.getDistance(node_j.position));

		float weight = distance * ((getCostFactor(disk_i) + getCostFactor(disk_j)) / 2.0f);
		return weight;
	}

	float calculateWeightSameDisk(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i, unsigned node_id_j)
	{
		const Disk& disk = *disks[node_list[node_id_i].disk_id];
		const Node& node_i = node_list[node_id_i];
		const Node& node_j = node_list[node_id_j];

		assert(node_j.disk_id == node_i.disk_id);

		const Vector3& center = disk.position;
		const Vector3 di = node_i.position - center;
		const Vector3 dj = node_j.position - center;

		float angle = float(di.getAngle(dj));

		float arc_length = angle * (disk.radius - 0.5f);
		float weight = arc_length * getCostFactor(disk);
		return weight;

	}

	float getCostFactor(const Disk& disk)
	{
		return 1.0f / disk.getSpeedFactor();
	}

	std::deque<unsigned> dijkstraSearch(unsigned node_start_id, unsigned node_end_id)
	{
		resetAll();
	dijkstra_visits = 0;
		std::deque<unsigned> path;

		//Current node we are at
		unsigned curr;

		node_list[node_start_id].path_node_to_start = node_start_id;
		node_list[node_start_id].gcost_from_start = 0;


		while (true)
		{
			curr = findNextNodeFromStart();
			dijkstra_visits++;
			if (curr == NO_VERTEX_FOUND) break;
			if (curr == node_end_id) break;

			node_list[curr].visited_from_start = true;

			if (node_list[curr].node_links.empty()) continue;

			//Look through all linked nodes and update if path is shorter
			for (auto& link : node_list[curr].node_links)
			{
				if (!node_list[link.dest_node_id].visited_from_start)
				{
					if (node_list[curr].gcost_from_start + link.weight < node_list[link.dest_node_id].gcost_from_start)
					{
						node_list[link.dest_node_id].gcost_from_start = node_list[curr].gcost_from_start + link.weight;
						node_list[link.dest_node_id].path_node_to_start = curr;
					}
				}

			}
		}
		return getPath(node_start_id, node_end_id);
	}


	std::deque<unsigned> getPath(unsigned node_start_id, unsigned node_end_id)
	{

		std::deque<unsigned> path;
		while (node_end_id != node_start_id)
		{

			path.push_front(node_end_id);
		//	std::cout << node_end_id << " cost: " << node_list[node_end_id].gcost_from_start << std::endl;
			node_end_id = node_list[node_end_id].path_node_to_start;

			if (node_end_id == NO_VERTEX_FOUND) std::cout << "Path error at:" << node_end_id << std::endl;
		}
		//std::cout << std::endl;

		return path;
	}

	std::deque<unsigned> aStarSearch(unsigned node_start_id, unsigned node_end_id)
	{
		resetAll();
		a_star_visits = 0;
		UpdatablePriorityQueue<float> queue_start;
		queue_start.setCapacityAndMaximumQueueSize(node_list.size(), node_list.size());


		node_list[node_start_id].path_node_to_start = node_start_id;
		node_list[node_start_id].gcost_from_start = float(node_list[node_start_id].position.getDistance(node_list[node_end_id].position));
		queue_start.enqueueOrSetPriority(node_start_id, node_list[node_start_id].gcost_from_start);

		while (!queue_start.isQueueEmpty())
		{
			const unsigned curr = queue_start.peekAndDequeue();
			a_star_visits++;
			//Build the path
			if (curr == node_end_id)
			break;
				

			node_list[curr].visited_from_start = true;

			if (node_list[curr].node_links.empty()) continue;

			//Look through all linked nodes and update if path is shorter
			for (const NodeLink& link : node_list[curr].node_links)
			{
				//If in closed set ignore because already evaluated
				if (node_list[link.dest_node_id].visited_from_start) continue;
				//if(!queue_start.isEnqueued(link.dest_node_id))
				//	queue_start.enqueueOrSetPriority(link.dest_node_id, HIGH_VALUE);

				const float g_score = node_list[curr].gcost_from_start + link.weight;
				const float h_score = heuristicCostEstimate(link.dest_node_id,node_end_id);

				//If better path
				if (g_score < node_list[link.dest_node_id].gcost_from_start)
				{
					//f = g + h
					node_list[link.dest_node_id].gcost_from_start = g_score;

					node_list[link.dest_node_id].path_node_to_start = curr;
					queue_start.enqueueOrSetPriority(link.dest_node_id, node_list[link.dest_node_id].gcost_from_start + h_score);
				}


			}
		}

		return getPath(node_start_id, node_end_id);
	}


	float heuristicCostEstimate(unsigned link_node_id, unsigned node_end_id)
	{

		return float(node_list[link_node_id].position.getDistance(node_list[node_end_id].position));
	}

	float getPathCost(std::deque<unsigned> q)
	{
		if(q.size() == 0) return 0;
		float cost = 0;
		for (unsigned i = 0; i < q.size() - 1; i++)
		{
			//find link
			for (auto j : node_list[q[i]].node_links)
			{
				if (j.dest_node_id == q[i + 1])
					cost += j.weight;
			}
		}
		return cost;
	}

	unsigned findNextNodeFromStart()
	{
		unsigned nextVertex = NO_VERTEX_FOUND;
		float shortestDistance = HIGH_VALUE;
		for (unsigned i = 0; i < node_list.size(); i++)
		{
			//Finds vertex with smallest distance that is unknown
			if (node_list[i].gcost_from_start < shortestDistance && !node_list[i].visited_from_start)
			{
				nextVertex = i;
				shortestDistance = node_list[i].gcost_from_start;
			}
		}
		return nextVertex;
	}


	void resetAll()
	{
		for (auto& node : node_list)
		{
			node.visited_from_start = false;
			node.visited_from_end = false;
			node.path_node_to_start = NO_VERTEX_FOUND;
			node.path_node_to_end = NO_VERTEX_FOUND;
			node.gcost_from_start = HIGH_VALUE;
			node.gcost_from_end = HIGH_VALUE;
		}
	}
};


