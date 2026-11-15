#pragma once
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include <omp.h>

#include "OctTree.h"
#include "ParticleSystem.h"
#include "SFML functions.h"


//#include "Node.h"
OctTree::OctTree(float x1, float y1, float z1, float x2, float y2, float z2) {
	//nodeList.push_back(new Node(this, 0, 0, x1, y1, z1, x2, y2, z2));
	for (int i = 0; i < poolSize; i++) {
		nodeList.push_back(Node(this, i));
	}
	nodeList[0].setData(0, x1, y1, z1, x2, y2, z2); // set root info
}
void  OctTree::GravParms(float t, double g, float e) {
	theta = t;
	G = g;
	expo = e + 1;
}
void OctTree::Draw(Engine* engine) {
	for (int i = 0; i < firstBlankNodeIndex; i++) {
		if (nodeList[i].leaf)
			nodeList[i].Draw(engine);
	}
}
void OctTree::Build() {
	nodeList[0].pStartIndex = 0;
	nodeList[0].pEndIndex = pointList.size(); //initialise so calcCener of mass works right

	nodeList[0].CalcCenterOfMass();
	Split(0); // split all nodes starting at root recusivly
}
void OctTree::Split(int index) {
	Node* node = &nodeList[index];
	if (node->pStartIndex == node->pEndIndex)
		return;

	if (node->depth > 10) { std::cout << "depth 11 reached" << std::endl; return; }
	if (firstBlankNodeIndex >= poolSize) { std::cout << "Node Pool out of mem" << std::endl; return; }

	node->leaf = false;
	//*
	Vector3 midpoint = node->centerOfMass;

	for (int i = 0; i < 8; i++)
		node->childrenIndex[i] = firstBlankNodeIndex + i;

	nodeList[firstBlankNodeIndex + 0].setData(node->depth + 1, node->x1, node->y1, node->z1, midpoint.x, midpoint.y, midpoint.z);
	nodeList[firstBlankNodeIndex + 1].setData(node->depth + 1, midpoint.x, node->y1, node->z1, node->x2, midpoint.y, midpoint.z);

	nodeList[firstBlankNodeIndex + 2].setData(node->depth + 1, node->x1, midpoint.y, node->z1, midpoint.x, node->y2, midpoint.z);
	nodeList[firstBlankNodeIndex + 3].setData(node->depth + 1, midpoint.x, midpoint.y, node->z1, node->x2, node->y2, midpoint.z);


	nodeList[firstBlankNodeIndex + 4].setData(node->depth + 1, node->x1, node->y1, midpoint.z, midpoint.x, midpoint.y, node->z2);
	nodeList[firstBlankNodeIndex + 5].setData(node->depth + 1, midpoint.x, node->y1, midpoint.z, node->x2, midpoint.y, node->z2);

	nodeList[firstBlankNodeIndex + 6].setData(node->depth + 1, node->x1, midpoint.y, midpoint.z, midpoint.x, node->y2, node->z2);
	nodeList[firstBlankNodeIndex + 7].setData(node->depth + 1, midpoint.x, midpoint.y, midpoint.z, node->x2, node->y2, node->z2);
	firstBlankNodeIndex += 8;

	node->SortPoints();
}
void OctTree::Grav() {
	operations = 0;
	//std::vector<int> nodeIndexesLower = std::vector<int>();
	//nodeIndexesLower.push_back(0);
	nodeList[0].Grav({0});

	nodeList[0].PropagateForce();
	for (int i = 0; i < firstBlankNodeIndex; i++) {
		if (nodeList[i].leaf) {
			nodeList[i].ApplyForce();
		}
	}
}
/*
void OctTree::PointGrav(Point* p, Node* node) {
	//std::cout << node->depth << " : " << node->pStartIndex << "-" << node->pEndIndex << " : " << (node->pEndIndex == node->pStartIndex) << " | ";
	if (node->pEndIndex == node->pStartIndex || node->mass == 0)
		return;
	operations++;

	const float dx = node->centerOfMassX - p->x;
	const float dy = node->centerOfMassY - p->y;
	const float dz = node->centerOfMassZ - p->z;

	const float dist = std::hypot(dx, dy, dz);
	
	if (dist / node->meanAbsoluteDeviation > theta) { // rework this please!!!

		const float force = G * node->mass * p->mass / powf(dist + 0.001, expo);
		p->fx += dx * force;
		p->fy += dy * force;
		p->fz += dz * force;

		//node->forceX -= dx * force;
		//node->forceY -= dy * force;
		//node->forceZ -= dz * force;
	}
	else if (node->leaf) {
		float sumX = 0;
		float sumY = 0;
		float sumZ = 0;
		for (int i = node->pStartIndex; i < node->pEndIndex; i++) {
			operations++;

			Point* p2 = &pointList[i];
			if (p == p2 || p->index == p2->index)
				continue;

			const float dx = p2->x - p->x;
			const float dy = p2->y - p->y;
			const float dz = p2->z - p->z;

			const float dist = std::hypot(dx, dy, dz);
			const float force = G * node->mass * p->mass / powf(dist + 0.001, expo);

			sumX += dx * force;
			sumY += dy * force;
			sumZ += dz * force;

			//p2->fx -= dx * force;
			//p2->fy -= dy * force;
			//p2->fz -= dz * force;

		}
		p->fx += sumX;
		p->fy += sumY;
		p->fz += sumZ;
	}
	else {
		for (int i = 0; i < std::size(node->childrenIndex); i++)
			PointGrav(p, &nodeList[node->childrenIndex[i]]);
	}
}*/
void OctTree::Print() {
	std::cout << "NodePoolSize: " << poolSize << " ActiveNodes: " << firstBlankNodeIndex << std::endl;
	for (int i = 0; i < firstBlankNodeIndex; i++) {
		std::cout << "Index: " << i;
		std::cout << " Leaf: " << nodeList[i].leaf;
		std::cout << " Points: " << nodeList[i].pEndIndex - nodeList[i].pStartIndex;
		std::cout << " PointsIndexs: " << nodeList[i].pStartIndex << "-" << nodeList[i].pEndIndex;
		//std::cout << " X1: " << nodeList[i].x1;
		//std::cout << " Y1: " << nodeList[i].y1;
		//std::cout << " Z1: " << nodeList[i].z1;
		//std::cout << " X2: " << nodeList[i].x2;
		//std::cout << " Y2: " << nodeList[i].y2;
		//std::cout << " Z2: " << nodeList[i].z2;
		std::cout << std::endl;
	}
}
OctTree::Point::Point(Vector3 ipos, float imass, int iindex) {
	pos = ipos;

	mass = imass;

	index = iindex;
}
OctTree::Node::Node(OctTree* itree, int imyIndex) {
	tree = itree;
	myIndex = imyIndex;
}
void OctTree::Node::setData(int idepth, float ix1, float iy1, float iz1, float ix2, float iy2, float iz2) {
	depth = idepth;
	x1 = ix1;
	y1 = iy1;
	z1 = iz1;
	x2 = ix2;
	y2 = iy2;
	z2 = iz2;
}
void OctTree::Node::Draw(Engine* engine) const {
	engine->drawCube(x1, y1, z1, x2, y2, z2);
}
void OctTree::AddPoint(Point p) {
	pointList.push_back(p);
	nodeList[0].pEndIndex++;
}
int OctTree::Node::FindNodeOfPoint(Point* p) {

	if (leaf) { return -1; }

	if (p->pos.x == tree->nodeList[childrenIndex[0]].x2 &&
		p->pos.y == tree->nodeList[childrenIndex[0]].y2 &&
		p->pos.y == tree->nodeList[childrenIndex[0]].z2) {
		std::cout << "Point at center adding to node 1" << std::endl;
		return 0;
	}
	else if (p->pos.x < tree->nodeList[childrenIndex[0]].x2) {
		if (p->pos.y < tree->nodeList[childrenIndex[0]].y2) {
			//top left
			if (p->pos.z < tree->nodeList[childrenIndex[0]].z2) {
				//front top left
				return 0;
			}
			else
			{
				//back top left
				return 4;
			}
		}
		else
		{
			//bot left
			if (p->pos.z < tree->nodeList[childrenIndex[0]].z2) {
				//front bot left
				return 2;
			}
			else
			{
				//back bot left
				return 6;
			}
		}
	}
	else
	{
		if (p->pos.y < tree->nodeList[childrenIndex[0]].y2) {
			//top right
			if (p->pos.z < tree->nodeList[childrenIndex[0]].z2) {
				//front top right
				return 1;
			}
			else
			{
				//back top right
				return 5;
			}
		}
		else
		{
			//bot right
			if (p->pos.z < tree->nodeList[childrenIndex[0]].z2) {
				//front bot right
				return 3;
			}
			else
			{
				//back bot right
				return 7;
			}
		}
	}
}
void OctTree::Node::SortPoints() {
	if (leaf)
		return;
	std::vector<int> workingLists[8];
	std::vector<Point> copyList;
	for (int i = pStartIndex; i < pEndIndex; i++) {
		workingLists[FindNodeOfPoint(&tree->pointList[i])].push_back(i);
	}
	for (int i = 0; i < 8; i++) {
		for (int i2 = 0; i2 < workingLists[i].size(); i2++) {
			copyList.push_back(tree->pointList[workingLists[i][i2]]);
		}
	}
	std::copy(copyList.begin(), copyList.end(), tree->pointList.begin() + pStartIndex);

	int s = pStartIndex;
	for (int i = 0; i < 8; i++) {
		tree->nodeList[childrenIndex[i]].pStartIndex = s;
		s += workingLists[i].size();
		tree->nodeList[childrenIndex[i]].pEndIndex = s;
		//std::cout << "sorting ... list size:" << workingLists[i].size() << std::endl;
	}
	for (int i = 0; i < 8; i++) {
		tree->nodeList[childrenIndex[i]].CalcCenterOfMass();
		if (tree->nodeList[childrenIndex[i]].pEndIndex - tree->nodeList[childrenIndex[i]].pStartIndex > tree->maxPointsPerNode) {
			tree->Split(childrenIndex[i]);
		}
	}
}
void OctTree::Node::CalcCenterOfMass() {
	if (pStartIndex == pEndIndex)
		return;
	mass = 0;
	centerOfMass = Vector3();
	for (int i = pStartIndex; i < pEndIndex; i++) {
		Point* p = &tree->pointList[i];
		mass += p->mass;
		centerOfMass += p->pos * p->mass;
	}
	centerOfMass /= mass;
	meanAbsoluteDeviation = 0;
	for (int i = pStartIndex; i < pEndIndex; i++) {
		Point* p = &tree->pointList[i];
		float d = (centerOfMass - p->pos).getLength();
		meanAbsoluteDeviation += d * p->mass;
	}
	//if (depth == 0)
		//std::cout << mass << std::endl;
	meanAbsoluteDeviation /= mass;
}
bool OctTree::Node::Contains(Point* p) {
	if (p->pos.x <= x2 && p->pos.x > x1 &&
		p->pos.y <= y2 && p->pos.y > y1 &&
		p->pos.z <= z2 && p->pos.z > z1)
		return true;
	else
		return false;
}
bool OctTree::Node::ErrorFunction(Node* node, float dist) {
	return (dist > (meanAbsoluteDeviation * log(pEndIndex - pStartIndex) + node->meanAbsoluteDeviation * log(node->pEndIndex - node->pStartIndex)) * tree->theta);
}
void OctTree::Node::Grav(std::vector<int> nodeIndexes) {
	std::vector<int> nodeIndexesLower = std::vector<int>();

	for (int i = 0; i < nodeIndexes.size(); i++) {
		int ind = nodeIndexes[i];
		Node* node = &tree->nodeList[ind];
		//std::cout << myIndex << " " << nodeIndexes[i] << std::endl;
		Vector3 dif = node->centerOfMass - centerOfMass;

		float dist = dif.getLength();
		if ((ErrorFunction(node, dist) || leaf) && !(node == this)) {
			NodeToNodeGrav(node);
		}
		else if (!node->leaf) {
			for (int i2 = 0; i2 < 8; i2++) {
				if (tree->nodeList[node->childrenIndex[i2]].leaf && leaf) {
					NodeToNodeGravAllPoints(node);
				}
				else {
					nodeIndexesLower.push_back(node->childrenIndex[i2]); // add unprossed node's children
				}
			}
		}
		else {
			if (!leaf) {
				nodeIndexesLower.push_back(ind); // if
			}
			else {
				NodeToNodeGravAllPoints(node); // if both are leaf do node to node grav all particles
			}
		}
	}
	if (!leaf) {
		for (int i = 0; i < 8; i++) {
			tree->nodeList[childrenIndex[i]].Grav(nodeIndexesLower);
		}
	}
	else {
		SelfGrav();
		//*
		for (int i = 0; i < nodeIndexesLower.size(); i++) {
			Node* node = &tree->nodeList[i];
			NodeToNodeGravSubDiv(node);
		}
		//*/
	}
}
void OctTree::Node::SelfGrav() {
	//std::cout << "depth " << depth << " node index " << myIndex << " indexes " << pStartIndex << " " << pEndIndex << std::endl;
	for (int i = pStartIndex; i < pEndIndex; i++) {
		Point* p1 = &tree->pointList[i];
		for (int i2 = i + 1; i2 < pEndIndex; i2++) {
			tree->operations++;
			//std::cout << i << " " << i2 << std::endl;
			
			Point* p2 = &tree->pointList[i2];

			Vector3 dif = p2->pos - p1->pos;

			const float dist = dif.getLength();

			const float force = 2 * p1->mass * p2->mass * tree->G / powf(dist + 0.001, tree->expo);

			p1->force += dif * force;

			p2->force -= dif * force;
		}
	}
}
void OctTree::Node::NodeToNodeGrav(Node* node) { // recusive function
	Vector3 dif = node->centerOfMass - centerOfMass;

	float dist = dif.getLength();
	float f = mass * node->mass * tree->G / powf(dist, tree->expo);

	force += dif * f;

	node->force -= dif * f;
}
void OctTree::Node::NodeToNodeGravSubDiv(Node* node) { // recusive function
	if ((node == this)) // handeled in self grav
		return;
	Vector3 dif = node->centerOfMass - centerOfMass;

	const float dist = dif.getLength();

	if (ErrorFunction(node, dist)) {
		float f = mass * node->mass * tree->G / powf(dist, tree->expo); // prevent same name

		force += dif * f;

		node->force -= dif * f;
	}
	else if (node->leaf) {
		NodeToNodeGravAllPoints(node);
	}
	else {
		for (int i = 0; i < 8; i++) {
			NodeToNodeGravSubDiv(&tree->nodeList[node->childrenIndex[i]]);
		}
	}
}
void OctTree::Node::NodeToNodeGravAllPoints(Node* node) { // recusive function
	for (int i = pStartIndex; i < pEndIndex; i++) {
		Point* p1 = &tree->pointList[i];
		for (int i2 = node->pStartIndex; i2 < node->pEndIndex; i2++) {
			Point* p2 = &tree->pointList[i2];

			Vector3 dif = p2->pos - p1->pos;

			const float dist = dif.getLength();

			const float force = p1->mass * p2->mass * tree->G / powf(dist + 0.001, tree->expo);

			p1->force += dif * force;

			p2->force -= dif * force;
		}
	}
}
void OctTree::Node::PropagateForce() {
	if (leaf)
		return;
	for (int i = 0; i < 8; i++) {
		Node* node = &tree->nodeList[childrenIndex[i]];
		node->force += force * node->mass / mass;
	}
	for (int i = 0; i < 8; i++) {
		tree->nodeList[childrenIndex[i]].PropagateForce();
	}
}
void OctTree::Node::ApplyForce() {
	for (int i = pStartIndex; i < pEndIndex; i++) {
		Point* p = &tree->pointList[i];
		p->force += force / mass * p->mass;
	}
}
/*
void OctTree::Node::Debug(Engine* engine, float offsetX, float offsetY, float offsetZ, float a) {
	for (int i = 0; i < tree->nodeList.size(); i++) {
		Node* node = &tree->nodeList[i];
		if (node == this) { Draw(engine); continue; }
		float dist = sqrt(powf(centerOfMassX - node->centerOfMassX, 2) + powf(centerOfMassY - node->centerOfMassY, 2) + powf(centerOfMassZ - node->centerOfMassZ, 2));
		if ((dist / (node->meanAbsoluteDeviation + meanAbsoluteDeviation)) > a) {
			node->Draw(engine);
		}
		else {
			node->Draw(engine);
		}
	}
}
*/