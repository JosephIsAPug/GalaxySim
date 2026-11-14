#include "UI.h"
UIManager::UIManager(sf::RenderWindow* iwindow) {
	renderWindow = iwindow;
	windowSize = renderWindow->getSize();
}
UIManager::Window* UIManager::addWindow(std::string name, sf::Vector2i pos, sf::Vector2i size) {
	updateOrder.push_back(updateOrder.size());
	windows.push_back(new Window(this, name, pos, size));
	return windows[windows.size() - 1];
}
UIManager::Window* UIManager::getWindow(std::string name) {
	for (UIManager::Window* w : windows) {
		if (w->name == name) {
			return w;
		}
	}
	return nullptr;
}
UIManager::Window* UIManager::getWindow(sf::Vector2i point) {
	for (int i = 0; i < updateOrder.size(); i++) { // get window on top
		Window* w = windows[updateOrder[i]];
		if (pointInRect(point, w->pos, w->pos + w->size)) {
			return w;
		}
	}
	return nullptr;
}
void UIManager::Event(std::optional<sf::Event> event) {
	for (int i = 0; i < updateOrder.size(); i++) {
		Window* w = windows[updateOrder[i]];
		if (w->Event(event)) { // only one window can be modifyed at a time
			if (event->is<sf::Event::MouseButtonPressed>())
				moveIToFront = i;
			break;
		}
	}
	if (event->is<sf::Event::Resized>()) {
		windowSize = renderWindow->getSize();
	}
}
void UIManager::Update() {
	if (moveIToFront != -1) {
		std::rotate(updateOrder.begin(), updateOrder.begin() + moveIToFront, updateOrder.begin() + moveIToFront + 1);
		moveIToFront = -1;
	}
	
	for (int i = 0; i < updateOrder.size(); i++) {
		Window* w = windows[updateOrder[i]];
		w->Transform();
		for (UIManager::Window::Element* e : w->elements) {
			e->Update();
		}
	}
}
void UIManager::Draw(Engine* engine) {
	for (int i = updateOrder.size() - 1; i >= 0; i--) { // reverse order
		Window* w = windows[updateOrder[i]];
		w->Draw(engine);
	}
}

