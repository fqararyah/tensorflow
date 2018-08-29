#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include "simulatePlacement.h"

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define CHANCE 20     /* Chance of having an Edge.  */


using namespace std;
/*
class DGraph;
class Node;
class Edge;
*/
void exportToDot(DGraph g);
////TimePartition./////
bool TimePartition::containsTP(TimePartition* tp){
  if(tp->getDevice() != this->device || this->device == -1) {
    return false;
  }
  else {
    if((tp->getEnd() < this->getStart()) || (tp->getStart() > this->getEnd())) {
      return true;
    }
    else {
      return false;
    }

  }
}
void TimePartition::printTP() {
    printf("Start: %d,", this->start);
    printf("End: %d, ", this->end);
    printf("Device: %d\n", this->device);
}
///////////////////////////

//////////////////////Node/////////////////////////////
int GNode::getWeight() { return this->weight;}
int GNode::getID() { return this->id;}
int GNode::getDevice(){ return this->device;}
string GNode::getName() { return this->name;}
vector<GEdge *> GNode::getChildrenEdges() { return this->childrenEdges;}
void GNode::setVisited(bool v) { this->visited=v;}
bool GNode::getVisited() { return this->visited;}
void GNode::setWeight(int w) { this->weight=w;}
void GNode::setDevice(int d) { this->device=d;}

//Constructor
GNode::GNode(string n, int w, int i)
      : name(n),
      visited(false),
      weight(w),
      id(i){}

GEdge* GNode::edgeTo(string targetName) {
    GEdge* res;
    for(GEdge* e : this->childrenEdges) {
      if(e->getDst()->getName() == targetName) {
        res = e;
        break;
      }
    }
    return res;
}
void GNode::addEdge(GEdge* e) { this->childrenEdges.push_back(e);}
/////////////////////////////////////////////////////

////////////////////////Edge/////////////////////////
int GEdge::getWeight() { return this->weight;}
int GEdge::getID() { return this->id;}
GNode* GEdge::getSrc() { return this->src;}
GNode* GEdge::getDst() { return this->dst;}
void GEdge::setWeight(int w){ this->weight = w;}
void GEdge::setSrc(GNode* n) { this->src = n;}
void GEdge::setDst(GNode* n) { this->dst = n;}

//Constructor
GEdge::GEdge(int w, GNode* s, GNode* d, int i)
      : weight(w),
      src(s),
      dst(d),
      id(i){}
////////////////////////////////////////////////////////

//////////////////////DGraph/////////////////////////////

GNode* DGraph::addNode(string l, int weight) {
    int id = this->nodes.size();

    GNode* n = new GNode(l, weight, id);
    if(this->nodes.count(l) != 1) {
      this->nodes[l] = n;
    }

    return n;
}
GEdge* DGraph::addEdge(int weigth, string from, string to) { 
    
    GNode* src = this->nodes.at(from);
    GNode* dst = this->nodes.at(to);

    int id = this->edges.size();

    GEdge* edge = new GEdge(weigth, src, dst, id);

    (*src).addEdge(edge);
    this->edges.push_back(edge);

    return edge;
}
////////////////////DGraph///////////////////////////

stack<GNode*> retractionStack;

void exportToDot(DGraph g) {
  printf("digraph G{\n");

  //Traverse the edges
  for(GEdge* e: g.getEdges()) {
    printf("%s->%s;\n", e->getSrc()->getName().c_str(), e->getDst()->getName().c_str());
  }
  printf("}");
}

DGraph generateRandomDAG(int chance, int maxRanks, int minRanks, int minPerRank, int maxPerRank) {
  
  int i, j, k, nodes = 0;
  srand (time (NULL));
  DGraph g;
  //map<>
  int ranks = minRanks
              + (rand () % (maxRanks - minRanks + 1));
  for (i = 0; i < ranks; i++) {
    // New nodes of 'higher' rank than all nodes generated till now.  
    int new_nodes = minPerRank
                    + (rand () % (maxPerRank - minPerRank + 1));

    // Edges from old nodes ('nodes') to new ones ('new_nodes').  
    for(j = 0; j < nodes; j++) {
      GNode* src = g.addNode("n"+to_string(j), 10);
      for(k = 0; k < new_nodes; k++) {
        GNode* dst = g.addNode("n"+to_string(k+nodes), 5);
        if( (rand () % 100) < chance)
          g.addEdge(5, src->getName(), dst->getName());

            //printf ("  %d -> %d;\n", j, k + nodes); /* An Edge*/.  
      }
    }
    nodes += new_nodes; /* Accumulate into old node set*/  
  }
  return g;
}
/*
Node* findPostDom(Node* current, vector<Node*> path) {

}*/

