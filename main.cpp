#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "galaxy.h"
#include <vector>
#include "Reader.h"

using namespace std;

typedef int Time;
void read_conduits(ifstream& in, Travel_Times* constraints);

void dump(Travel_Times* ct);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    char* conduits = argv[1];
    char* routes = argv[2];

    ifstream in;
    in.open(conduits);
    if (!in) {
        cerr << "Unable to open file conduits.txt";
        return EXIT_FAILURE;
    } 

    Travel_Times* constraints = new Travel_Times();
    read_conduits(in, constraints);
    in.close();
    
    dump(constraints);

    in.open(routes);
    if (!in) {
        cerr << "unable to open file route structure.txt" << endl;
        return EXIT_FAILURE;
    }
    Reader reader(in, constraints);
    //cerr << "before loading" << endl;
    Galaxy* gal = reader.load();
    //cerr << "after loading" << endl;
    //cerr << endl;
    gal->dump();
    gal->search();
    in.close();

    return EXIT_SUCCESS;
};


void read_conduits(ifstream& in, Travel_Times* constraints) {
    string line;
    while (getline(in, line)) {
        constraints->size++;
        istringstream iss(line);
        string token;
        unsigned int timeToken;

        getline(iss, token, '\t');
        constraints->departure_.push_back(token);
        cerr << token << " ";

        getline(iss, token, '\t');
        constraints->arrival_.push_back(token);
        cerr << token << " ";

        iss >> timeToken;
        constraints->travel_time.push_back(timeToken);
        cerr << timeToken << endl;
    }
}

void dump(Travel_Times* constraints) {
    cerr << "Verifying if conduits.txt is read properly or not" << endl;
    cerr << endl;
    for (int i = 0; i < constraints->size; i++) {
        cerr << constraints->departure_[i];
        cerr << "\t " << constraints->arrival_[i];
        cerr << "\t " << constraints->travel_time[i] << endl;;
    }
    cerr << endl;
}