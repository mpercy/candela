#include <stdint.h>

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Represents an (x,y) point.
struct Point {
  Point() : x(0), y(0) {}
  Point(int x, int y) : x(x), y(y) {}
  std::string ToString() {
    std::stringstream ss;
    ss << "x: " << x << ", y: " << y;
    return ss.str();
  }
  int x;
  int y;
};

// Comparator so we can put Point in a std::map.
struct ComparePoint {
    bool operator() (const Point& a, const Point& b) const{
      if (a.x < b.x) return true;
      if (a.x == b.x && a.y < b.y) return true;
      return false;
    }
};

typedef std::set<Point, ComparePoint> BitMap; // TODO: Use a std::unordered_set.

struct CanvasBitMap {
  int width;
  int height;
  BitMap bitmap;
};

// Print 'canvas_bitmap' as C++ source code.
void PrintBitmapAsCode(const CanvasBitMap& canvas_bitmap) {
  std::cout << "CanvasBitMap canvas_bitmap;" << std::endl;
  std::cout << "canvas_bitmap.width = " << canvas_bitmap.width << ";" << std::endl;
  std::cout << "canvas_bitmap.height = " << canvas_bitmap.height << ";" << std::endl;
  for (BitMap::const_iterator iter = canvas_bitmap.bitmap.begin();
       iter != canvas_bitmap.bitmap.end();
       ++iter) {
    std::cout << "canvas_bitmap.bitmap.insert("
         << "Point(" << iter->x << ", " << iter->y << "));" << std::endl;
  }
}

// Render 'canvas_bitmap' on the console.
void RenderBitmap(const CanvasBitMap& canvas_bitmap) {
  for (int y = 0; y < canvas_bitmap.height; y++) {
    for (int x = 0; x < canvas_bitmap.width; x++) {
      Point p(x, y);
      if (canvas_bitmap.bitmap.count(p) > 0) {
        std::cout << "#";
      } else {
        std::cout << " ";
      }
    }
    std::cout << std::endl;
  }
}
