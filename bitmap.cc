#include "bitmap.h"

#include <iostream>

// Simple test for bitmap.h
int main(int argc, char** argv) {
  CanvasBitMap canvas_bitmap;
  canvas_bitmap.width = 3;
  canvas_bitmap.height = 3;
  canvas_bitmap.bitmap.insert(Point(0, 0));
  canvas_bitmap.bitmap.insert(Point(1, 2));
  canvas_bitmap.bitmap.insert(Point(2, 2));
  PrintBitmapAsCode(canvas_bitmap);
  RenderBitmap(canvas_bitmap);
}
