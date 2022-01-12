#pragma once

#include "misc.hpp"

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

sf::Color b2ColorToSfColor(b2Color color, int alpha = 255);

class SfDebugDraw : public b2Draw {
  sf::RenderWindow* window;

  public:
  SfDebugDraw(sf::RenderWindow* win) : window(win) {}
  ~SfDebugDraw() {}

  void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color, bool fill) {
    sf::ConvexShape poly;
    poly.setPointCount(vertex_count);
    for (int i = 0; i < vertex_count; i++) {
      b2Vec2 vertex = vertices[i];
      poly.setPoint(i, sf::Vector2f(vertex.x * length, vertex.y * length));
    }
    poly.setOutlineColor(b2ColorToSfColor(color, 50));
    if (fill) {
      poly.setFillColor(b2ColorToSfColor(color));
    } else {
      poly.setFillColor(sf::Color::Transparent);
    }
    window->draw(poly);
  }

  void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
    DrawPolygon(vertices, vertex_count, color, false);
  }

  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
    DrawPolygon(vertices, vertex_count, color, true);
  }

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color, bool fill) {
    const float r = radius * length;
    sf::CircleShape c(r);
    c.setOrigin(r, r);
    c.setPosition(center.x * length, center.y * length);
    c.setOutlineColor(b2ColorToSfColor(color, 50));
    if (fill) {
      c.setFillColor(b2ColorToSfColor(color));
    } else {
      c.setFillColor(sf::Color::Transparent);
    }
    window->draw(c);
  }

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    DrawCircle(center, radius, color, false);
  }

  void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    DrawCircle(center, radius, color, true);
  }

  void DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
    sf::Vertex pt(sf::Vector2f(p.x * length, p.y * length), b2ColorToSfColor(color));
    window->draw(&pt, 1, sf::Points);
  }

  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    sf::Vertex p[2];
    p[0] = sf::Vertex(sf::Vector2f(p1.x * length, p1.y * length), b2ColorToSfColor(color));
    p[1] = sf::Vertex(sf::Vector2f(p2.x * length, p2.y * length), b2ColorToSfColor(color));
    window->draw(p, 2, sf::Lines);
  }

  void DrawTransform(const b2Transform& xf) {
    const float line_length = 0.05; // box2d units

    b2Vec2 p1 = xf.p;
    DrawSegment(p1, p1 + (line_length * xf.q.GetXAxis()), b2Color(1.f, 0.f, 0.f));
    DrawSegment(p1, p1 - (line_length * xf.q.GetYAxis()), b2Color(0.f, 1.f, 0.f));
  }
};
