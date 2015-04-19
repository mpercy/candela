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

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include "bdf.h"

#define ENFORCE(cond, msg) \
  do { \
    bool _cond = (cond); \
    if (!_cond) { \
      cerr << "Parse error: " << msg << endl; \
      return false; \
    } \
  } while (false) \

bool g_debug = false;
bool g_print_code = false;

#define DLOG() g_debug && cerr

using std::cerr;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::map;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;

void SplitString(const string& in, char delim, vector<string>* out) {
  out->clear();
  std::stringstream ss(in);
  std::string item;
  while (getline(ss, item, delim)) {
    out->push_back(item);
  }
}

// Decode a hexadecimal string to an integer.
uint32_t HexDecode(const std::string& hex_val) {
  assert(hex_val.size() == 2);
  uint32_t tot_val = 0;
  int len = hex_val.length();
  for (int i = 0; i < len; i++) {
    uint32_t val = 0;
    char c = hex_val[len - (i + 1)];
    switch (c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        val = c - '0';
        break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
        val = 10 + c - 'A';
        break;
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
        val = 10 + c - 'a';
        break;
      default:
        assert(0);
    }
    tot_val += (val << (i * 4));
  }
  return tot_val;
}

// Quick and dirty string-to-int parser.
// This silently ignores the case where there are trailing non-digits in the string.
int ToInt(const std::string& str) {
  int i;
  sscanf(str.c_str(), "%d", &i);
  return i;
}

void BDFChar::Clear() {
  name.clear();
  code = -1;
  scalable_width = Point(0, 0);
  device_width = Point(0, 0);
  bbx.width = 0;
  bbx.height = 0;
  bbx.x_offset = 0;
  bbx.y_offset = 0;
  bitmap.clear();
}

bool BDFFont::Load(const std::string& filename) {
  ifstream infile(filename.c_str());
  if (infile.bad()) {
    cerr << "Cannot open infile for reading: " << filename << endl;
    return false;
  }

  map<int, BDFChar> char_map;
  BDFChar bdf_char;
  bool parsing_char = false;
  bool parsing_bitmap = false;

  string line;
  vector<string> tokens;
  while (getline(infile, line)) {
    SplitString(line, ' ', &tokens);
    if (tokens.empty()) continue;
    string first_token = tokens[0];
    if (first_token == "STARTCHAR") {
      if (tokens.size() != 2)
      ENFORCE(!parsing_char, "STARTCHAR encountered without intervening ENDCHAR");
      parsing_char = true;
      ENFORCE(tokens.size() == 2, "STARTCHAR must be followed with char name");
      bdf_char.Clear();
      bdf_char.name = tokens[1];
    } else if (first_token == "ENDCHAR") {
      ENFORCE(parsing_char, "ENDCHAR encountered without intervening STARTCHAR");
      chars_.insert(pair<char, BDFChar>(bdf_char.code, bdf_char));
      parsing_bitmap = false;
      parsing_char = false;
    } else if (first_token == "ENCODING") {
      ENFORCE(tokens.size() == 2, "ENCODING takes 1 argument");
      bdf_char.code = ToInt(tokens[1]);
    } else if (first_token == "SWIDTH") {
      ENFORCE(tokens.size() == 3, "SWIDTH takes 2 arguments");
      bdf_char.scalable_width.x = ToInt(tokens[1]);
      bdf_char.scalable_width.y = ToInt(tokens[2]);
    } else if (first_token == "DWIDTH") {
      ENFORCE(tokens.size() == 3, "DWIDTH takes 2 arguments");
      bdf_char.device_width.x = ToInt(tokens[1]);
      bdf_char.device_width.y = ToInt(tokens[2]);
    } else if (first_token == "BBX") {
      ENFORCE(tokens.size() == 5, "BBX takes 4 arguments");
      bdf_char.bbx.width = ToInt(tokens[1]);
      bdf_char.bbx.height = ToInt(tokens[2]);
      bdf_char.bbx.x_offset = ToInt(tokens[3]);
      bdf_char.bbx.y_offset = ToInt(tokens[4]);
      if (bdf_char.bbx.height + bdf_char.bbx.y_offset > max_char_height_) {
        max_char_height_ = bdf_char.bbx.height + bdf_char.bbx.y_offset;
      }
    } else if (first_token == "BITMAP") {
      ENFORCE(!parsing_bitmap, "BITMAP encountered twice without intervening ENDCHAR");
      parsing_bitmap = true;
    } else if (parsing_bitmap) {
      ENFORCE(tokens.size() == 1, "More than one token in bitmap found");
      ENFORCE(first_token.size() <= 2, "We only support bitmap entries with 8 bits or less");
      // Bitmap char.
      uint8_t bits = HexDecode(first_token);
      bdf_char.bitmap.push_back(bits);
    }

    infile.clear();
  }
  return true;
}

