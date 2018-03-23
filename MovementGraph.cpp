#include "MovementGraph.h"

void MovementGraph::destroy()
{
	node_link_count = 0;
	node_count = 0;
	node_list.clear();
	disk_node_list.clear();
	search_data.clear();
	memorized_search_data.clear();
}

void MovementGraph::init(const std::vector<std::unique_ptr<Disk>>& disks)
{
	unsigned size = disks.size();
	assert(size > 0);
	a_star_visits = 0;
	dijkstra_visits = 0;
	node_list.resize(0);
	disk_node_list.resize(size);


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
	search_data = std::vector<NodeSearchData>(node_list.size(), NodeSearchData());
}

MovementGraph::~MovementGraph()
{
	destroy();
}

std::deque<unsigned> MovementGraph::dijkstraSearch(unsigned node_start_id, unsigned node_end_id)
{
	resetSearchData();

	dijkstra_visits = 0;
	UpdatablePriorityQueue<float> queue_start;
	queue_start.setCapacityAndMaximumQueueSize(node_list.size(), node_list.size());


	search_data[node_start_id].start.path_node = node_start_id;
	search_data[node_start_id].start.given_cost = 0;
	search_data[node_start_id].start.priority = search_data[node_start_id].start.given_cost + search_data[node_start_id]
		.start.heuristic;
	queue_start.enqueueOrSetPriority(node_start_id, search_data[node_start_id].start.priority);


	while (true)
	{
		//Current node we are at
		const unsigned curr = queue_start.peekAndDequeue();

		dijkstra_visits++;
		//Goal Found, return path
		if (curr == node_end_id) break;

		search_data[curr].start.visited = true;

		//Look through all linked nodes and update if path is shorter
		for (auto& link : node_list[curr].node_links)
		{
			//If in closed set ignore because already evaluated
			if (search_data[link.dest_node_id].start.visited) continue;

			const float g_score = search_data[curr].start.given_cost + link.weight;

			if (g_score < search_data[link.dest_node_id].start.given_cost)
			{
				//f = g + h
				//update g
				search_data[link.dest_node_id].start.given_cost = g_score;
				//update priority f = g + h
				search_data[link.dest_node_id].start.priority = g_score + search_data[link.dest_node_id].start.heuristic;
				//Mark the node we came from
				search_data[link.dest_node_id].start.path_node = curr;
				//Update open list with priority (f = g + h)
				queue_start.enqueueOrSetPriority(link.dest_node_id, search_data[link.dest_node_id].start.priority);
			}
		}
	}
	return getPath(node_start_id, node_end_id);
}

std::deque<unsigned> MovementGraph::aStarSearch(unsigned node_start_id, unsigned node_end_id)
{
	resetSearchDataWithHeuristics(node_start_id, node_end_id);
	a_star_visits = 0;
	UpdatablePriorityQueue<float> queue_start;
	queue_start.setCapacityAndMaximumQueueSize(node_list.size(), node_list.size());


	search_data[node_start_id].start.path_node = node_start_id;
	search_data[node_start_id].start.given_cost = 0;
	search_data[node_start_id].start.priority = search_data[node_start_id].start.given_cost + search_data[node_start_id]
		.start.heuristic;
	queue_start.enqueueOrSetPriority(node_start_id, search_data[node_start_id].start.priority);

	while (!queue_start.isQueueEmpty())
	{
		const unsigned curr = queue_start.peekAndDequeue();
		a_star_visits++;
		//Goal Found, return path
		if (curr == node_end_id) break;

		//This node has been visited
		search_data[curr].start.visited = true;

		//Look through all linked nodes and update if path is shorter
		for (const NodeLink& link : node_list[curr].node_links)
		{
			//If in closed set ignore because already evaluated
			if (search_data[link.dest_node_id].start.visited) continue;
			//if(!queue_start.isEnqueued(link.dest_node_id))
			//	queue_start.enqueueOrSetPriority(link.dest_node_id, HIGH_VALUE);

			const float g_score = search_data[curr].start.given_cost + link.weight;

			//If better path
			if (g_score < search_data[link.dest_node_id].start.given_cost)
			{
				//f = g + h
				//update g
				search_data[link.dest_node_id].start.given_cost = g_score;
				//update priority f = g + h
				search_data[link.dest_node_id].start.priority = g_score + search_data[link.dest_node_id].start.heuristic;
				//Mark the node we came from
				search_data[link.dest_node_id].start.path_node = curr;
				//Update open list with priority (f = g + h)
				queue_start.enqueueOrSetPriority(link.dest_node_id, search_data[link.dest_node_id].start.priority);
			}
		}
	}

	return getPath(node_start_id, node_end_id);
}

