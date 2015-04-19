// Copyright 2015 Michael Percy <mpercy@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Program to print something to the console.

#include <iostream>
#include <set>
#include <sstream>
#include <string>

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

// Holds a BitMap and the size of the "canvas" we are writing on.
struct CanvasBitMap {
  int width;
  int height;
  BitMap bitmap;
};

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

// Print something to the console.
int main(int argc, char** argv) {
  CanvasBitMap canvas_bitmap;
  canvas_bitmap.width = 32;
  canvas_bitmap.height = 5;
  canvas_bitmap.bitmap.insert(Point(0, 1));
  canvas_bitmap.bitmap.insert(Point(0, 2));
  canvas_bitmap.bitmap.insert(Point(0, 3));
  canvas_bitmap.bitmap.insert(Point(1, 0));
  canvas_bitmap.bitmap.insert(Point(1, 4));
  canvas_bitmap.bitmap.insert(Point(2, 0));
  canvas_bitmap.bitmap.insert(Point(2, 4));
  canvas_bitmap.bitmap.insert(Point(4, 1));
  canvas_bitmap.bitmap.insert(Point(4, 2));
  canvas_bitmap.bitmap.insert(Point(4, 3));
  canvas_bitmap.bitmap.insert(Point(4, 4));
  canvas_bitmap.bitmap.insert(Point(5, 0));
  canvas_bitmap.bitmap.insert(Point(5, 2));
  canvas_bitmap.bitmap.insert(Point(6, 1));
  canvas_bitmap.bitmap.insert(Point(6, 2));
  canvas_bitmap.bitmap.insert(Point(6, 3));
  canvas_bitmap.bitmap.insert(Point(6, 4));
  canvas_bitmap.bitmap.insert(Point(8, 0));
  canvas_bitmap.bitmap.insert(Point(8, 1));
  canvas_bitmap.bitmap.insert(Point(8, 2));
  canvas_bitmap.bitmap.insert(Point(8, 3));
  canvas_bitmap.bitmap.insert(Point(8, 4));
  canvas_bitmap.bitmap.insert(Point(9, 1));
  canvas_bitmap.bitmap.insert(Point(9, 2));
  canvas_bitmap.bitmap.insert(Point(9, 3));
  canvas_bitmap.bitmap.insert(Point(10, 0));
  canvas_bitmap.bitmap.insert(Point(10, 1));
  canvas_bitmap.bitmap.insert(Point(10, 2));
  canvas_bitmap.bitmap.insert(Point(10, 3));
  canvas_bitmap.bitmap.insert(Point(10, 4));
  canvas_bitmap.bitmap.insert(Point(12, 0));
  canvas_bitmap.bitmap.insert(Point(12, 1));
  canvas_bitmap.bitmap.insert(Point(12, 2));
  canvas_bitmap.bitmap.insert(Point(12, 3));
  canvas_bitmap.bitmap.insert(Point(12, 4));
  canvas_bitmap.bitmap.insert(Point(13, 0));
  canvas_bitmap.bitmap.insert(Point(13, 4));
  canvas_bitmap.bitmap.insert(Point(14, 1));
  canvas_bitmap.bitmap.insert(Point(14, 2));
  canvas_bitmap.bitmap.insert(Point(14, 3));
  canvas_bitmap.bitmap.insert(Point(16, 0));
  canvas_bitmap.bitmap.insert(Point(16, 1));
  canvas_bitmap.bitmap.insert(Point(16, 2));
  canvas_bitmap.bitmap.insert(Point(16, 3));
  canvas_bitmap.bitmap.insert(Point(16, 4));
  canvas_bitmap.bitmap.insert(Point(17, 0));
  canvas_bitmap.bitmap.insert(Point(17, 2));
  canvas_bitmap.bitmap.insert(Point(17, 4));
  canvas_bitmap.bitmap.insert(Point(18, 0));
  canvas_bitmap.bitmap.insert(Point(18, 2));
  canvas_bitmap.bitmap.insert(Point(18, 4));
  canvas_bitmap.bitmap.insert(Point(20, 0));
  canvas_bitmap.bitmap.insert(Point(20, 1));
  canvas_bitmap.bitmap.insert(Point(20, 2));
  canvas_bitmap.bitmap.insert(Point(20, 3));
  canvas_bitmap.bitmap.insert(Point(20, 4));
  canvas_bitmap.bitmap.insert(Point(21, 4));
  canvas_bitmap.bitmap.insert(Point(22, 4));
  canvas_bitmap.bitmap.insert(Point(24, 1));
  canvas_bitmap.bitmap.insert(Point(24, 2));
  canvas_bitmap.bitmap.insert(Point(24, 3));
  canvas_bitmap.bitmap.insert(Point(24, 4));
  canvas_bitmap.bitmap.insert(Point(25, 0));
  canvas_bitmap.bitmap.insert(Point(25, 2));
  canvas_bitmap.bitmap.insert(Point(26, 1));
  canvas_bitmap.bitmap.insert(Point(26, 2));
  canvas_bitmap.bitmap.insert(Point(26, 3));
  canvas_bitmap.bitmap.insert(Point(26, 4));
  canvas_bitmap.bitmap.insert(Point(29, 0));
  canvas_bitmap.bitmap.insert(Point(29, 1));
  canvas_bitmap.bitmap.insert(Point(29, 2));
  canvas_bitmap.bitmap.insert(Point(29, 4));

  RenderBitmap(canvas_bitmap);
  return 0;
}
