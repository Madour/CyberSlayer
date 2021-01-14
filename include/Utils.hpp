#pragma once

/**
 * \brief Returns true if there is an intersection between segments [p1, p2] and [q1, q1].
 * Returns the intersection point in the intersect parameter.
 */
bool segmentIntersection(const sf::Vector2f& p1, const sf::Vector2f& p2,
                         const sf::Vector2f& q1, const sf::Vector2f& q2,
                         sf::Vector2f& intersect);

bool circleRectIntersection(const sf::Vector2f& circle_pos, float radius, const sf::FloatRect& rect, sf::Vector2f& overlap);
