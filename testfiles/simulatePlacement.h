#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using namespace std;
class GEdge;
class GNode;

class TimePartition {
  int start;
  int end;
  int device;

public:
  void setStart(int s) { start = s;}
  void setEnd(int e) {end = e;}
  void setDevice(int d) { device = d;}

  int getStart() { return start;}
  int getEnd() { return end;}
  int getDevice() { return device;}

  bool containsTP(TimePartition tp);
  void printTP();

  //Constructor
  TimePartition();
  TimePartition(int s, int e, int d)
            : start(s),
              end(e),
              device(d){} 
};

class GNode{
  int id;
  string name;
  int weight;
  vector<GEdge> childrenEdges;
  bool visited;
  int device;
  int inDeg;
  int outDeg;

public:
  int getWeight();
  int getID();
  int getDevice();
  string getName();
  vector<GEdge> getChildrenEdges();
  void setVisited(bool v);
  bool getVisited();
  void setWeight(int w);
  void setDevice(int d);
  int getInDeg();
  int getOutDeg();
  void setInDeg(int v);
  void setOutDeg(int v);

  GEdge edgeTo(string targetName);
  void addEdge(GEdge e);

  //Constructor
  GNode(string n, int w, int i);
  GNode();
};

class GEdge{
  int id;
  GNode src;
  GNode dst;
  int weight;
public:
  int getWeight();
  int getID();
  GNode getSrc();
  GNode getDst();
  void setWeight(int w);
  void setSrc(GNode n);
  void setDst(GNode n);

  //Constructor 1
  GEdge(int w, GNode s, GNode d, int i);
  GEdge();
};

class DGraph { 
  vector<GEdge> edges;
  map<string,GNode> nodes;
public:
  vector<GEdge> getEdges() { return edges;}

  vector<GNode> getNodes();
  GNode getNode(string l);
  GNode addNode(string l, int weigth);
  GEdge addEdge(int weigth, string from, string to);
  void addSourceAndSink();
};