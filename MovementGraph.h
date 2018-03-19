#pragma once

#include "World.h"
#include "Collision.h"
#include "MathHelper.h"


struct NodeLink;
struct Node
{
	Vector3 position;
	unsigned disk_id;
	//int distance;
	std::vector<NodeLink> node_links;
	Node(Vector3 pos, unsigned id) :position(pos), disk_id(id), node_links({}) {};
	Node():node_links({}){};
};

struct NodeLink
{
	unsigned disk_id;
	float weight;
	NodeLink(unsigned id, float weight):disk_id(id),weight(weight){};
};

class MovementGraph
{
public:

	std::vector<Node> node_list;
	std::vector<std::vector<unsigned>> disk_node_list;

	MovementGraph()
	{
	}

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
		//Offset for collision checking to include disks almost touching
		float offset = 0.1f;

		unsigned size = world.disks.size();
		const std::vector<std::unique_ptr<Disk>>& disks = world.disks;
		node_list.resize(world.disks.size());
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
					float(disk_i.position.x), float(disk_i.position.z), disk_i.radius + offset,
					float(disk_j.position.x), float(disk_j.position.z), disk_j.radius + offset))
					continue;

				const Vector3 position_i = calculateNodePosition(disk_i, disk_j);
				const Vector3 position_j = calculateNodePosition(disk_j, disk_i);

				unsigned node_id_i = addNode(position_i, i);
				unsigned node_id_j = addNode(position_j, j);

				float weight_ij = calculateWeightBetweenDisks(disks, node_id_i, node_id_j);
				addLink(node_id_i, node_id_j, weight_ij);

				for (auto& node_id_k : disk_node_list[i])
				{
					float weight_ik = calculateWeightSameDisk(disks, node_id_i, node_id_k);
					addLink(node_id_i, node_id_k, weight_ik);
				}
				for (auto& node_id_k : disk_node_list[j])
				{
					float weight_jk = calculateWeightSameDisk(disks, node_id_j, node_id_k);
					addLink(node_id_j, node_id_k, weight_jk);
				}

				disk_node_list[i].push_back(node_id_i);
				disk_node_list[j].push_back(node_id_j);
			}
		}

	}
private:

	void addLink(unsigned node_id_i, unsigned node_id_j, float weight)
	{
		node_list[node_id_i].node_links.emplace_back(node_id_j,weight);
		node_list[node_id_j].node_links.emplace_back(node_id_i,weight);
	}

	Vector3 calculateNodePosition(const Disk& disk_i, const Disk& disk_j) const
	{
		//dir
		Vector3 dir = (disk_j.position - disk_i.position).getNormalized();
		//Position of node is edge of disk in the direction
		Vector3 pos = disk_i.position +  dir * disk_i.radius;
		//Move nodes 0.5 metres inward from edge of disk
		pos -= dir * 0.5;
		return pos;
	}

	unsigned addNode(const Vector3& position, unsigned disk_index)
	{
		node_list.emplace_back(position, disk_index);
		const unsigned pos = node_list.size() - 1;
		return pos;
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

		float angle =  float(di.getAngle(dj));

		float arc_length = angle  * (disk.radius - 0.5f);
		float weight = arc_length *getCostFactor(disk);
		return weight;

	}

	float getCostFactor(const Disk& disk)
	{
		return 1.0f / disk.getSpeedFactor();
	}
};


