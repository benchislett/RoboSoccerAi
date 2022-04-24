#include "visualize.hpp"

sf::Color b2ColorToSfColor(b2Color color) {
  return sf::Color((sf::Uint8)(color.r * 255), (sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
                   (sf::Uint8)(color.a * 255));
}
