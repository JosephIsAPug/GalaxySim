#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>



class Engine {
	sf::RenderWindow* window;

	float offsetX = 0;
	float offsetY = 0;
	float offsetZ = 0;
	float scale = 1;
	float rotX = 0;
	float rotY = 0;
	float zoom = 1;

	sf::Color color = sf::Color::White;

	sf::Font font = sf::Font("Audiowide-Regular.ttf");

	class Point {
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		Point() : x(0), y(0), z(0) {}
		Point(float ix, float iy, float iz) {
			x = ix;
			y = iy;
			z = iz;
		}
		void RotateX(float r) {
			float i = y; // prevent errors
			y = cos(r) * i - sin(r) * z;
			z = cos(r) * z + sin(r) * i;
			//std::cout << "points: " << x << " | " << y << " | " << z << std::endl;
		}
		void RotateY(float r) {
			float i = x; // prevent errors
			x = cos(r) * i - sin(r) * z;
			z = cos(r) * z + sin(r) * i;
			//std::cout << "points: " << x << " | " << y << " | " << z << std::endl;
		}
	};
public:
	Engine(sf::RenderWindow* w) {
		window = w;
	}
	// change value functions
	void Offset(float x, float y, float z) {
		offsetX += x;
		offsetY += y;
		offsetZ += z;
	}
	void RotateX(float r) {
		rotX += r;
	}
	void RotateY(float r) {
		rotY += r;
	}

	// set value functions
	void setOffset(float x, float y, float z) {
		offsetX = x;
		offsetY = y;
		offsetZ = z;
	}
	void setOffsetZ(float z) {
		offsetZ = z;
	}
	void setScale(float s) {
		scale = s;
	}
	void setRotationX(float r) {
		rotX = r;
	}
	void setRotationY(float r) {
		rotY = r;
	}
	void setZoom(float z) {
		zoom = z;
	}
	void setColor(sf::Color c) {
		color = c;
	}

	// get value functions
	float getOffsetX() {
		return offsetX;
	}
	float getOffsetY() {
		return offsetY;
	}
	float getOffsetZ() {
		return offsetZ;
	}

