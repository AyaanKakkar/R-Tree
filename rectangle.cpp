#include <rectangle.h>
#include <iostream>
#include <cmath>

using namespace std;

#define MIN(a, b) (a) < (b) ? a : b
#define MAX(a, b) (a) > (b) ? a : b

// Default Constructor to create placeholder
Rectangle::Rectangle() {
    minX_ = 1;
    maxX_ = -1;
    minY_ = 1;
    maxY_ = -1;
}

// Constructor
Rectangle::Rectangle(long double minX, long double maxX, long double minY, long double maxY) {
    minX_ = minX;
    maxX_ = maxX;
    minY_ = minY;
    maxY_ = maxY;
}

// Calculate the intersection of two Rectangles
Rectangle Rectangle::intersection(const Rectangle& rect1, const Rectangle& rect2) {
    
    // Calculate the params of the intersection
    long double minX = MAX(rect1.minX_, rect2.minX_);
    long double maxX = MIN(rect1.maxX_, rect2.maxX_);
    long double minY = MAX(rect1.minY_, rect2.minY_);
    long double maxY = MIN(rect1.maxY_, rect2.maxY_);
    
    return Rectangle(minX, maxX, minY, maxY);
}

// Calculate the union of two Rectangles
Rectangle Rectangle::combine(const Rectangle& rect1, const Rectangle& rect2) {
    
    // Calculate the params of the Union
    long double minX = MIN(rect1.minX_, rect2.minX_);
    long double maxX = MAX(rect1.maxX_, rect2.maxX_);
    long double minY = MIN(rect1.minY_, rect2.minY_);
    long double maxY = MAX(rect1.maxY_, rect2.maxY_);

    return Rectangle(minX, maxX, minY, maxY);
}

// Calculate the area of the Rectangle
long double Rectangle::getArea() {
    return (maxX_ - minX_) * (maxY_ - minY_);
}

// Calculate the area enlargement required to combine the two rectangles
long double Rectangle::getAreaEnlargement(const Rectangle& rect) {
    return combine(*this, rect).getArea() - getArea();
}

// Check if the rectange contains a particular point
bool Rectangle::containsPoint(long double x, long double y) {
    return (x <= maxX_ && x >= minX_) && (y <= maxY_ && y >= minY_);
}


// Calculate the euclidean distance between the midpoints of the two rectangles
long double Rectangle::distance(const Rectangle& rect1, const Rectangle& rect2) {

    long double midX1 = (rect1.maxX_ + rect1.minX_) / 2;
    long double midX2 = (rect2.maxX_ + rect2.minX_) / 2;
    long double midY1 = (rect1.maxY_ + rect1.minY_) / 2;
    long double midY2 = (rect2.maxY_ + rect2.minY_) / 2;

    return sqrt((midX1 - midX2) * (midX1 - midX2) + (midY1 - midY2) * (midY1 - midY2));
}

// Print the rectangle
void Rectangle::print() {
    cout << "(X : " << minX_ << " - " << maxX_ << "; ";
    cout << "Y : " << minY_ << " - " << maxY_ << ")";
}

bool Rectangle::isValid() {
    return minX_ <= maxX_ && minY_ <= maxY_;
}

long double Rectangle::getCenterX() {
    return (minX_ + maxX_) / 2;
}

long double Rectangle::getCenterY() {
    return (minY_ + maxY_) / 2;
}

bool Rectangle::containsRect(Rectangle rect) {
    return (minX_ <= rect.minX_ && minY_ <= rect.minY_
        && maxX_ >= rect.maxX_ && maxY_ >= rect.maxY_);
}