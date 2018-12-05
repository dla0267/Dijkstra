#include "galaxy.h"


void Itinerary::print(Fleet& fleet, std::ostream& out) {
    int size = this->destinations.size(); 
    out << fleet.name(this->legs[size-1].id) << "\t"; 
    out << this->origin->name << "\t"; 
    out << this->legs[size-1].departure_time << "\t"; 
    out << this->destinations[size-1]->name << "\t";
    out << this->legs[size-1].arrival_time << endl;
    for (int i = size - 1; i > 0; i--) {
        out << fleet.name(this->legs[i].id) << "\t"; 
        out << this->destinations[i]->name << "\t"; 
        out << this->legs[i - 1].departure_time << "\t"; 
        out << this->destinations[i - 1]->name << "\t";
        out << this->legs[i - 1].arrival_time << endl;
    }
}

void Itinerary::enroute_time() {
    int size = this->legs.size();
    cerr << "Departing time: " << this->legs[size-1].departure_time << endl;
    cerr << "Arrival time: " << this->legs[0].arrival_time << endl;
    int enroute_time = this->legs[0].arrival_time - 
                            this->legs[this->destinations.size() - 1].departure_time;
    cerr << "Enroute time: " << enroute_time << " (" << enroute_time / 24 
            << " days and " << enroute_time % 24 << " hours)" << endl;
    cerr << "Time in Space: " << this->time_in_space << " (" << this->time_in_space / 24 
            << " days and " << this->time_in_space % 24 << " hours)" << endl;
    cerr << std::endl; 
}


void Edge::sort() {
    std::sort(this->departures.begin(), this->departures.end(), Leg::less_than);
}

void Edge::sort(int first, int last) {
	int mid = (last + first) / 2;
    if (Leg::less_than(this->departures[last], this->departures[first])) {
		swap(this->departures[last], this->departures[first]);
	}
	if (Leg::less_than(this->departures[mid], this->departures[first]))
	{
		swap(this->departures[mid], this->departures[first]);
	}
	if (Leg::less_than(this->departures[last], this->departures[mid]))
	{
		swap(this->departures[last], this->departures[mid]);
	}
	Leg pivot = this->departures[mid];
	swap(this->departures[mid], this->departures[last - 1]);
	int indexFromLeft = first + 1;
	int indexFromRight = last - 2;
	bool done = false;
	while (!done)
	{
		while (Leg::less_than(this->departures[indexFromLeft], pivot))
		{
			indexFromLeft++;
		}
        while (Leg::less_than(pivot, this->departures[indexFromRight]))
		// while (this->departures[indexFromRight] > pivot)
		{
			indexFromRight--;
		}
		if (indexFromLeft < indexFromRight)
		{
			swap(this->departures[indexFromLeft], this->departures[indexFromRight]);
			indexFromLeft++;
			indexFromRight--;
		}
		else
		{
			done = true;
		}
	}
	swap(this->departures[indexFromLeft], this->departures[last - 1]);
	sort(first, indexFromLeft - 1);
	sort(indexFromLeft + 1, last);
}


void Edge::swap(Leg& lhs, Leg& rhs) {
    Leg temp = lhs;
    lhs.id = rhs.id;
    lhs.departure_time = rhs.departure_time;
    lhs.arrival_time = rhs.departure_time;
    rhs.id = temp.id;
    rhs.departure_time = temp.departure_time;
    rhs.arrival_time = temp.arrival_time;
}


void Edge::dump(Galaxy* galaxy) {
    cerr << "To destination planet: " << destination->name << "\t";
    cerr << "Enroute time: " << this->travel_time << endl;
    if (this->departures.size() == 0) {
            cerr << "There is no available to time offered" << endl;
            cerr << endl;
    } else {
        cerr << "Available Times:" << endl;
        for (int i = 0; i < this->departures.size(); i++) {
        Leg leg = this->departures[i];
        cerr << "\t By " << galaxy->fleet.name(leg.id) << "\t"
            << "Departure time: " << leg.departure_time << "\t"
            <<  "Arrival time: " << leg.arrival_time << endl;
        }
        cerr << endl;
    }
}