std::deque<unsigned> MovementGraph::mmSearch(unsigned node_start_id, unsigned node_end_id)
{
	resetSearchDataWithHeuristics(node_start_id, node_end_id);
	mm_visits = 0;
	UpdatablePriorityQueue<float> queue_start;
	UpdatablePriorityQueue<float> queue_end;
	unsigned curr = NO_VERTEX_FOUND;
	queue_start.setCapacityAndMaximumQueueSize(node_list.size(), node_list.size());
	queue_end.setCapacityAndMaximumQueueSize(node_list.size(), node_list.size());

	//Add start node to Open List 1
	search_data[node_start_id].start.path_node = node_start_id;
	search_data[node_start_id].start.given_cost = 0;
	search_data[node_start_id].start.priority = search_data[node_start_id].start.given_cost + search_data[node_start_id]
		.start.heuristic;
	queue_start.enqueueOrSetPriority(node_start_id, search_data[node_start_id].start.priority);

	//Add end node to Open List 2
	search_data[node_end_id].end.path_node = node_end_id;
	search_data[node_end_id].end.given_cost = 0;
	search_data[node_end_id].end.priority = search_data[node_end_id].end.given_cost + search_data[node_end_id]
		.end.heuristic;
	queue_end.enqueueOrSetPriority(node_end_id, search_data[node_end_id].end.priority);

	//If both are empty no path found
	while (!queue_start.isQueueEmpty() && !queue_end.isQueueEmpty())
	{
		float p1 = HIGH_VALUE;
		float p2 = HIGH_VALUE;
		if (!queue_start.isQueueEmpty())
			p1 = queue_start.peekPriority();
		if (!queue_end.isQueueEmpty())
			p2 = queue_end.peekPriority();

		bool popped_from_start_queue = false;
		SearchData* curr_node_search_data;

		//Pop the node with lowest priority
		if (p1 <= p2)
		{
			curr = queue_start.peekAndDequeue();
			//Goal Found if node is on opposite closed list
			if (search_data[curr].end.visited) break;

			//Get the seach data from correct direction
			curr_node_search_data = &(search_data[curr].start);
			popped_from_start_queue = true;
		} else
		{
			curr = queue_end.peekAndDequeue();
			//Goal Found if node is on opposite closed list
			if (search_data[curr].start.visited) break;

			//Get the seach data from correct direction
			curr_node_search_data = &(search_data[curr].end);
			popped_from_start_queue = false;
		}

		mm_visits++;


		//This node has been visited
		curr_node_search_data->visited = true;

		//Look through all linked nodes and update if path is shorter
		for (const NodeLink& link : node_list[curr].node_links)
		{
			//Get the correct search data based on whether we are searching from the start or the end currently
			SearchData* linked_node_search_data = popped_from_start_queue
				? &search_data[link.dest_node_id].start
				: &search_data[link.dest_node_id].end;

			//If in closed set ignore because already evaluated
			if (linked_node_search_data->visited) continue;
			//if(!queue_start.isEnqueued(link.dest_node_id))
			//	queue_start.enqueueOrSetPriority(link.dest_node_id, HIGH_VALUE);

			const float g_score = curr_node_search_data->given_cost + link.weight;

			//If better path
			if (g_score < linked_node_search_data->given_cost)
			{
				//f = g + h
				//update g
				linked_node_search_data->given_cost = g_score;
				//update priority f = g + h
				linked_node_search_data->priority = g_score + linked_node_search_data->heuristic;
				//Mark the node we came from
				linked_node_search_data->path_node = curr;

				//Update open list with priority (f = g + h)
				//Update prority on correct queue
				if (popped_from_start_queue)
					queue_start.enqueueOrSetPriority(link.dest_node_id, linked_node_search_data->priority);
				else
					queue_end.enqueueOrSetPriority(link.dest_node_id, linked_node_search_data->priority);
			}
		}
	}
	//Meet in the middle is visited from both direction
	search_data[curr].end.visited = true;
	search_data[curr].start.visited = true;
	return getmmPath(node_start_id, curr, node_end_id);
}

