#include "galaxy.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;



struct Travel_Times {
    int size = 0;
    vector<string> departure_;
    vector<string> arrival_;
    vector<int> travel_time;
};


class Reader {
public:
  Reader(istream& in, Travel_Times* constraints);
  ~Reader();

  Galaxy* load();

private:
  static const int MIN_LAYOVER_TIME = 4;

  // buliding the graph of galaxy with given planets from input text file.
  void buildGalaxy();
  // Read next leg of ship's route
  bool get_record();
  // Verify that that current leg is a valid continuation of the
  // previous leg or the beginning of the route for another ship.
  bool validate();

  std::istream& in;
  // Data structure holding the travel times between planets.
  Travel_Times* constraints;

  // Input string representing a single leg.
  string current_input_line;

  // Previous leg information for validation.
  Ship_ID previous_ship_id = -1;
  Planet* previous_destination_planet = nullptr;
  int previous_arrival_time = -1;

  // Current leg information
  Ship_ID ship_id = -1;
  Planet* departure_planet = nullptr;
  Time departure_time = 0;
  Planet* destination_planet = nullptr;
  Time arrival_time = 0;

  // Planet name to planet object
  map<string, Planet*> planets;

  // Planet-name pair to edge object
  map<const Planet*, map<const Planet*, Edge*> > edges;

  // Ship name to id.
  map<string, Ship_ID> ships;

  // Route structure under construction.
  Galaxy* galaxy;
};
