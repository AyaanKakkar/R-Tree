#ifndef _RECTANGLE
#define _RECTANGLE

class Rectangle {
    
    /*
        Used to declare the MBR of a node in the rtree.
        Also contains functions to calculate the intersection and the union of MBRs
    */

    private:
        // Bounding box params
        long double minX_, maxX_, minY_, maxY_;
    
    public:
        Rectangle();
        Rectangle(long double minX, long double maxX, long double minY, long double maxY);
        static Rectangle intersection(const Rectangle& rect1, const Rectangle& rect2);
        static Rectangle combine(const Rectangle& rect1, const Rectangle& rect2); 
        static long double distance(const Rectangle& rect1, const Rectangle& rect2);
        long double getArea();
        long double getAreaEnlargement(const Rectangle& rect);
        bool containsPoint(long double x, long double y);
        void print();
        bool isValid();
        long double getCenterX();
        long double getCenterY();
        bool containsRect(Rectangle rect);
};

#endif