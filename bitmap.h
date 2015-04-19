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

#include <stdint.h>

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

const char kRenderChar = '#';

// Represents an (x,y) point.
struct Point {
  Point() : x(0), y(0) {}
  Point(int x, int y) : x(x), y(y) {}
  std::string ToString() {
    std::stringstream ss;
    ss << "(" << x << ", " << y << ")";
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
  CanvasBitMap() : width(0), height(0) {}
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
//
// The way the bitmaps are represented, the (0,0) coordinate is in the
// bottom-left corner of the screen.
void RenderBitmap(const CanvasBitMap& canvas_bitmap) {
  for (int y = canvas_bitmap.height - 1; y >= 0; y--) {
    //std::cout << y << ": ";
    for (int x = 0; x < canvas_bitmap.width; x++) {
      Point p(x, y);
      if (canvas_bitmap.bitmap.count(p) > 0) {
        std::cout << kRenderChar;
      } else {
        std::cout << " ";
      }
    }
    std::cout << std::endl;
  }
}
