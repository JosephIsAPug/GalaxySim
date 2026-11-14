/*
#pragma once
//#include <iostream>
#include <SFML/Graphics.hpp>
class OctTree;

class Node 
{
private:
	class Point {
	public:
		float x;
		float y;
	};
public:
	float x1;
	float y1;
	float x2;
	float y2;

	bool leaf = true;
	int childrenIndex[4] = { 0 };
	int myIndex;

	OctTree* tree;

	std::vector<Point> points;
	Node(OctTree* itree, int imyIndex, float ix1, float iy1, float ix2, float iy2) {
		tree = itree;
		myIndex = imyIndex;
		x1 = ix1;
		y1 = iy1;
		x2 = ix2;
		y2 = iy2;
	}
	void Draw(sf::RenderWindow* window) {
		sf::RectangleShape rect({ (x2 - x1) * window->getSize().x, (y2 - y1) * window->getSize().y });
		rect.setPosition({ x1 * window->getSize().x, y1 * window->getSize().y });
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Green);
		rect.setOutlineThickness(10);
		
		window->draw(rect);
	}
	void AddPoint(Point p) {
		points.push_back(p);
	}
	void DistributePoints(int maxPointsPerNode) {
		if (leaf) { return; }
		if (points.size() > maxPointsPerNode) {
			for (int i = 0; i < points.size(); i++) {
				Point* p = &points[i];
				if (p->x < tree->nodeList[childrenIndex[0]].x2) {
					if (p->y < tree->nodeList[childrenIndex[0]].y2) {
						//top left
						tree->nodeList[childrenIndex[0]].AddPoint(points[i]);
					}
					else
					{
						//bot left
						tree->nodeList[childrenIndex[2]].AddPoint(points[i]);
					}
				}
				else
				{
					if (p->y < tree->nodeList[childrenIndex[0]].y2) {
						//top right
						tree->nodeList[childrenIndex[1]].AddPoint(points[i]);
					}
					else
					{
						//bot right
						tree->nodeList[childrenIndex[3]].AddPoint(points[i]);
					}
				}
			}
		}
	}
};
*/