//functions for window class
UIManager::Window::Window(UIManager* imanager, std::string iname, sf::Vector2i ipos, sf::Vector2i isize) {
	manager = imanager;
	name = iname;
	pos = ipos;
	size = isize;

	minSize.x = name.length() * 8 + borderThic + 20;
	Bound();

	Element* e = addElement("exit", "button", sf::Vector2i(-20, 10), sf::Vector2i(10, 10)); // closeing buttons
	e->anchorPoint = 1;
	e->outlineColor = sf::Color::Red;

	e = addElement("minimize", "button", sf::Vector2i(-35, 10), sf::Vector2i(10, 10));
	e->anchorPoint = 1;
	e->outlineColor = sf::Color::Yellow;
}
bool UIManager::Window::Event(std::optional<sf::Event> event) {
	if (closed)
		return false;

	if (const auto resized = event->getIf<sf::Event::Resized>()) {
		//std::cout << resized->size.x - manager->windowSize.x;
		pos.x *= (float)resized->size.x / (float)manager->windowSize.x;
		pos.y *= (float)resized->size.y / (float)manager->windowSize.y;
		size.x *= (float)resized->size.x / (float)manager->windowSize.x;
		size.y *= (float)resized->size.y / (float)manager->windowSize.y;
		Bound();
	}

	sf::Vector2i mPos = sf::Mouse::getPosition(*manager->renderWindow);

	if (event->is<sf::Event::MouseButtonPressed>()) {
		std::cout << "window detects mouse clicked" << std::endl;
		if (mPos.x > pos.x && mPos.x < pos.x + size.x &&
			mPos.y > pos.y && mPos.y < pos.y + minSize.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 0;
			std::cout << "mouse clicked on top" << std::endl;
		}
		if (mPos.x > pos.x + size.x - resizeThresh && mPos.x < pos.x + size.x &&
			mPos.y > pos.y && mPos.y < pos.y + size.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 1;
			std::cout << "mouse clicked on right" << std::endl;
		}
		if (mPos.x > pos.x && mPos.x < pos.x + size.x &&
			mPos.y > pos.y + size.y - resizeThresh && mPos.y < pos.y + size.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 2;
			std::cout << "mouse clicked on bot" << std::endl;
		}
		if (mPos.x > pos.x && mPos.x < pos.x + resizeThresh &&
			mPos.y > pos.y && mPos.y < pos.y + size.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 3;
			std::cout << "mouse clicked on left" << std::endl;
		}
		if (mPos.x > pos.x + size.x - resizeThresh && mPos.x < pos.x + size.x &&
			mPos.y > pos.y + size.y - resizeThresh && mPos.y < pos.y + size.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 4;
			std::cout << "mouse clicked on bot right corner" << std::endl;
		}
		if (mPos.x > pos.x && mPos.x < pos.x + resizeThresh &&
			mPos.y > pos.y + size.y - resizeThresh && mPos.y < pos.y + size.y) {
			dragging = true;
			mlast = mPos;
			resizeSide = 5;
			std::cout << "mouse clicked on bot left corner" << std::endl;
		}
	}
	if (event->is<sf::Event::MouseButtonReleased>()) {
		dragging = false;
	}
	if (pointInRect((sf::Vector2i)mPos, pos, pos + size)) {

		for (Element* e : elements) {
			if (e->Event(event))
				break;
		}
		if (elements[0]->value == 1 && closeable) {
			closed = true;
			dragging = false;
		}
		if (elements[1]->value == 1) {
			std::cout << "minize!!!";
			minimized = !minimized;
			if (minimized) {
				minSize.x = (name.length() + 3) * 7 + borderThic;
				elements[1]->outlineColor = sf::Color::Green;
			}
			else {
				minSize.x = name.length() * 8 + borderThic + 20;
				elements[1]->outlineColor = sf::Color::Yellow;
			}
		}
		return true;
	}
	return false;
}
void UIManager::Window::Transform() {
	if (closed)
		return;

	if (dragging) {
		sf::Vector2i mPos = sf::Mouse::getPosition(*manager->renderWindow);

		sf::Vector2i delta = mPos - mlast;
		mlast = mPos;
		if (resizeSide == 0) {
			pos += delta;
		}
		if (!minimized) {
			if (resizeSide == 1) {
				size.x += delta.x;
			}
			else if (resizeSide == 2) {
				size.y += delta.y;
			}
			else if (resizeSide == 3) {
				pos.x += delta.x;
				size.x -= delta.x;
			}
			else if (resizeSide == 4) {
				size += delta;
			}
			else if (resizeSide == 5) {
				pos.x += delta.x;
				size.x -= delta.x;
				size.y += delta.y;
			}
		}
		Bound();
	}
}
void UIManager::Window::Bound() {
	size.x = fmaxf(size.x, minSize.x);
	size.y = fmaxf(size.y, minSize.y);
	size.x = fminf(size.x, manager->renderWindow->getSize().x);
	size.y = fminf(size.y, manager->renderWindow->getSize().y);

	pos.x = fmaxf(pos.x, 0);
	pos.y = fmaxf(pos.y, 0);
	if (!minimized) {
		pos.x = fminf(pos.x, manager->renderWindow->getSize().x - size.x);
		pos.y = fminf(pos.y, manager->renderWindow->getSize().y - size.y);
	}
	else {
		pos.x = fminf(pos.x, manager->renderWindow->getSize().x - minSize.x);
		pos.y = fminf(pos.y, manager->renderWindow->getSize().y - minSize.y);
	}
}
void UIManager::Window::Draw(Engine* engine) {
	if (closed)
		return;

	if (!minimized) {
		engine->setColor(sf::Color(100, 100, 100, 200));
		engine->drawRoundedRect((sf::Vector2f)pos, (sf::Vector2f)(pos + size), 10, 5);
		engine->setColor(sf::Color(50, 50, 50));
		engine->drawRoundedRectOutline((sf::Vector2f)pos, (sf::Vector2f)(pos + size), 10, -borderThic, 5);
	}
	else {
		engine->setColor(sf::Color(100, 100, 100, 200));
		engine->drawRoundedRect((sf::Vector2f)pos, (sf::Vector2f)(pos + minSize), 10, 5);
		engine->setColor(sf::Color(50, 50, 50));
		engine->drawRoundedRectOutline((sf::Vector2f)pos, (sf::Vector2f)(pos + minSize), 10, -borderThic, 5);
	}
	//if (name.length() * 10 + borderThic < size.x - 20 && !minimized)
	engine->setColor(sf::Color::White);
	if (!minimized)
		engine->drawText(name, (sf::Vector2f)pos + sf::Vector2f(borderThic, borderThic), 12);
	else
		engine->drawText(name.substr(0, name.length() / 2) + "...", (sf::Vector2f)pos + sf::Vector2f(borderThic, borderThic), 12);
	for (Element* e : elements) {
		e->Draw(engine);
	}
}
UIManager::Window::Element* UIManager::Window::addElement(std::string name, std::string type, sf::Vector2i ipos, sf::Vector2i isize) {
	elements.push_back(new Element(this, name, type, ipos, isize));
	return elements[elements.size() - 1];
}
UIManager::Window::Element* UIManager::Window::addElement(std::string name, std::string type, sf::Vector2i ipos, float isize) {
	sf::Vector2i size;
	if (type == "toggle") {
		size = sf::Vector2i(isize * 2, isize);
	}
	else {
		size = sf::Vector2i(isize, isize);
	}
	elements.push_back(new Element(this, name, type, ipos, size));
	return elements[elements.size() - 1];
}
UIManager::Window::Element* UIManager::Window::addElement(std::string name, std::string type, sf::Vector2i ipos, sf::Vector2i isize, float igraphNumValues, float igraphSmoothing) {
	elements.push_back(new Element(this, name, type, ipos, isize, igraphNumValues, igraphSmoothing));
	return elements[elements.size() - 1];
}
UIManager::Window::Element* UIManager::Window::getElement(std::string name) {
	for (UIManager::Window::Element* e : elements) {
		if (e->name == name) {
			return e;
		}
	}
	return nullptr;
}
UIManager::Window::Element* UIManager::Window::getElement(sf::Vector2i point) {
	for (UIManager::Window::Element* e : elements) {
		if (pointInRect(point, e->pos, e->pos + e->size)) {
			return e;
		}
	}
	return nullptr;
}

