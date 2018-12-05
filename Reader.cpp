
#include "Reader.h"


Reader::Reader(std::istream& in, Travel_Times* constraints)
        : in(in), constraints(constraints), galaxy(nullptr), 
                planets(), edges() {
    }


Reader::~Reader() {}


void Reader::buildGalaxy() {
    this->galaxy = new Galaxy();
    for (int i = 0; i < constraints->size; i++) {
        string departure = constraints->departure_[i];
        string arrival = constraints->arrival_[i];
        int travel_time = constraints->travel_time[i];

        Planet* departure_p;
        Planet* arrival_p;
        
        if (planets.count(departure) == 0) {
            cerr << "there is no such planet now, so let's add it   " << departure << endl;
            departure_p = new Planet(departure);
            this->planets[departure] =  departure_p;
            this->galaxy->add(departure_p);
        } else {
            departure_p = this->planets[departure];
        }

        if (planets.count(arrival) == 0) {
            cerr << "there is no such planet now, so let's add it   " << arrival << endl;
            arrival_p = new Planet(arrival);
            this->planets[arrival] = arrival_p;
            this->galaxy->add(arrival_p);
        } else {
            arrival_p = this->planets[arrival];
        }

        Edge* edge1 = new Edge(departure_p, travel_time);
        Edge* edge2 = new Edge(arrival_p, travel_time);
        departure_p->add(edge2);
        arrival_p->add(edge1);
        this->edges[departure_p][arrival_p] = edge2;
        this->edges[arrival_p][departure_p] = edge1;
    }
    cerr << "Total number of planets: " << this->galaxy->planets.size() << endl;
    cerr << "galaxy has been built successfully" << endl;
}

Galaxy* Reader::load() {
    this->buildGalaxy();
    // cerr << "before assert" << endl;
    assert(galaxy);  
    // cerr << "assert has no problem, start get_record" << endl;
    this->get_record(); 
    // cerr << "after get_record" << endl;
    return this->galaxy;
}

bool Reader::get_record() {
    cerr << endl;
    cerr << "Reading Route Structure " << endl;
    while (getline(in, current_input_line)) {
        //cerr << current_input_line << endl;
        
        istringstream iss(current_input_line);

        string ship_name;
        if (!getline(iss, ship_name, '\t')) {
            cerr << "Route reader: found an error during reading ship_name" << endl;
            exit(EXIT_FAILURE); 
        }
        cerr << ship_name << "\t";

        string departure_p;
        if (!getline(iss, departure_p, '\t')) {
            cerr << "Route reader: found an error during reading departure" << endl;
            exit(EXIT_FAILURE); 
        }
        cerr << departure_p << "\t";

        string departureTime;
        if (!getline(iss, departureTime, '\t')) {
            cerr << "Route reader: found an error during reading departure time" << endl;
            exit(EXIT_FAILURE); 
        }

        istringstream iss2(departureTime);
        iss2 >> this->departure_time;
        cerr << this->departure_time << "\t";
        
        string destination_p;
        if (!getline(iss, destination_p, '\t')) {
            cerr << "Route reader: found an error during reading destination" << endl;
            exit(EXIT_FAILURE); 
        }
        cerr << destination_p << "\t";

        string arrivalTime;
        if (!getline(iss, arrivalTime, '\t')) {
            cerr << "Route reader: found an error during reading arrival time" << endl;
            exit(EXIT_FAILURE); 
        }
        istringstream iss3(arrivalTime);
        iss3 >> this->arrival_time;
        cerr << this->arrival_time << endl;

        if (this->ships.count(ship_name) == 0) {
            this->ship_id = this->galaxy->fleet.add(ship_name);
            this->ships[ship_name] = this->ship_id;
        } else {
            this->validate();
        }
         
        this->departure_planet = planets[departure_p];
        this->destination_planet = planets[destination_p];

        Leg leg(this->ship_id, this->departure_time, this->arrival_time);
//        cerr << "before adding leg" << endl;
//        cerr << ship_name << " " << this->ships[ship_name] << ship_id <<  endl;
//        cerr << leg.departure_time << " " << leg.arrival_time << endl;
        this->edges[this->departure_planet][this->destination_planet]->add(leg);
//        cerr << "adding leg has not problems" << endl;
        
        this->previous_ship_id = this->ship_id;
        this->previous_destination_planet = this->departure_planet;
        this->previous_arrival_time = this->arrival_time;
    }      

    cerr << "get record has no problem and is validated." << endl;
    cerr << endl;
}

bool Reader::validate() {
    if (this->ship_id == this->previous_ship_id) {
        //cerr << this->ship_id << " "  << this->previous_ship_id << endl;
        if (this->previous_destination_planet != this->departure_planet ||
            (this->previous_arrival_time + MIN_LAYOVER_TIME) > this->departure_time) {
                // cerr << this->previous_destination_planet << endl;
                // cerr << this->departure_planet << endl;
                // cerr << this->previous_arrival_time << endl;
                // cerr << this->departure_time << endl;
                cerr << "Validation Failed for ship ID " << this->ship_id << endl;
                exit(EXIT_FAILURE);
        }
    }
    return true;
}