	// draw functions
	void drawCube(float x1, float y1, float z1, float x2, float y2, float z2) {
		//if (z1 + offsetZ <= 0)
			//return;
		Point points[8];
		points[0] = Point(x1, y1, z1);
		points[1] = Point(x2, y1, z1);
		points[2] = Point(x2, y2, z1);
		points[3] = Point(x1, y2, z1);

		points[4] = Point(x1, y2, z2);
		points[5] = Point(x2, y2, z2);
		points[6] = Point(x2, y1, z2);
		points[7] = Point(x1, y1, z2);

		for (int i = 0; i < 8; i++) { 
			//points[i].x += offsetX;
			//points[i].y -= offsetY;
			//points[i].z += offsetZ;

			points[i].RotateY(rotY); 
			points[i].RotateX(rotX); 
		}

		sf::VertexArray line(sf::PrimitiveType::LineStrip, 16);

		for (int i = 0; i < 16; i++) {
			int i2 = i;
			if (i == 4) {
				i2 = 0;
			}
			if (i > 4) {
				i2 = 12 - i;
			}
			if (i == 9) {
				i2 = 7;
			}
			if (i > 9) {
				i2 = 14 - i;
			}
			if (i == 13) {
				i2 = 5;
			}
			if (i == 14) {
				i2 = 6;
			}
			if (i == 15) {
				i2 = 1;
			}
			float x = points[i2].x * scale;
			float y = points[i2].y * scale;
			float z = points[i2].z * scale;

			x += offsetX;
			y -= offsetY;
			z += offsetZ;

			x /= abs(z) / zoom;
			y /= abs(z) / zoom;

			x += window->getSize().x / 2;
			y += window->getSize().y / 2;

			line[i].position = sf::Vector2f(x, y);
			
			if (z > 0)
				line[i].color = color;
			else {
				line[i].color = sf::Color::Transparent;
				line[fmaxf(i - 1, 0)].color = sf::Color::Transparent;
				//line[fminf(i + 1, 15)].color = sf::Color::Transparent;
			}
		}

		window->draw(line);
	}
	void drawPointCloud(std::vector<sf::Vector3f> points, std::vector <sf::Color> colors) {
		
		const int numPoints = points.size();
		sf::VertexArray VA{ sf::PrimitiveType::Points, static_cast<size_t>(numPoints)};
		for (int i = 0; i < numPoints; i++) {
			Point p(points[i].x, points[i].y, points[i].z);

			p.RotateY(rotY);
			p.RotateX(rotX);
			

			float x = p.x * scale;
			float y = p.y * scale;
			float z = p.z * scale;
			
			x += offsetX;
			y -= offsetY;
			z += offsetZ;

			x /= abs(z) / zoom;
			y /= abs(z) / zoom;

			x += window->getSize().x / 2;
			y += window->getSize().y / 2;

			VA[i].position = sf::Vector2f(x, y);
			if (z > 0)
				VA[i].color = colors[i];
			else {
				VA[i].color = sf::Color::Transparent;
			}
		}
		window->draw(VA);
	}
	void drawRoundedRect(sf::Vector2f c1, sf::Vector2f c2, float rad, int qual) {
		rad = fminf(rad, (c2.x - c1.x) / 2);
		rad = fminf(rad, (c2.y - c1.y) / 2);

		c1 += sf::Vector2f(rad, rad);
		c2 -= sf::Vector2f(rad, rad);

		sf::VertexArray VA{ sf::PrimitiveType::TriangleFan, static_cast<size_t>(qual * 4) };
		for (int c = 0; c < 4; c++) {
			sf::Vector2f base;
			if (c == 0) {
				base = c1;
			}
			else if (c == 1) {
				base = sf::Vector2f(c2.x, c1.y);
			}
			else if (c == 2) {
				base = c2;
			}
			else if (c == 3) {
				base = sf::Vector2f(c1.x, c2.y);
			}
			for (int i = 0; i < qual; i++) {
				VA[c * qual + i].position = base + getCirPoint(rad, (2 * 3.1415 / (4 * qual)) * (c * qual + i + 0.5) + 3.1415 * 1.5);
				VA[c * qual + i].color = color;
			}
		}
		window->draw(VA);
	}
	void drawRoundedRectOutline(sf::Vector2f c1, sf::Vector2f c2, float rad, float thic, int qual) {
		rad = fminf(rad, (c2.x - c1.x) / 2);
		rad = fminf(rad, (c2.y - c1.y) / 2);

		c1 += sf::Vector2f(rad, rad);
		c2 -= sf::Vector2f(rad, rad);

		sf::VertexArray VA{ sf::PrimitiveType::TriangleStrip, static_cast<size_t>(qual * 8 + 2) };
		for (int c = 0; c < 4; c++) {
			sf::Vector2f base;
			if (c == 0) {
				base = c1;
			}
			else if (c == 1) {
				base = sf::Vector2f(c2.x, c1.y);
			}
			else if (c == 2) {
				base = c2;
			}
			else if (c == 3) {
				base = sf::Vector2f(c1.x, c2.y);
			}
			for (int i = 0; i < qual; i++) {
				VA[(c * qual + i) * 2].position = base + getCirPoint(rad, (2 * 3.1415 / (4 * qual)) * (c * qual + i + 0.5) + 3.1415 * 1.5);
				VA[(c * qual + i) * 2].color = color;

				VA[(c * qual + i) * 2 + 1].position = base + getCirPoint(rad + thic, (2 * 3.1415 / (4 * qual)) * (c * qual + i + 0.5) + 3.1415 * 1.5);
				VA[(c * qual + i) * 2 + 1].color = color;
			}
		}
		VA[qual * 8].position = c1 + getCirPoint(rad, (2 * 3.1415 / (4 * qual)) * (0.5) + 3.1415 * 1.5);
		VA[qual * 8].color = color;

		VA[qual * 8 + 1].position = c1 + getCirPoint(rad + thic, (2 * 3.1415 / (4 * qual)) * (0.5) + 3.1415 * 1.5);
		VA[qual * 8 + 1].color = color;

		window->draw(VA);
	}
	sf::Vector2f getCirPoint(float rad, float ang) {
		return sf::Vector2f(sinf(ang), -cosf(ang)) * rad; // y is reversed
	}
	void drawText(std::string str, sf::Vector2f pos, int size) {
		sf::Text text(font);
		text.setString(str);
		text.setPosition(pos);
		text.setCharacterSize(size);
		text.setFillColor(color);
		window->draw(text);
	}
};