int atomicCost(vector<GNode*> leftPath, vector<GNode*> rightPath, int device ) {
 
  int rightCost = 0;

  vector<TimePartition*> timeLine;
  //int timeLineEnd = 0;
  //Create bins for each device
  //map<int, vector<Node*>> devQueues;

  //Node* parent = leftPath.at(0);
  //Node* current = rightPath.at(0);
  //Node* postDom = leftPath.at(leftPath.size()-1);

  bool done = false;
  int tempStartLeft = 0;
  int tempEndLeft = 0;
  //Create the timeline
  for(int i=1; i < leftPath.size()-1; i++) {
    

    GNode* cur = leftPath.at(i);
    GNode* prev = leftPath.at(i-1);

    if(prev->getDevice() != cur->getDevice()) {
      
      tempEndLeft += prev->edgeTo(cur->getName())->getWeight();
      TimePartition* comTP = new TimePartition(tempStartLeft, tempEndLeft, -1);
      timeLine.push_back(comTP);
      tempStartLeft = tempEndLeft; 
    }
    tempEndLeft += cur->getWeight();
    TimePartition* tp = new TimePartition(tempStartLeft, tempEndLeft, cur->getDevice());

    timeLine.push_back(tp);
  }
  int tempStartRight = 0;
  int tempEndRight = 0;
  TimePartition* tprc; //communication cost
  //If we place the node on its parent's device -- no communication cost
  if(rightPath.at(0)->getDevice() != leftPath.at(0)->getDevice()) {

    tempStartRight += leftPath.at(0)->edgeTo(rightPath.at(0)->getName())->getWeight();
  }
  for(GNode* n : rightPath) {
    tempEndRight = tempStartRight + n->getWeight();
    TimePartition* temptp = new TimePartition(tempStartRight, tempEndRight, device);
    //Scan the left path timeline to estimate idleness cost
    for(TimePartition* t : timeLine) {
      if(!t->containsTP(temptp)) {
        tempEndRight += n->getWeight();
        break;
      }
      else {
        temptp->setStart(t->getEnd());
        temptp->setEnd(temptp->getStart() +n->getWeight());
        tempEndRight = temptp->getEnd();
      }
    }
  }
  if(rightPath.back()->getDevice() != leftPath.back()->getDevice()) { 
    tempEndRight += rightPath.back()->edgeTo(leftPath.back()->getName())->getWeight();
  }
  if(timeLine.back()->getEnd() >= tempEndRight ) {
    return timeLine.back()->getEnd(); // left bracnh's timeline dominates the execution time
  }
  else {
    return tempEndRight;
  }
}
/*
int assignDevice(Node* current, Node* parent) {
  vector<Node*> path;
  path.push_back(parent);

  while(retractionStack.top()->getChildrenEdges.size() < 2) {
    path.push_back(retractionStack.pop());
  }
  path.push_back(retractionStack.top());

  Node* postDominator = findPostDom(current, path);
  Node* sibling = path.at(1);

  return min(cost(current))
}
*/
int main (void)
{
  /*Test for atomicCost function
  vector<GNode*> leftPath;
  leftPath.push_back(new GNode("n1", 3, 1));
  leftPath.push_back(new GNode("n2", 5, 2));
  leftPath.push_back(new GNode("n3", 8, 3));
  leftPath.push_back(new GNode("n4", 10, 4));
  leftPath.push_back(new GNode("n5", 12, 5));

  GEdge* e = new GEdge(10, leftPath.at(0), leftPath.at(1), 1);
  leftPath.at(0)->addEdge(e);
  leftPath.at(1)->addEdge(new GEdge(2, leftPath.at(1), leftPath.at(2), 2));
  leftPath.at(2)->addEdge(new GEdge(1, leftPath.at(2), leftPath.at(3), 3));
  leftPath.at(3)->addEdge(new GEdge(3, leftPath.at(3), leftPath.at(4), 4));

  leftPath.at(0)->setDevice(0);
  leftPath.at(1)->setDevice(1);
  leftPath.at(2)->setDevice(2);
  leftPath.at(3)->setDevice(3);
  leftPath.at(4)->setDevice(0);

  
  atomicCost(leftPath);

  /*
  map<int,vector<Node*>> testQueues;

  //Test for 
  DGraph g = generateRandomDAG(CHANCE, MAX_RANKS, MIN_RANKS, MIN_PER_RANK, MAX_PER_RANK );
  //Test the graph implementation
  for(Node* n: g.getNodes()) {
    if(n->getID()%2 == 0) {
      n->setDevice(0);
    }
    else {
      n->setDevice(1);
    }
  }
  for(Node* n: g.getNodes()) {
    testQueues[n->getDevice()].push_back(n);
  }

  for(auto& kv: testQueues) {
    printf("Queue for device %d:\n\n", kv.first);
    for(Node*n : kv.second) {
      printf("Node: %s, ", n->getName().c_str());
    }
    printf("\n");
  }

  exportToDot(g);


  return 0;
  /*
  int i, j, k,nodes = 0;
  srand (time (NULL));

  int ranks = MIN_RANKS
              + (rand () % (MAX_RANKS - MIN_RANKS + 1));

  printf ("digraph {\n");
  
  for (i = 0; i < ranks; i++)
    {
      /* New nodes of 'higher' rank than all nodes generated till now.  
      int new_nodes = MIN_PER_RANK
                      + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

      /* Edges from old nodes ('nodes') to new ones ('new_nodes').  
      for (j = 0; j < nodes; j++)
        for (k = 0; k < new_nodes; k++)
          if ( (rand () % 100) < PERCENT)
            printf ("  %d -> %d;\n", j, k + nodes); /* An Edge.  

      nodes += new_nodes; /* Accumulate into old node set.  
    }
  printf ("}\n");
  return 0;
  */
}