float MovementGraph::getPathCost(std::deque<unsigned> q) const
{
	if (q.empty()) return 0;
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

void MovementGraph::memorizeLastSearch()
{
	memorized_a_star_visits = a_star_visits;
	memorized_dijkstra_visits = dijkstra_visits;
	memorized_mm_visits = mm_visits;
	memorized_search_data = search_data;
}

const std::vector<NodeSearchData>& MovementGraph::getMemorizedSearchData() const
{
	return memorized_search_data;
}

unsigned MovementGraph::getMemorizedAStarVisits() const
{
	return memorized_a_star_visits;
}

unsigned MovementGraph::getMemorizedDijsktraVisits() const
{
	return memorized_dijkstra_visits;
}

unsigned MovementGraph::getMemorizedmmVisits() const
{
	return memorized_mm_visits;
}

unsigned MovementGraph::getNodeCount() const
{
	return node_count;
}

unsigned MovementGraph::getNodeLinkCount() const
{
	return node_link_count;
}

std::deque<unsigned> MovementGraph::getPath(unsigned node_start_id, unsigned node_end_id)
{
	std::deque<unsigned> path;
	while (node_end_id != node_start_id)
	{
		path.push_front(node_end_id);
		//	std::cout << node_end_id << " cost: " << node_list[node_end_id].gcost_from_start << std::endl;
		node_end_id = search_data[node_end_id].start.path_node;
		if (node_end_id == NO_VERTEX_FOUND) std::cout << "Path error at:" << node_end_id << std::endl;
	}
	//std::cout << std::endl;

	return path;
}

std::deque<unsigned> MovementGraph::getmmPath(unsigned node_start_id, unsigned node_meeting_id,
	unsigned node_end_id)
{
	std::deque<unsigned> path;

	//Start at middle and go to start for path
	unsigned curr_node = node_meeting_id;
	
	//Note: We won't push the start node because it not part of the path
	while (curr_node != node_start_id)
	{
		path.push_front(curr_node);
		curr_node = search_data[curr_node].start.path_node;
	}

	//Start and middle and go to end for path
	curr_node = node_meeting_id;

	//Skip the meeting node here so we dont push it on the queue twice
	if (curr_node != node_end_id)
		curr_node = search_data[curr_node].end.path_node;

	while (curr_node != node_end_id)
	{
		path.push_back(curr_node);
		curr_node = search_data[curr_node].end.path_node;
	}

	//Push the end node
	path.push_back(node_end_id);

	return path;
}

void MovementGraph::resetSearchData()
{
	for (auto& node : search_data)
	{
		node.init();
	}
}

void MovementGraph::resetSearchDataWithHeuristics(unsigned node_start_id, unsigned node_end_id)
{
	for (unsigned i = 0; i < search_data.size(); i++)
	{
		search_data[i].start.init();
		search_data[i].end.init();

		search_data[i].start.priority = search_data[i].start.heuristic = heuristicCostEstimate(i, node_end_id);
		search_data[i].end.priority = search_data[i].end.heuristic = heuristicCostEstimate(i, node_start_id);
	}
}

float MovementGraph::heuristicCostEstimate(unsigned link_node_id, unsigned node_end_id)
{
	return float(node_list[link_node_id].position.getDistance(node_list[node_end_id].position));
}

unsigned MovementGraph::addNode(const Vector3& position, unsigned disk_index)
{
	const unsigned id = node_list.size();
	node_list.emplace_back(id, disk_index, position);
	node_count++;
	return id;
}

void MovementGraph::addLink(unsigned disk_id_i, unsigned node_id_i, unsigned disk_id_j, unsigned node_id_j,
	float weight)
{
	node_list[node_id_i].node_links.emplace_back(node_id_i, node_id_j, disk_id_j, weight);
	node_list[node_id_j].node_links.emplace_back(node_id_j, node_id_i, disk_id_i, weight);
	node_link_count++;
}

Vector3 MovementGraph::calculateNodePosition(const Disk& disk_i, const Disk& disk_j) const
{
	//dir
	Vector3 dir = (disk_j.position - disk_i.position).getNormalized();
	//Position of node is edge of disk in the direction
	Vector3 pos = disk_i.position + dir * disk_i.radius;
	//Move nodes 0.5 metres inward from edge of disk
	pos -= dir * node_offset;
	return pos;
}

float MovementGraph::getCostFactor(const Disk& disk)
{
	return 1.0f / disk.getSpeedFactor();
}

float MovementGraph::calculateWeightBetweenDisks(const std::vector<std::unique_ptr<Disk>>& disks,
	unsigned node_id_i, unsigned node_id_j)
{
	const Disk& disk_i = *disks[node_list[node_id_i].disk_id];
	const Disk& disk_j = *disks[node_list[node_id_j].disk_id];
	const Node& node_i = node_list[node_id_i];
	const Node& node_j = node_list[node_id_j];

	float distance = float(node_i.position.getDistance(node_j.position));

	float weight = distance * ((getCostFactor(disk_i) + getCostFactor(disk_j)) / 2.0f);
	return weight;
}

float MovementGraph::calculateWeightSameDisk(const std::vector<std::unique_ptr<Disk>>& disks, unsigned node_id_i,
	unsigned node_id_j)
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