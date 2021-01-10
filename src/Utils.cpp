#include <NasNas.h>
#include "Utils.hpp"

bool segmentIntersection(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& q1, const sf::Vector2f& q2, sf::Vector2f& intersect) {
    static sf::Vector2f p; p.x = p2.x - p1.x; p.y = p2.y - p1.y;
    static sf::Vector2f q; q.x = q2.x - q1.x; q.y = q2.y - q1.y;
    static sf::Vector2f q1p1; q1p1.x = q1.x - p1.x; q1p1.y = q1.y - p1.y;
    static sf::Vector2f q1p1_inv; q1p1_inv.x = -q1p1.x; q1p1_inv.y = -q1p1.y;

    float d = ns::cross_product(p, q);

    if (d == 0.f)
        return false;

    auto t = ns::cross_product(q1p1, q) / d;
    auto u = ns::cross_product(q1p1_inv, p) / -d;

    intersect.x = p1.x + t * p.x;
    intersect.y = p1.y + t * p.y;
    return (0 <= t && t <= 1 && 0 <= u && u <= 1);
}

bool circleRectIntersection(const sf::Vector2f& circle_pos, float radius, const sf::FloatRect& rect, sf::Vector2f& overlap) {
    // circle rect collision test
    overlap.x = 0;
    overlap.y = 0;
    if (circle_pos.x + radius < rect.left) return false;
    if (circle_pos.x - radius > rect.left + rect.width) return false;
    if (circle_pos.y + radius < rect.top) return false;
    if (circle_pos.y - radius > rect.top + rect.height) return false;

    if (circle_pos.x < rect.left) {
        if (circle_pos.x + radius > rect.left) {
            overlap.x = circle_pos.x + radius - rect.left;
            return true;
        }
    }
    else if (circle_pos.x > rect.left + rect.width) {
        if (circle_pos.x - radius < rect.left + rect.width) {
            overlap.x = circle_pos.x - radius - rect.left - rect.width;
            return true;
        }
    }

    if (circle_pos.y < rect.top) {
        if (circle_pos.y + radius > rect.top) {
            overlap.y = circle_pos.y + radius - rect.top;
            return true;
        }
    }
    else if (circle_pos.y > rect.top + rect.height) {
        if (circle_pos.y - radius < rect.top + rect.height) {
            overlap.y = circle_pos.y - radius - rect.top - rect.height;
            return true;
        }
    }
    return false;
}