// sfml test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <omp.h>

#include "ParticleSystem.h"
#include "Mis.h"
#include "OctTree.h"
#include "UI.h"
#include "SFML functions.h"

#include "FastNoiseLite.h"


int main()
{
    std::cout << "Hello World!\n";

    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML YAY!", sf::Style::Default);
    window.setFramerateLimit(60);

    float universeSize = 1;
    const float timeMult = 0.1;
    float t = 0.01;
    float totalT = 0;
    float G = 0.0000001;
    int pNum = 10000;
    int mode = 1;

    ParticleSystem particleSystem = ParticleSystem();
    ParticleSystem::ParticleGroup allParticles = ParticleSystem::ParticleGroup(&particleSystem);
    std::vector<ParticleSystem*> objects = std::vector<ParticleSystem*>();

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(4);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);

    if (mode == 1) {
        for (int i = 0; i < pNum * 0.5; i++) {
            float z = Rnd_Float(-1, 1);
            float r = sqrt(1 - z * z);
            float a = Rnd_Float(0, 2 * 3.1415);
            float x = cos(a) * r;
            float y = sin(a) * r;
            float l = powf(Rnd_Float(0.01, 1), 1.0 / 3.0);
            x *= l * 0.95;
            y *= l * 0.95;
            z *= l * 0.95;
            float vx = z / l * 0.01 + Rnd_Float(-1, 1) * 0;
            float vy = 0 + Rnd_Float(-1, 1) * 0;
            float vz = -x / l * 0.01 + Rnd_Float(-1, 1) * 0;
            ParticleSystem::Particle p(Vector3(x - 5, y * 0.25, z) * 0.2, Vector3(vx, vy, vz), 1000.0 / pNum);
            allParticles.addNewParticle(p);
        }
        for (int i = 0; i < pNum * 0.5; i++) {
            float z = Rnd_Float(-1, 1);
            float r = sqrt(1 - z * z);
            float a = Rnd_Float(0, 2 * 3.1415);
            float x = cos(a) * r;
            float y = sin(a) * r;
            float l = powf(Rnd_Float(0.01, 1), 1.0 / 3.0);
            x *= l * 0.95;
            y *= l * 0.95;
            z *= l * 0.95;
            float vx = z / l * 0.01 + Rnd_Float(-1, 1) * 0;
            float vy = 0 + Rnd_Float(-1, 1) * 0;
            float vz = -x / l * 0.01 + Rnd_Float(-1, 1) * 0;
            ParticleSystem::Particle p(Vector3(x + 5, y * 0.25, z) * 0.2, Vector3(vx, vy, vz), 1000.0 / pNum);
            allParticles.addNewParticle(p);
        }
    }

    if (mode == 0) {
        G *= 20;
        for (int i = 0; i < pNum; i++) {
            float z = Rnd_Float(-1, 1);
            float r = sqrt(1 - z * z);
            float a = Rnd_Float(0, 2 * 3.1415);
            float x = cos(a) * r;
            float y = sin(a) * r;
            float l = powf(Rnd_Float(0, 1), 1.0 / 3.0);
            x *= l * 0.95;
            y *= l * 0.95;
            z *= l * 0.95;
            float vx = x / powf(l, 0) * 0.1 + Rnd_Float(-1, 1) * 0;
            float vy = y / powf(l, 0) * 0.1 + Rnd_Float(-1, 1) * 0;
            float vz = z / powf(l, 0) * 0.1 + Rnd_Float(-1, 1) * 0;
            float rnd = Rnd_Float(0, (noise.GetNoise(x, y, z) + 1) / 2);
            if (rnd < 0.3) { i--; continue; }
            ParticleSystem::Particle p(Vector3(x, y, z), Vector3(vx, vy, vz), 1000.0 / pNum);
            allParticles.addNewParticle(p);
        }
    }
    if (mode == 2) {
        //G *= 1;
        for (int i = 0; i < pNum; i++) {
            float z = Rnd_Float(-1, 1);
            float r = sqrt(1 - z * z);
            float a = Rnd_Float(0, 2 * 3.1415);
            float x = cos(a) * r;
            float y = sin(a) * r;
            float l = powf(Rnd_Float(0, 1), 1.0 / 3.0);
            x *= l;
            y *= l;
            z *= l;
            float vx = Rnd_Float(-1, 1);
            float vy = Rnd_Float(-1, 1);
            float vz = Rnd_Float(-1, 1);
            float speed = std::hypot(vx, vy, vz);
            float newSpeed = Rnd_Float(0, 1) * 0.01;
            vx /= speed / newSpeed;
            vy /= speed / newSpeed;
            vz /= speed / newSpeed;
            ParticleSystem::Particle p(Vector3(x, y, z), Vector3(vx, vy, vz), 1000.0 / pNum);
            allParticles.addNewParticle(p);
        }
    }
    //std::cout << points.particles.size() << " Paricles spawned" << std::endl;

    Engine engine = Engine(&window);
    engine.setOffset(0, 0, 1);
    bool drawTree = false;
    bool dragging = false;
    float dragX = 0;
    float dragY = 0;
    int pointDrawMode = 1;
    //float camX = 0;
    //float camY = 0;
    //float camZ = -2;
    //float camRot = 0;

    UIManager UI = UIManager(&window);
    {
        UIManager::Window* w;
        UIManager::Window::Element* e;

        w = UI.addWindow("WindowPannel", sf::Vector2i(800, 0), sf::Vector2i(150, 400));
        w->closeable = false;
        w->elements[0]->outlineColor = sf::Color(50, 50, 50);

        e = w->addElement("RenderSettingsText", "text", sf::Vector2i(30, 25), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "RenderSettings";
        e = w->addElement("openRenderSettings", "button", sf::Vector2i(10, 25), 20);
        e->outlineThic = 4;
        e->discription = "Opens the window containing RenderSettings";

        e = w->addElement("DiscriptionsText", "text", sf::Vector2i(30, 50), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "Discriptions";
        e = w->addElement("openDiscriptions", "button", sf::Vector2i(10, 50), 20);
        e->outlineThic = 4;
        e->discription = "Opens the window containing Discriptions";

        e = w->addElement("PerformeceText", "text", sf::Vector2i(30, 75), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "Performece";
        e = w->addElement("openPerformece", "button", sf::Vector2i(10, 75), 20);
        e->outlineThic = 4;
        e->discription = "Opens the window containing Performece info";

        e = w->addElement("SimulationSettingsText", "text", sf::Vector2i(30, 100), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "SimulationSettings";
        e = w->addElement("openSimulationSettings", "button", sf::Vector2i(10, 100), 20);
        e->outlineThic = 4;
        e->discription = "Opens the window containing SimulationSettings";


        w = UI.addWindow("RenderSettings", sf::Vector2i(500, 0), sf::Vector2i(150, 180));

        e = w->addElement("fullScreenText", "text", sf::Vector2i(55, 25), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "FullScreen";
        e = w->addElement("fullScreen", "toggle", sf::Vector2i(10, 25), 20);
        e->outlineThic = 4;
        e->discription = "Sets window to fullscreen";

        e = w->addElement("drawTreeText", "text", sf::Vector2i(55, 50), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "DrawTree";
        e = w->addElement("drawTree", "toggle", sf::Vector2i(10, 50), 20);
        e->outlineThic = 4;
        e->discription = "toggles drawing the quadtree";

        e = w->addElement("drawVelocityText", "text", sf::Vector2i(30, 75), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "DrawVelocity";
        e = w->addElement("drawVelocity", "button", sf::Vector2i(10, 75), 20);
        e->outlineThic = 4;
        e->discription = "Draws the particles color based on velocity";

        e = w->addElement("drawAccelerationText", "text", sf::Vector2i(30, 100), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "DrawAcceleration";
        e = w->addElement("drawAcceleration", "button", sf::Vector2i(10, 100), 20);
        e->outlineThic = 4;
        e->discription = "Draws the particles color based on acceleration";

        e = w->addElement("drawParticlesColorContrastText", "text", sf::Vector2i(80, 125), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "Color Contrast";
        e = w->addElement("drawParticlesColorContrast", "parameter", sf::Vector2i(10, 125), sf::Vector2i(70, 20));
        e->discription = "Value controling the color contrast for drawing particles";
        e->value = 20;
        e->outlineThic = 3;
        e->color = sf::Color(50, 50, 50);
        e->color2 = sf::Color(100, 100, 100);

        e = w->addElement("drawConstentRatioText", "text", sf::Vector2i(55, 150), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "DrawConstentRatio";
        e = w->addElement("drawConstentRatio", "toggle", sf::Vector2i(10, 150), 20);
        e->outlineThic = 4;
        e->discription = "Draws the particles maintaning the universes size";
        

        w = UI.addWindow("Discriptions", sf::Vector2i(800, 800), sf::Vector2i(150, 150));

        e = w->addElement("discriptionText", "text", sf::Vector2i(10, 20), sf::Vector2i(1000, 12));


        w = UI.addWindow("Performece", sf::Vector2i(0, 800), sf::Vector2i(190, 220));
        w->minSize.x = 190;

        e = w->addElement("numParticlesText", "text", sf::Vector2i(10, 20), sf::Vector2i(1000, 12));

        e = w->addElement("timePerFrameText", "text", sf::Vector2i(10, 48), sf::Vector2i(1000, 12));

        e = w->addElement("timeBuildTreeText", "text", sf::Vector2i(10, 60), sf::Vector2i(1000, 12));

        e = w->addElement("timeGravForceText", "text", sf::Vector2i(10, 72), sf::Vector2i(1000, 12));
       
        e = w->addElement("timeGraph", "graph", sf::Vector2i(10, 100), sf::Vector2i(170, 100), 100, 0);
        e->color = sf::Color(100, 100, 100);
        e->outlineColor = sf::Color(150, 150, 150);
        e->discription = "graphs time per frame over last 100 frames";
        e = w->addElement("timeGraphText", "text", sf::Vector2i(30, 105), sf::Vector2i(1000, 12));
        e->color = sf::Color::White;
        e->discription = "Time Graph";


        w = UI.addWindow("SimulationSettings", sf::Vector2i(0, 200), sf::Vector2i(150, 150));

        e = w->addElement("pauseText", "text", sf::Vector2i(55, 25), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "Pause Simulation";
        e = w->addElement("pause", "toggle", sf::Vector2i(10, 25), 20);
        e->outlineThic = 4;
        e->discription = "Pauses simulation";
        e->value = 1; // start paused

        e = w->addElement("timestepText", "text", sf::Vector2i(100, 50), sf::Vector2i(1000, 12));
        e->color = sf::Color::White;
        e->discription = "timestep";
        e = w->addElement("timestep", "parameter", sf::Vector2i(10, 50), sf::Vector2i(80, 20));
        e->discription = "Value controling the timestep for simulation updates";
        e->value = t;
        e->outlineThic = 3;
        e->color = sf::Color(50, 50, 50);
        e->color2 = sf::Color(100, 100, 100);

        e = w->addElement("thetaText", "text", sf::Vector2i(100, 75), sf::Vector2i(1000, 12));
        e->color = sf::Color::White;
        e->discription = "theta";
        e = w->addElement("theta", "parameter", sf::Vector2i(10, 75), sf::Vector2i(80, 20));
        e->discription = "Value controling the accuracy of gravity for simulation updates";
        e->value = 0.5;
        e->outlineThic = 3;
        e->color = sf::Color(50, 50, 50);
        e->color2 = sf::Color(100, 100, 100);


        w = UI.addWindow("Spawner", sf::Vector2i(0, 350), sf::Vector2i(150, 150));

        e = w->addElement("drawGhostText", "text", sf::Vector2i(55, 25), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "Draw Ghost";
        e = w->addElement("drawGhost", "toggle", sf::Vector2i(10, 25), 20);
        e->outlineThic = 4;
        e->discription = "Draws ghost particles where they will be placed";

        e = w->addElement("placeGhostText", "text", sf::Vector2i(30, 50), sf::Vector2i(300, 12));
        e->color = sf::Color::White;
        e->discription = "RenderSettings";
        e = w->addElement("placeGhost", "button", sf::Vector2i(10, 50), 20);
        e->outlineThic = 4;
        e->discription = "Places particles";
    }
    
    int frame = 0;

    while (window.isOpen()) {
        frame++;
        while (const std::optional event = window.pollEvent()) {
            UI.Event(event);

            if (event->is<sf::Event::Closed>())
                window.close();
            if (event->is<sf::Event::Resized>()) {
                sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(window.getSize()));
                window.setView(sf::View(visibleArea));
            }
            //if (event->is<sf::Event::)
            if (event->is<sf::Event::MouseButtonPressed>()) {
                std::cout << "click start" << std::endl;
                const sf::Event::MouseButtonPressed* data = event->getIf<sf::Event::MouseButtonPressed>();
                if (data->button == sf::Mouse::Button::Left)
                    drawTree = !drawTree;
                if (data->button == sf::Mouse::Button::Right) {
                    std::cout << "startDrag" << std::endl;
                    dragging = true;
                    dragX = data->position.x;
                    dragY = data->position.y;
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                std::cout << "click releaced" << std::endl;
                const sf::Event::MouseButtonReleased* data = event->getIf<sf::Event::MouseButtonReleased>();
                if (data->button == sf::Mouse::Button::Right)
                    dragging = false;
            }
            if (event->is<sf::Event::MouseWheelScrolled>()) {
                const sf::Event::MouseWheelScrolled* data = event->getIf<sf::Event::MouseWheelScrolled>();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
                    engine.RotateY(powf(data->delta, 2) * Sign(data->delta) * 0.05);
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                    engine.RotateX(powf(data->delta, 2) * Sign(data->delta) * -0.05);
                else
                    engine.setOffsetZ(engine.getOffsetZ() * powf(1.1, -data->delta));
                //std::cout << engine.getOffsetZ() << std::endl;
                //camX -= (1 - powf(1.025, data->delta)) * window.getSize().x / camZoom;// * 0.5;
                //camY += (1 - powf(1.025, data->delta)) * window.getSize().y / camZoom;// * 0.5;
                //camZoom *= powf(1.05, data->delta);

            }
            if (event->is<sf::Event::KeyPressed>()) {
                const sf::Event::KeyPressed* data = event->getIf<sf::Event::KeyPressed>();
                if (data->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }
        const float scrollSpeed = 0.01;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            engine.Offset(0, scrollSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            engine.Offset(scrollSpeed, 0, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            engine.Offset(0, -scrollSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            engine.Offset(-scrollSpeed, 0, 0);
        }
        if (dragging) {
            //engine.Offset((dragX - sf::Mouse::getPosition(window).x) / window.getSize().x * -engine.getOffsetZ(), 0, 0);
            engine.Offset((dragX - sf::Mouse::getPosition(window).x) / window.getSize().x * -engine.getOffsetZ(),
                (dragY - sf::Mouse::getPosition(window).y) / window.getSize().y * engine.getOffsetZ(), 0);
            dragX = sf::Mouse::getPosition(window).x;
            dragY = sf::Mouse::getPosition(window).y;
            //sf::Mouse::getPosition(window);
            //std::cout << sf::Mouse::getPosition(window).x << std::endl;
            //std::cout << universeSize << std::endl;
        }

        if (UI.getWindow("RenderSettings")->getElement("fullScreen")->oldValue != UI.getWindow("RenderSettings")->getElement("fullScreen")->value) {
            if (UI.getWindow("RenderSettings")->getElement("fullScreen")->value == 1) {
                window.create(sf::VideoMode({ 800, 800 }), "SFML YAY!", sf::Style::Default, sf::State::Fullscreen);
            }
            else {
                window.create(sf::VideoMode({ 800, 800 }), "SFML YAY!", sf::Style::Default, sf::State::Windowed);
            }
            UI.Event(sf::Event::Resized(window.getSize()));
        }

        if (mode == 0 && UI.getWindow("SimulationSettings")->getElement("pause")->value == 0) {
            //universeSize += 0.005 * t;
            universeSize *= powf(2 - universeSize * 0.0001, t);
            //t = fminf(universeSize * 10, 1000);
            //t = (float)(frame * frame) / (frame * frame + 100000000) * log(universeSize) + 0.0001;
        }

        std::chrono::duration<double, std::milli> timeTotal;
        std::chrono::duration<double, std::milli> time_span;
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        

        // *** create oct tree with THETA VALUE ***
        OctTree tree = OctTree(-1, -1, -1, 1, 1, 1);

        for (int i = 0; i < allParticles.indies.size(); i++) {
            ParticleSystem::Particle p = particleSystem.particlePool[allParticles.indies[i]];
            tree.AddPoint(OctTree::Point(p.pos, p.mass, i));
        }
        tree.Build();
        //tree.Print();
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        time_span = t2 - t1;
        timeTotal = time_span;
        t1 = t2;
        UI.getWindow("Performece")->getElement("numParticlesText")->discription = "Number of Particles: " + std::to_string(allParticles.indies.size());
        if (frame % 10 == 0)
            std::cout << "Num points: " << allParticles.indies.size() << " Mass: " << tree.nodeList[0].mass;
        if (frame % 10 == 0)
            std::cout << " Ms | TreeBuild: " << time_span.count();
        UI.getWindow("Performece")->getElement("timeBuildTreeText")->discription = "Time to Build Tree: " + std::to_string((int)time_span.count()) + "ms";

        tree.GravParms(UI.getWindow("SimulationSettings")->getElement("theta")->value, G / powf(universeSize, 2), 2);
        
        tree.Grav();

        t2 = std::chrono::high_resolution_clock::now();
        time_span = t2 - t1;
        timeTotal += time_span;
        t1 = t2;
        if (frame % 10 == 0)
            std::cout << " Grav/Intg: " << time_span.count() << " FLOPS: " << tree.operations;
        UI.getWindow("Performece")->getElement("timeGravForceText")->discription = "Time to Calc Grav: " + std::to_string((int)time_span.count()) + "ms";
        //*
        for (int i = 0; i < tree.pointList.size(); i++) {
            OctTree::Point* point = &tree.pointList[i];
            ParticleSystem::Particle* p = &particleSystem.particlePool[allParticles.indies[point->index]];
            p->acc = point->force / p->mass;

        }

        if (UI.getWindow("SimulationSettings")->getElement("pause")->value == 0) {
            allParticles.UpdateVel(t);
            allParticles.UpdatePos(t, universeSize);

            float size = tree.nodeList[0].meanAbsoluteDeviation * 2;
            
            for (int i : allParticles.indies) {
                ParticleSystem::Particle* p = &particleSystem.particlePool[i];
                p->pos /= size;
            }
            universeSize *= size;

            totalT += t;
            t = UI.getWindow("SimulationSettings")->getElement("timestep")->value;
        }



        t2 = std::chrono::high_resolution_clock::now();
        time_span = t2 - t1;
        timeTotal += time_span;
        t1 = t2;
        if (frame % 10 == 0)
            std::cout << " Mis: " << time_span.count();

        window.clear();

        
        engine.setZoom(800);

        

        if (UI.getWindow("RenderSettings")->getElement("drawConstentRatio")->value == 1)
            engine.setScale(1);
        else
            engine.setScale(1 * universeSize);
        sf::Color c = sf::Color{ 0, 255, 0, 20 };
        engine.setColor(c);
        if (UI.getWindow("RenderSettings")->getElement("drawTree")->value == 1)
            tree.Draw(&engine);
        /*
        if (UI.getWindow("Spawner")->getElement("drawGhost")->value == 1) {
            Rnd_SetSeed(1);
            toSpawn.clear();
            toSpawn.spawnGalaxy(1000, 0, 0, 0, 0, 0, 0, 1);
            Rnd_RndSeed();

            toSpawn.Draw(&engine, UI.getWindow("RenderSettings")->getElement("drawParticlesColorContrast")->value, pointDrawMode);
            if (UI.getWindow("Spawner")->getElement("placeGhost")->value == 1) {
                pNum += 1000;
                points.addSystem(&toSpawn);
            }
        }
        */
        allParticles.Draw(&engine, UI.getWindow("RenderSettings")->getElement("drawParticlesColorContrast")->value, pointDrawMode);
        

        sf::Vector2 pos = sf::Mouse::getPosition(window);
        float size = window.getSize().y; //window size

        
        c = sf::Color{ 255, 0, 0, 100 };
        engine.setColor(c);
        engine.drawCube(-1, -1, -1, 1, 1, 1);
        c = sf::Color{ 200, 200, 0, 100 };
        engine.setColor(c);
        //engine.setZoom(800 / universeSize);
        if (UI.getWindow("RenderSettings")->getElement("drawConstentRatio")->value == 1)
            engine.setScale(1 / universeSize);
        else
            engine.setScale(1);
        engine.drawCube(-1, -1, -1, 1, 1, 1);
        
        if (UI.getWindow("WindowPannel")->getElement("openRenderSettings")->value == 1)
            UI.getWindow("RenderSettings")->closed = !UI.getWindow("RenderSettings")->closed;
        if (UI.getWindow("WindowPannel")->getElement("openDiscriptions")->value == 1)
            UI.getWindow("Discriptions")->closed = !UI.getWindow("Discriptions")->closed;
        if (UI.getWindow("WindowPannel")->getElement("openPerformece")->value == 1)
            UI.getWindow("Performece")->closed = !UI.getWindow("Performece")->closed;
        if (UI.getWindow("WindowPannel")->getElement("openSimulationSettings")->value == 1)
            UI.getWindow("SimulationSettings")->closed = !UI.getWindow("SimulationSettings")->closed;

        if (UI.getWindow("RenderSettings")->getElement("drawVelocity")->value == 1)
            pointDrawMode = 0;
        if (UI.getWindow("RenderSettings")->getElement("drawAcceleration")->value == 1)
            pointDrawMode = 1;
        {
            UI.getWindow("Discriptions")->getElement("discriptionText")->discription = "N/A";
            UIManager::Window* w = UI.getWindow(pos);
            if (w != nullptr) {
                //std::cout << w->name << std::endl;
                UIManager::Window::Element* e = w->getElement(pos - w->pos);
                if (e != nullptr) {
                    //std::cout << e->name << std::endl;
                    UI.getWindow("Discriptions")->getElement("discriptionText")->discription = e->discription;
                }
            }
        }
        

        UI.Draw(&engine);
        UI.Update();
        
        
        // *** text displays ***

        
        engine.setColor(sf::Color::Blue);

        std::string s = "UniverseSize: " + std::to_string(universeSize);
        engine.drawText(s, sf::Vector2f(), 24);

        s = "TimeStep: " + std::to_string(t);
        engine.drawText(s, sf::Vector2f(0, 25), 24);

        s = "TimeElapsed: " + std::to_string(totalT);
        engine.drawText(s, sf::Vector2f(0, 50), 24);

        window.display();

        t2 = std::chrono::high_resolution_clock::now();
        time_span = t2 - t1;
        timeTotal += time_span;
        t1 = t2;
        if (frame % 10 == 0)
            std::cout << " Drawing: " << time_span.count() << " total: " << timeTotal.count() << std::endl;

        
        UI.getWindow("Performece")->getElement("timePerFrameText")->discription = "Time Per frame: " + std::to_string((int)timeTotal.count()) + "ms";
        UI.getWindow("Performece")->getElement("timeGraph")->GraphAddValue((int)timeTotal.count());
    }
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
