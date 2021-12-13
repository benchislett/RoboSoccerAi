#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

int main() {
  BallChaseEnv env;

  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);

  SfDebugDraw debugDraw(&window);
  env.world->SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(0x00ff);

  window.setFramerateLimit(fps);

  BallChaseAgent agent;

  int hits = 0;
  int i    = 0;
  while (window.isOpen()) {
    window.clear(sf::Color(0, 0, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
      env.player.drive(0.8, 0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
      env.player.drive(-0.8, -0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
      env.player.drive(-0.5, 0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
      env.player.drive(0.5, -0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
      env.player.body->SetLinearVelocity(b2Vec2(0, 0));
      env.player.body->SetAngularVelocity(0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
      break;
    }

    env.step();
    env.act(agent.action(env.state()));

    env.world->DebugDraw();
    window.display();

    i++;
  }

  return 0;
}