// functions for element class
UIManager::Window::Element::Element(Window* iwindow, std::string iname, std::string itypeName, sf::Vector2i ipos, sf::Vector2i isize) {
	window = iwindow;
	name = iname;
	typeName = itypeName;
	pos = ipos;
	size = isize;
}
UIManager::Window::Element::Element(Window* iwindow, std::string iname, std::string itypeName, sf::Vector2i ipos, sf::Vector2i isize, float igraphNumValues, float igraphSmoothing) {
	window = iwindow;
	name = iname;
	typeName = itypeName;
	pos = ipos;
	size = isize;

	graphNumValues = igraphNumValues;
	graphSmoothing = igraphSmoothing;
	if (typeName == "graph") {
		for (int i = 0; i < graphNumValues + graphSmoothing; i++) {
			graphValues.push_back(0);
		}
	}
}
bool UIManager::Window::Element::Event(std::optional<sf::Event> event) {
	sf::Vector2i mPos = sf::Mouse::getPosition(*window->manager->renderWindow);
	if (anchorPoint == 0)
		mPos -= (sf::Vector2i)window->pos;
	else {
		mPos -= (sf::Vector2i)window->pos;
		if (!window->minimized)
			mPos.x -= window->size.x;
		else
			mPos.x -= window->minSize.x;
	}
	if ((typeName == "button" || typeName == "toggle") && event->is<sf::Event::MouseButtonPressed>()) {
		if (mPos.x > pos.x && mPos.x < pos.x + size.x &&
			mPos.y > pos.y && mPos.y < pos.y + size.y) {
			if (typeName == "button")
				value = 1;
			else if (typeName == "toggle") {
				//newValue = 1;
				if (value == 0)
					value = 1;
				else
					value = 0;
			}
			return true;
		}
	}
	if (typeName == "parameter") {
		if (auto data = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (mPos.x > pos.x && mPos.x < pos.x + size.x &&
				mPos.y > pos.y && mPos.y < pos.y + size.y) {
				if (data->button == sf::Mouse::Button::Left) {
					oldValue = value;
					dragging = true;
				}
				else if (data->button == sf::Mouse::Button::Right && !typing) {
					typing = true;
					textInput = "";
				}
			}
		}
		if (auto data = event->getIf<sf::Event::TextEntered>()) {
			if (data->unicode != 8) // backspace
				textInput.push_back(data->unicode);
		}
		if (auto data = event->getIf<sf::Event::KeyPressed>()) {
			if (data->code == sf::Keyboard::Key::Enter && typing) {
				typing = false;

				bool correctlyParsed = true;
				float num;
				try {
					size_t pos;
					num = std::stof(textInput, &pos);
					correctlyParsed = pos == textInput.size();
				}
				catch (const std::invalid_argument&) {
					correctlyParsed = false;
				}
				catch (const std::out_of_range&) {
					correctlyParsed = false;
				}

				if (correctlyParsed) {
					value = num;
				}
			}
			if (data->code == sf::Keyboard::Key::Backspace && typing && textInput.size() > 0) {
				textInput.pop_back();// delete last
			}
		}
	}
	return false;
}
void UIManager::Window::Element::Update() {
	if (typeName == "button") {
		value = 0;
	}
	if (typeName == "toggle") {
		oldValue = value;
	}
	if (typeName == "parameter") {
		if (dragging) {
			sf::Vector2i mPos = sf::Mouse::getPosition(*window->manager->renderWindow);
			float centerX = window->pos.x + pos.x + size.x / 2;

			float mult = mPos.x - centerX;
			mult /= size.x / 2;
			mult = fmaxf(fminf(mult, 1), -1);
			mult = pow(2, mult);
			//std::cout << mult << std::endl;
			value = oldValue * mult;

			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				dragging = false;
				oldValue = value;
			}
		}
	}
}
void UIManager::Window::Element::GraphAddValue(float v) {
	if (typeName == "graph") {
		graphValues.insert(graphValues.begin(), v);
		graphValues.pop_back();
	}
}
void UIManager::Window::Element::Draw(Engine* engine) {
	sf::Vector2i rel = window->pos;
	if (anchorPoint == 1) {
		if (!window->minimized)
			rel.x += window->size.x;
		else
			rel.x += window->minSize.x;
	}
	
	if (typeName == "text") {
		engine->setColor(color);

		sf::Vector2i offset = sf::Vector2i();
		int maxLetters = fminf(fminf(size.x, window->size.x - pos.x - window->borderThic) / size.y * 1.6, discription.length());
		int place = 0;
		while (place < discription.length()) {
			if (!window->minimized && (pos.y + size.y + offset.y + window->borderThic > window->size.y) ||
				window->minimized && (pos.y + size.y + offset.y + window->borderThic > window->minSize.y))
				return;
			engine->drawText(discription.substr(place, maxLetters), (sf::Vector2f)(rel + pos + offset), size.y);
			offset.y += size.y;
			place += maxLetters;
		}
	}

	if (!window->minimized && (pos.x + size.x + window->borderThic > window->size.x || pos.y + size.y + window->borderThic > window->size.y) ||
		window->minimized && (pos.x + size.x + window->borderThic > window->minSize.x || pos.y + size.y + window->borderThic > window->minSize.y))
		return;
	if (typeName == "button") {
		if (value == 0)
			engine->setColor(color);
		else
			engine->setColor(color2);
		engine->drawRoundedRect((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, 5);
		engine->setColor(outlineColor);
		engine->drawRoundedRectOutline((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, -outlineThic, 5); // draw relitive to window
	}
	if (typeName == "toggle") {
		engine->setColor(backColor); 
		engine->drawRoundedRect((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), size.x, 5);// backgroung
		if (value == 0)
			engine->setColor(color);
		else
			engine->setColor(color2);
		sf::Vector2i c1;
		sf::Vector2i c2;
		if (value == 0) {
			c1 = rel + pos + sf::Vector2i(outlineThic, outlineThic);
			c2 = rel + pos + sf::Vector2i(size.y - outlineThic, size.y - outlineThic);
		}
		else {
			c1 = rel + pos + sf::Vector2i(size.x - size.y + outlineThic, outlineThic);
			c2 = rel + pos + sf::Vector2i(size.x, size.y - outlineThic);
		}
		engine->drawRoundedRect((sf::Vector2f)c1, (sf::Vector2f)c2, size.x, 6); // handle
		
		engine->setColor(outlineColor); // outline
		engine->drawRoundedRectOutline((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), size.x, -outlineThic, 6); // draw relitive to window
	}
	if (typeName == "graph") {
		engine->setColor(color);
		engine->drawRoundedRect((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, 5);
		

		float max = 0;
		for (float v : graphValues) {
			//std::cout << "value: " << v << std::endl;
			if (v > max) {
				max = v;
			}
		}
		//std::cout << "max value: " << max << std::endl;
		if (max == 0) {
			return;
		}
		
		sf::VertexArray VA{ sf::PrimitiveType::TriangleStrip, static_cast<size_t>(graphNumValues * 2) };
		for (int i = 0; i < graphNumValues; i++) {
			float v = 0;
			for (int i2 = 0; i2 < graphSmoothing + 1; i2++) {
				v += graphValues[graphNumValues + i2 - 1 - i];
			}
			v /= graphSmoothing + 1;
			float x = ((float)size.x - outlineThic * 2) / (graphNumValues - 1) * i;
			VA[i * 2 + 0].position = (sf::Vector2f)(rel + pos + sf::Vector2i(x, (size.y - outlineThic * 2) * 0.9 * -v / max) + sf::Vector2i(outlineThic, size.y - outlineThic));
			VA[i * 2 + 1].position = (sf::Vector2f)(rel + pos + sf::Vector2i(x, 0) + sf::Vector2i(outlineThic, size.y - outlineThic));

			VA[i * 2 + 0].color = color2;
			VA[i * 2 + 1].color = color2;

			x += outlineThic;
			if (x < 20) {
				VA[i * 2 + 1].position.y -= 20 - sqrt(pow(20, 2) - pow(20 - x, 2));
			}
			else if (x > size.x - 20) {
				VA[i * 2 + 1].position.y -= 20 - sqrt(pow(20, 2) - pow(size.x - 20 - x, 2));
			}
		}
		
		window->manager->renderWindow->draw(VA);

		engine->setColor(outlineColor);
		engine->drawRoundedRectOutline((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, -5, 5); // draw relitive to window
	}
	if (typeName == "parameter") {
		engine->setColor(color);
		if (typing)
			engine->setColor(color2);
		engine->drawRoundedRect((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, 5);

		sf::RectangleShape r = sf::RectangleShape({ (float)outlineThic, (float)size.y });
		r.setPosition((sf::Vector2f)(rel + pos + sf::Vector2i(size.x / 2, 0)));
		window->manager->renderWindow->draw(r);

		engine->setColor(outlineColor);
		engine->drawRoundedRectOutline((sf::Vector2f)(rel + pos), (sf::Vector2f)(rel + pos + size), 20, -outlineThic, 5); // draw relitive to window
		std::string text = std::to_string(value).substr(0, 6);
		if (typing)
			text = textInput;
		if (dragging)
			text = std::to_string(value);
		engine->setColor(sf::Color::White);
		sf::Vector2i offset = sf::Vector2i(outlineThic * 2, outlineThic);
		engine->drawText(text, (sf::Vector2f)(rel + pos + offset), size.y / 2);
	}
}
//misc functions
bool UIManager::pointInRect(sf::Vector2i p, sf::Vector2i c1, sf::Vector2i c2) {
	if (p.x >= c1.x && p.x <= c2.x &&
		p.y >= c1.y && p.y <= c2.y) {
		return true;
	}
	return false;
}