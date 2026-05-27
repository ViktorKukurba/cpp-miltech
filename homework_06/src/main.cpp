#include <iostream>
#include <fstream>
#include <cstring>

#include "ballistics.hpp"

const char* INPUT_FILE = "input.txt";
const char* OUTPUT_FILE = "output.txt";

int main()
{
  try {
    Data data = getData(INPUT_FILE);

    double h = getHorizontalFlightDistance(data);
    cout << "Horizontal Flight Distance: " << h << endl;

    double D = getTargetDistance(data);
    ofstream outFile(OUTPUT_FILE);

    if (h + data.accelerationPath > D) {
      setIntermediatePoint(data, h, D);
      outFile << data.xd << ' ' << data.yd << ' ';

      D = getTargetDistance(data);
    }

    Point p = getPoint(data, h, D);
    cout << "Coordinates x: " << p.x << " y: " << p.y << endl;

    outFile << (p.x) << ' ' << p.y;
  }
  catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << endl;
  }

  return 0;
}
