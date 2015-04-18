// Glyph Bitmap Distribution Format (BDF) font representation.

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "bitmap.h"

struct BoundingBox {
  int width;
  int height;
  int x_offset;
  int y_offset;
};

// Representation of a BDF character.
struct BDFChar {
  // Reset all fields to their default values.
  void Clear();

  std::string name;
  int code;
  Point scalable_width;
  Point device_width;
  BoundingBox bbx;
  std::vector<uint8_t> bitmap;
};

class BDFFont {
 public:
  // Constructor.
  BDFFont() : max_char_height_(-1) {}

  // Load the font file.
  // Specify the name of the .bdf file you wish to load in 'filename'.
  // If the font cannot be loaded from 'filename', an error will be printed
  // on the console and false will be returned. Otherwise, returns true.
  bool Load(const std::string& filename);

  // Renders the given text as a bitmap.
  // If the text could not be rendered, prints an error to stderr and returns
  // false. Otherwise, returns true.
  bool TextToBitmap(const std::string& text, CanvasBitMap* bitmap);

 private:
  // Render a single letter onto its own private bitmap.
  bool RenderLetter(char c, CanvasBitMap* letter_bitmap);

  // Combine several bitmaps into a single one.
  // Returns false if and only if the letter bitmaps are not compatible.
  void CombineBitmaps(const std::vector<CanvasBitMap>& letter_bitmaps, CanvasBitMap* phrase_bitmap);

  // Map of code -> character.
  std::map<char, BDFChar> chars_;
  int max_char_height_;
};