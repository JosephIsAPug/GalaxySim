#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <SFML/Graphics.hpp>

#include "SFML functions.h"
class UIManager {
public:
	class Window
	{
	public:
		class Element {
		public:
			Window* window;
			std::string name;
		
			std::string typeName;
			std::string discription = "";

			sf::Vector2i pos;
			sf::Vector2i size;
			int anchorPoint = 0;

			int outlineThic = 5;
			sf::Color color = sf::Color::Red;
			sf::Color color2 = sf::Color::Green;
			sf::Color backColor = sf::Color(50, 50, 50);
			sf::Color outlineColor = sf::Color(150, 150, 150);

			float value = 0;
			std::vector<float> graphValues;
			int graphNumValues = 10;
			int graphSmoothing = 0;
			bool dragging = false;
			float oldValue = 0;
			bool typing = false;
			std::string textInput = "";

			Element(Window* iwindow, std::string iname, std::string itypeName, sf::Vector2i ipos, sf::Vector2i isize);
			Element(Window* iwindow, std::string iname, std::string itypeName, sf::Vector2i ipos, sf::Vector2i isize, float igraphNumValues, float igraphSmoothing);
			bool Event(std::optional<sf::Event> event);
			void Update();
			void GraphAddValue(float v);
			void Draw(Engine* engine);
		};
		UIManager* manager;

		std::string name;

		sf::Vector2i pos;
		sf::Vector2i size;

		// magic numbers
		sf::Vector2i minSize = sf::Vector2i(50, 30);
		const int resizeThresh = 10;
		const int borderThic = 5;

		bool dragging = false;
		sf::Vector2i mlast = sf::Vector2i();
		int resizeSide = 0;
		bool minimized = false;
		bool closed = false;
		bool closeable = true;
	public:
		std::vector<Element*> elements;
	
		Window(UIManager* imanager, std::string iname, sf::Vector2i ipos = sf::Vector2i(), sf::Vector2i isize = sf::Vector2i(50, 100));
		Element* addElement(std::string iname, std::string type, sf::Vector2i ipos = sf::Vector2i(), sf::Vector2i isize = sf::Vector2i(20, 20));
		Element* addElement(std::string iname, std::string type, sf::Vector2i ipos = sf::Vector2i(), float isize = 20);
		Element* addElement(std::string iname, std::string type, sf::Vector2i ipos, sf::Vector2i isize, float igraphNumValues, float igraphSmoothing);
		Element* getElement(std::string name);
		Element* getElement(sf::Vector2i point);
		bool Event(std::optional<sf::Event> event);
		void Transform();
		void Bound();
		void Draw(Engine* engine);
	};
private:
	sf::RenderWindow* renderWindow;
	sf::Vector2u windowSize;

	std::vector<Window*> windows;

	std::vector<int> updateOrder;
	int moveIToFront = -1;
public:
	UIManager(sf::RenderWindow* iwindow);
	Window* addWindow(std::string iname = "", sf::Vector2i ipos = sf::Vector2i(), sf::Vector2i isize = sf::Vector2i(50, 100));
	Window* getWindow(std::string name);
	Window* getWindow(sf::Vector2i point);
	void Event(std::optional<sf::Event> event);
	void Update();
	void Draw(Engine* engine);

	static bool pointInRect(sf::Vector2i p, sf::Vector2i c1, sf::Vector2i c2);
};
