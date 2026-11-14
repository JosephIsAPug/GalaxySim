#pragma once
//#include <vector>
#include <array>
#include "ParticleSystem.h"
//#include <SFML/Graphics.hpp>
#include "SFML functions.h"

//#include "Node.h"
class OctTree
{
public:
	class Point {
	public:
		float x;
		float y;
		float z;

		float fx = 0;
		float fy = 0;
		float fz = 0;

		float mass = 1;

		int index;
		int nearestIndex = -1;
		float nearestDist = 1;
		Point(float ix, float iy, float iz, float mass=0, int index=0);
	};
	class Node {
	public:
		float x1;
		float y1;
		float z1;
		float x2;
		float y2;
		float z2;

		bool leaf = true;
		int childrenIndex[8] = { 0 };
		int myIndex;
		int depth = -1;

		float mass = 0;
		float centerOfMassX = 0;
		float centerOfMassY = 0;
		float centerOfMassZ = 0;
		float meanAbsoluteDeviation = 0;
		float forceX = 0;
		float forceY = 0;
		float forceZ = 0;

		OctTree* tree;

		int pStartIndex = 0;
		int pEndIndex = 0;

		//std::vector<int> points;

		Node(OctTree* itree, int imyIndex);
		void setData(int idepth, float ix1, float iy1, float iz1, float ix2, float iy2, float iz2);
		void Draw(Engine* engine) const;
		int FindNodeOfPoint(Point* p);
		void SortPoints();
		void CalcCenterOfMass();

		bool Contains(Point* p);
		bool ErrorFunction(Node* node, float dist);

		void Grav(std::vector<int> nodeIndexes);
		void SelfGrav();
		void NodeToNodeGrav(Node* node);
		void NodeToNodeGravSubDiv(Node* node);
		void NodeToNodeGravAllPoints(Node* node);
		void PropagateForce();
		void ApplyForce();
		void Debug(Engine* engine, float offsetX, float offsetY, float offsetZ, float a);
	};
	const int poolSize = 20000;
	int firstBlankNodeIndex = 1;

	const int maxPointsPerNode = 16;
	float theta = 1;
	double G = 0;
	float expo = 2;

	int operations = 0;

	std::vector<Node> nodeList;
	std::vector<Point> pointList;
	//ParticleSystem* system;
	OctTree(float x1, float y1, float z1, float x2, float y2, float z2);
	void GravParms(float t, double g, float e);
	void AddPoint(Point p);
	void Draw(Engine* engine);
	void Build();
	void Split(int index);
	void Grav();
	void PointGrav(Point* p, Node* node);
	void Print();

	
};