Planet* Planet::search(PriorityQueue<Planet, int (*)(Planet*, Planet*)>& pq) {
    this->best_leg = Leg(-1, 0, 0);
    Planet* current_p = this;
    pq.reduce(current_p);
    //pq.pop();
    //cerr << "first planet to look at::  " << current_p->name << "   " << current_p->arrival_time() << endl;
    while (!pq.empty()) {
        Planet* next_p = pq.pop();
        //cerr << "checking if planets are in order in pq" << endl;
        //cerr << next_p->name << " " << next_p->arrival_time() << endl;
        if (current_p->arrival_time() < next_p->arrival_time()) {
            current_p = next_p;
        }
        next_p->relax_neighbors(pq);
    }
    return current_p;
}

Itinerary* Planet::make_itinerary(Planet* destination) {
    Itinerary* itinerary_ = new Itinerary(this);
    Planet* cur_p = destination;
    Planet* predecessor_ = destination->predecessor;
    while (predecessor_ != nullptr) {
        itinerary_->destinations.push_back(cur_p);
        itinerary_->legs.push_back(cur_p->best_leg);
        cur_p = predecessor_;
        predecessor_ = cur_p->predecessor;
    }
    return itinerary_;
}

void Planet::dump(Galaxy* galaxy) {
    for (int i = 0; i < this->edges.size(); i++) {
            this->edges[i]->dump(galaxy);
    }
}

void Planet::relax_neighbors(PriorityQueue<Planet, int (*)(Planet*, Planet*)>& pq) {
    Time departure_time_ = this->arrival_time() + TRANSFER_TIME;
    //cerr << "origin's arrived at : " << this->best_leg.arrival_time << " + transfer_time " << TRANSFER_TIME << " is " << new_departure_time << endl;
    for (int i = 0; i < this->edges.size(); i++) {
        Edge* cur_edge = edges[i];
        Planet* destination_p = cur_edge->destination;
        cur_edge->sort();
        //cur_edge->sort(0, cur_edge->departures.size()-1);
        Leg best_leg_;
        for (auto leg_ : cur_edge->departures) {
            Time leg_d_time = leg_.departure_time;
            if (departure_time_ <= leg_d_time && Leg::less_than(leg_, best_leg_)) {
                best_leg_ = leg_;
            }            
        }
        if (Leg::less_than(best_leg_, destination_p->best_leg)) {  
            //cerr << "best_leg)'s time " << best_leg_.arrival_time << endl;
            destination_p->predecessor = this;
            destination_p->best_leg = best_leg_;
            pq.reduce(destination_p);
        }
    }
}

void Galaxy::search() {
    for (int i = 0; i < planets.size(); i++) {
        PriorityQueue<Planet, int (*)(Planet*, Planet*)> pq(Planet::compare);
        for (int j = 0; j < planets.size(); j++) {
            pq.push_back(this->planets[j]);
        }  
        // find the furtheset plan from each planet in the galaxy
        Planet* furthest_p = this->planets[i]->search(pq);
        cerr << "last   " << furthest_p->name << endl;
        this->dump_routes(this->planets[i], furthest_p, std::cerr);
        Itinerary* itinerary_ = this->planets[i]->make_itinerary(furthest_p);
        itinerary_->print(this->fleet, std::cout);
        itinerary_->enroute_time();
        this->reset();
    }

}

void Galaxy::dump() {
    cerr << "galaxy dump starts here: printing all the planets" << endl;
    for (int i = 0; i < planets.size(); i++) {
        cerr << "-------" << planets[i]->name << "-------"<< endl;
        planets[i]->dump(this);
    }
    //cerr << "galxy dump ends here" << endl;
}

void Galaxy::dump_routes(Planet* origin, Planet* furthest_p, std::ostream& out) {
    cerr << "--------Printing Itinerary's route--------" << endl;
    vector<Planet*> planets;
    vector<Leg> legs;
    Planet* cur_p = furthest_p;
    Planet* predecessor_ = cur_p->getPredecessor();

    while (predecessor_ != nullptr) {
        planets.push_back(cur_p);
        legs.push_back(cur_p->getLeg());
        cur_p = predecessor_;
        predecessor_ = cur_p->getPredecessor();
    }

    out << "From " << origin->name << endl;
    int count = 0;
    for (int i = planets.size() - 1; i >= 0; i--) {
          out << "\t" << "Departing at\t" << legs[i].departure_time
            << "\tArriving at\t" << legs[i].arrival_time << "\t"
            << "To\t" << planets[i]->name << endl;
    }
    out << endl;
}

