// Copyright 2015 Red Blob Games <redblobgames@gmail.com>
// License: Apache v2.0 <http://www.apache.org/licenses/LICENSE-2.0.html>

#include "traverse.h"
#include "traverse-json.h"
#include <iostream>

struct Point {
  int x, y;
};
TRAVERSE_STRUCT(Point, FIELD(x) FIELD(y))

struct LineSegment {
  Point a, b;
};
TRAVERSE_STRUCT(LineSegment, FIELD(a) FIELD(b))

struct Polygon {
  std::string name;
  std::vector<Point> points;
};
TRAVERSE_STRUCT(Polygon, FIELD(name) FIELD(points))

int main() {
  traverse::CoutWriter writer;
  Point p = {3, 5};
  LineSegment s = {{1, 7}, {13, 19}};
  Polygon polygon = {"UFO", {{3, 5}, {4, 6}, {5, 7}}};
  
  std::cout << "original data: " << std::endl;
  visit(writer, polygon);
  std::cout << std::endl;

  std::cout << "serialized to bytes: " << std::endl;
  traverse::BinarySerialize writer2;
  visit(writer2, polygon);
  std::string msg = writer2.out.str();
  for (int i = 0; i < msg.size(); i++) {
    std::cout << int(msg[i]) << ' ';
  }
  std::cout << std::endl;

  std::cout << "read back from bytes: " << std::endl;
  traverse::BinaryDeserialize reader(msg);
  Polygon polygon2;
  visit(reader, polygon2);
  visit(writer, polygon2);
  std::cout << std::endl;

  std::cout << "corrupting data: " << std::endl;
  msg = writer2.out.str();
  msg[3] = 0x7f;
  traverse::BinaryDeserialize reader2(msg);
  Polygon polygon3;
  visit(reader2, polygon3);
  visit(writer, polygon3);
  std::cout << std::endl;

  std::cout << "write polygon to json:" << std::endl;
  picojson::value json1;
  traverse::JsonWriter jsonwriter{json1};
  visit(jsonwriter, polygon);
  std::cout << json1.serialize() << std::endl;

  std::cout << "parse json into picojson object:" << std::endl;
  picojson::value json2;
  auto err = picojson::parse(json2, "{\"points\":[{\"UNUSED\":0,\"x\":3,\"y\":5},{\"y\":6,\"x\":4},{\"y\":7},{\"x\":\"WRONGTYPE\"}]}");
  if (!err.empty()) { std::cout << "JSON error " << err << std::endl; }
  else { std::cout << "(success)" << std::endl; }
  
  std::cout << "read back from json into polygon:" << std::endl;
  traverse::JsonReader jsonreader{json2};
  Polygon polygon4;
  visit(jsonreader, polygon4);
  visit(writer, polygon4);
  std::cout << std::endl;
}
