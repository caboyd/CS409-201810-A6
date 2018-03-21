#pragma once

#include "World.h"
#include "Collision.h"
#include "MathHelper.h"
#include <deque>

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
	float distance_from_start;
	float distance_from_end;
	std::vector<NodeLink> node_links;
	Node(const unsigned node_id, const unsigned disk_id, const Vector3 pos)
		:node_id(node_id), disk_id(disk_id), position(pos)
	{
		node_links = {};
		visited_from_start = false;
		visited_from_end = false;
		path_node_to_start = NO_VERTEX_FOUND;
		path_node_to_end = NO_VERTEX_FOUND;
		distance_from_start = HIGH_VALUE;
		distance_from_end = HIGH_VALUE;
	};
	Node() {};
};

struct NodeLink
{
	unsigned disk_id;
	unsigned node_id;
	float weight;
	NodeLink(unsigned node_id, unsigned disk_id, float weight) :node_id(node_id), disk_id(disk_id), weight(weight) {};
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

	MovementGraph()
	{}

	void destroy()
	{
		node_list.clear();
		disk_node_list.clear();
	}

	~MovementGraph()
	{
		destroy();
	}

	void init(const World& world)
	{

		unsigned size = world.disks.size();
		const std::vector<std::unique_ptr<Disk>>& disks = world.disks;
		node_list.resize(0);
		disk_node_list.resize(world.disks.size());

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
		node_list[node_id_i].node_links.emplace_back(node_id_j, disk_id_j, weight);
		node_list[node_id_j].node_links.emplace_back(node_id_i, disk_id_i, weight);
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
		std::deque<unsigned> path;

		//Current node we are at
		unsigned curr;

		node_list[node_start_id].path_node_to_start = node_start_id;
		node_list[node_start_id].distance_from_start = 0;


		while (true)
		{
			curr = findNextNodeFromStart();

			if (curr == NO_VERTEX_FOUND) break;
			if(curr == node_end_id) break;

			node_list[curr].visited_from_start = true;

			if(node_list[curr].node_links.empty()) continue;

			//Look through all linked nodes and update if path is shorter
			for(auto& link: node_list[curr].node_links)
			{
				if(!node_list[link.node_id].visited_from_start)
				{
					if(node_list[curr].distance_from_start + link.weight < node_list[link.node_id].distance_from_start)
					{
						node_list[link.node_id].distance_from_start = node_list[curr].distance_from_start + link.weight;
						node_list[link.node_id].path_node_to_start = curr;
					}
				}
			
			}
		}

		if(curr == NO_VERTEX_FOUND) std::cout << "Didnt find path\n";

		//Build the path
		while(curr != node_start_id)
		{
			path.push_front(curr);
			curr = node_list[curr].path_node_to_start;
			if(curr == NO_VERTEX_FOUND) std::cout << "Path error at:" << curr << std::endl;
		}

		return path;
	}



	unsigned findNextNodeFromStart()
	{
		unsigned nextVertex = NO_VERTEX_FOUND;
		float shortestDistance = HIGH_VALUE;
		for (unsigned i = 0; i < node_list.size(); i++)
		{
			//Finds vertex with smallest distance that is unknown
			if (node_list[i].distance_from_start < shortestDistance && !node_list[i].visited_from_start)
			{
				nextVertex = i;
				shortestDistance = node_list[i].distance_from_start;
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
			node.distance_from_start = HIGH_VALUE;
			node.distance_from_end = HIGH_VALUE;
		}
	}
};