bool BDFFont::TextToBitmap(const std::string& text, CanvasBitMap* canvas) {
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    DLOG() << "DEBUG: ------------" << endl;
    DLOG() << "DEBUG: Rendering character: " << c << endl;
    if (!RenderLetter(c, canvas)) return false;
  }
  return true;
}

bool BDFFont::RenderLetter(char c, CanvasBitMap* canvas) {
  map<char, BDFChar>::const_iterator iter = chars_.find(c);
  if (iter == chars_.end()) {
    cerr << "Error: Cannot find character in bitmap: " << c << endl;
    return false;
  }
  const BDFChar& bdf_char = iter->second;
  const vector<uint8_t>& bm = bdf_char.bitmap;
  for (int y = max_char_height_; y >= 0; y--) {
    int width = bdf_char.bbx.width;
    int height = bdf_char.bbx.height;
    int y_offset = bdf_char.bbx.y_offset;
    DLOG() << "DEBUG: --" << endl;
    DLOG() << "DEBUG: y pixel: " << y << endl;
    DLOG() << "DEBUG: BBX char width: " << width << endl;
    if (y < y_offset) continue;
    if (y > y_offset + height - 1) continue;
    // The bitmaps are specified high y-coord first.
    int bitmap_index = height - y + y_offset - 1;
    DLOG() << "DEBUG: bitmap index: " << bitmap_index << endl;
    uint8_t b = bm[bitmap_index];
    DLOG() << std::hex;
    DLOG() << "DEBUG: Seen bitmap val: 0x" << static_cast<int>(b) << endl;
    DLOG() << std::dec;
    for (uint8_t x = 0; x < bdf_char.bbx.width; x++) {
      uint8_t val_at_bit = 1 << (7 - x);
      DLOG() << std::hex;
      DLOG() << "DEBUG: Looking for bit: 0x" << (int)val_at_bit << endl;
      DLOG() << std::dec;
      if (b & val_at_bit) {
        // Render a Point @ (x,y) with the bbx offsets.
        int p_x = x + bdf_char.bbx.x_offset + canvas->width;
        int p_y = y + bdf_char.bbx.y_offset;
        Point p(p_x, p_y);
        DLOG() << "DEBUG: Adding Point: " << p.ToString() << endl;
        canvas->bitmap.insert(p);
      }
    }
  }

  canvas->width += bdf_char.device_width.x;
  canvas->height = max_char_height_;
  return true;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " font.bdf phrase_to_render" << endl;
    return 1;
  }
  BDFFont font;
  if (!font.Load(argv[1])) {
    cerr << "Cannot load font file " << argv[1] << endl;
    return 1;
  }
  CanvasBitMap canvas_bitmap;
  if (!font.TextToBitmap(argv[2], &canvas_bitmap)) {
    cerr << "Cannot render phrase " << argv[2] << endl;
    return 1;
  }
  if (g_print_code) {
    PrintBitmapAsCode(canvas_bitmap);
  }
  RenderBitmap(canvas_bitmap);
  return 0;
}
