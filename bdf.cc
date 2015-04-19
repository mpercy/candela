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
      if (bdf_char.bbx.height > max_char_height_) {
        max_char_height_ = bdf_char.bbx.height;
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

bool BDFFont::TextToBitmap(const std::string& text, CanvasBitMap* phrase_bitmap) {
  vector<CanvasBitMap> per_letter_blocks;
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    cerr << "DEBUG: ------------" << endl;
    cerr << "DEBUG: Rendering character: " << c << endl;
    CanvasBitMap canvas;
    if (!RenderLetter(c, &canvas)) return false;
    per_letter_blocks.push_back(canvas);
  }
  CombineBitmaps(per_letter_blocks, phrase_bitmap);
  return true;
}

bool BDFFont::RenderLetter(char c, CanvasBitMap* letter_bitmap) {
  map<char, BDFChar>::const_iterator iter = chars_.find(c);
  if (iter == chars_.end()) {
    cerr << "Error: Cannot find character in bitmap: " << c << endl;
    return false;
  }
  const BDFChar& bdf_char = iter->second;
  letter_bitmap->width = std::max(bdf_char.device_width.x, bdf_char.bbx.width);
  letter_bitmap->height = bdf_char.bbx.height;
  const vector<uint8_t>& bm = bdf_char.bitmap;
  if (bm.size() != letter_bitmap->height) {
    cerr << "Bitmap size != BBX height for char: " << bdf_char.name;
    return false;
  }
  for (int y = 0; y < letter_bitmap->height; y++) {
    uint8_t b = bm[letter_bitmap->height - 1 - y]; // The bitmaps are specified high y-coord first.
    uint8_t width = bdf_char.bbx.width;
    cerr << "DEBUG: --" << endl;
    cerr << "DEBUG: BBX char width: " << (int)width << endl;
    cerr << std::hex;
    cerr << "DEBUG: Seen bitmap val: 0x" << (int)b << endl;
    cerr << std::dec;
    for (uint8_t x = 0; x < bdf_char.bbx.width; x++) {
      uint8_t val_at_bit = 1 << (7 - x);
      cerr << std::hex;
      cerr << "DEBUG: Looking for bit: 0x" << (int)val_at_bit << endl;
      cerr << std::dec;
      if (b & val_at_bit) {
        // Render a Point @ (x,y) with the bbx offsets.
        int p_x = x + bdf_char.bbx.x_offset;
        int p_y = y + bdf_char.bbx.y_offset;
        cerr << "DEBUG: Adding Point: " << p_x << ", " << p_y << endl;
        letter_bitmap->bitmap.insert(Point(p_x, p_y));
      }
    }
  }
  return true;
}

void BDFFont::CombineBitmaps(const std::vector<CanvasBitMap>& letter_bitmaps,
                             CanvasBitMap* phrase_bitmap) {
  int max_height = 0;
  int running_width = 0;
  for (int i = 0; i < letter_bitmaps.size(); i++) {
    const CanvasBitMap& letter_bitmap = letter_bitmaps[i];
    if (max_height < letter_bitmap.height) {
      max_height = letter_bitmap.height;
    }
    for (BitMap::const_iterator iter = letter_bitmap.bitmap.begin();
         iter != letter_bitmap.bitmap.end();
         ++iter) {
      phrase_bitmap->bitmap.insert(Point(iter->x + running_width, iter->y));
    }
    running_width += letter_bitmap.width;
  }
  cerr << "DEBUG: Running width: " << running_width << endl;
  cerr << "DEBUG: Max height: " << max_height << endl;
  phrase_bitmap->width = running_width;
  phrase_bitmap->height = max_height;
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
  PrintBitmapAsCode(canvas_bitmap);
  RenderBitmap(canvas_bitmap);
  return 0;
}
