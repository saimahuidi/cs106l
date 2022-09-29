/*
 * TODO: complete this file comment.
 */
#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "SimpleGraph.h"

using namespace std;

void Welcome();
void ReadData(vector<Edge>&, vector<Node>&, istream& = cin);
void Mainloop(int, SimpleGraph&);
int GetInteger(string&, istream& = cin);
void GetFileAndTime(string&, int&, istream& = cin);
void UpdateRepulsizeForce(vector<Node>&);
void UpdateAttractiveForce(vector<Node>&, vector<Edge>&);

const double kPi = 3.14159265358979323;
const double Krepel = 0.001;
const double Kattract = 0.001;
// Main method
int main() {
    Welcome();
    SimpleGraph *simpleGraph = new SimpleGraph();
    string fileName;
    int timeLasting;
    GetFileAndTime(fileName, timeLasting);
    ifstream ifs(fileName);
    ReadData(simpleGraph->edges, simpleGraph->nodes, ifs);
    InitGraphVisualizer(*simpleGraph);
    DrawGraph(*simpleGraph);
    Mainloop(timeLasting, *simpleGraph);
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

void GetFileAndTime(string& fileName, int& timeLasting, istream& in) {
    cout << "Please enter the fileName: ";
    getline(in, fileName);
    string promt("Please enter the timeLasting: ");
    timeLasting = GetInteger(promt, in);
}
void ReadData(vector<Edge>& edges, vector<Node>& nodes, istream& in) {
    int num = 0;
    in >> num;
    if (in.fail())
        throw domain_error("There don't exist the num of the nodes");
    for (int i = 0; i < num; i++) {
        Node initNode;
        initNode.x = cos((2 * kPi) / num * i);
        initNode.y = sin((2 * kPi) / num * i);
        nodes.push_back(initNode);
    }
    while (true) {
        int node1, node2;
        in >> node1 >> node2;
        if (in.fail()) {
            break;
        }
        Edge edge;
        edge.start = node1;
        edge.end   = node2;
        edges.push_back(edge);
    }
}

inline void Mainloop(int timeLasting, SimpleGraph& simplegraph) {
    time_t startTime = time(NULL);
    while (difftime(time(NULL), startTime) < timeLasting) {
        UpdateRepulsizeForce(simplegraph.nodes);
        UpdateAttractiveForce(simplegraph.nodes, simplegraph.edges);
        DrawGraph(simplegraph);
//        cout << "nice" << endl;
    }
    cout << "time over!" << endl;
}

int GetInteger(string& promt, istream& in) {
    while (true) {
        cout << promt;
        string line;
        if (!getline(in, line)) throw domain_error("...");

        istringstream iss(line);
        int val; char ch;

        if (iss >> val && !(iss >> ch)) return val;
    }
}

void UpdateRepulsizeForce(vector<Node>& nodes) {
    vector<Node> forces(nodes.size(), {0, 0});
    int size = nodes.size();
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            double x_ = nodes[i].x - nodes[j].x;
            double y_ = nodes[i].y - nodes[j].y;
            double Frepel = Krepel / sqrt(pow(x_, 2) + pow(y_, 2));
            double angle = atan2(y_, x_);
            forces[i].x += Frepel * cos(angle);
            forces[i].y += Frepel * sin(angle);
            forces[j].x -= Frepel * cos(angle);
            forces[j].y -= Frepel * sin(angle);
        }
    }
    for (int i = 0; i < size; i++) {
        nodes[i].x += forces[i].x;
        nodes[i].y += forces[i].y;
    }
}

void UpdateAttractiveForce(vector<Node>& nodes, vector<Edge>& edges) {
    vector<Node> forces(nodes.size(), {0, 0});
    int size = edges.size();
    for (int k = 0; k < size; k++) {
        int i = edges[k].start;
        int j = edges[k].end;
        double x_ = nodes[i].x - nodes[j].x;
        double y_ = nodes[i].y - nodes[j].y;
        double Fattract = Kattract * (pow(x_, 2) + pow(y_, 2));
        double angle = atan2(y_, x_);
        forces[i].x -= Fattract * cos(angle);
        forces[i].y -= Fattract * sin(angle);
        forces[j].x += Fattract * cos(angle);
        forces[j].y += Fattract * sin(angle);
    }
    int nodeNum = nodes.size();
    for (int i = 0; i < nodeNum; i++) {
        nodes[i].x += forces[i].x;
        nodes[i].y += forces[i].y;
    }

}
