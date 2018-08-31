#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include "simulatePlacement.h"

#define MIN_PER_RANK 5 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 5 /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define CHANCE 15     /* Chance of having an Edge.  */


using namespace std;

/*
class DGraph;
class Node;
class Edge;
*/

void exportToDot(DGraph g);
////TimePartition./////
bool TimePartition::containsTP(TimePartition tp){
  if(tp.getDevice() != this->device || this->device == -1) {
    return false;
  }
  else {
    if((tp.getEnd() < this->getStart()) || (tp.getStart() > this->getEnd())) {
      return false;
    }
    else {
      return true;
    }

  }
}
void TimePartition::printTP() {
    printf("Start: %d,", this->start);
    printf("End: %d, ", this->end);
    printf("Device: %d\n", this->device);
}
TimePartition::TimePartition(){}
///////////////////////////

//////////////////////Node/////////////////////////////
int GNode::getWeight() { return this->weight;}
int GNode::getID() { return this->id;}
int GNode::getDevice(){ return this->device;}
string GNode::getName() { return this->name;}
vector<GEdge> GNode::getChildrenEdges() { return this->childrenEdges;}
void GNode::setVisited(bool v) { this->visited=v;}
bool GNode::getVisited() { return this->visited;}
void GNode::setWeight(int w) { this->weight=w;}
void GNode::setDevice(int d) { this->device=d;}
int GNode::getInDeg(){ return this->inDeg;}
int GNode::getOutDeg() { return this->outDeg;}
void GNode::setInDeg(int val) { this->inDeg = val;}
void GNode::setOutDeg(int val) { this->outDeg = val;}
void GNode::setName(string n) { this->name = n;}

//Constructor
GNode::GNode(string n, int w, int i)
      : name(n),
      visited(false),
      weight(w),
      inDeg(0),
      outDeg(0),
      id(i),
      childrenEdges(vector<GEdge>()){}

GNode::GNode()
      : name(""),
       weight(0),
       visited(false),
       device(0),
       inDeg(0),
       outDeg(0){}

GEdge GNode::edgeTo(string targetName) {
    GEdge res;
    for(GEdge e : this->childrenEdges) {
      if(e.getDst().getName() == targetName) {
        res = e;
        break;
      }
    }
    return res;
}
void GNode::addEdge(GEdge e) { /*printf("adding ref\n");*/this->childrenEdges.push_back(e);}
/////////////////////////////////////////////////////

////////////////////////Edge/////////////////////////
int GEdge::getWeight() { return this->weight;}
int GEdge::getID() { return this->id;}
GNode GEdge::getSrc() { return this->src;}
GNode GEdge::getDst() { return this->dst;}
void GEdge::setWeight(int w){ this->weight = w;}
void GEdge::setSrc(GNode n) { this->src = n;}
void GEdge::setDst(GNode n) { this->dst = n;}

//Constructor
GEdge::GEdge()
      :weight(-1),
      id(-1),
      src(GNode()),
      dst(GNode()){}
GEdge::GEdge(int w, GNode s, GNode d, int i)
      : weight(w),
      src(s),
      dst(d),
      id(i){}
////////////////////////////////////////////////////////

//////////////////////DGraph/////////////////////////////

GNode DGraph::addNode(string l, int weight) {
    int id = this->nodes.size();

    GNode n(l, weight, id);
    if(this->nodes.count(l) != 1) {
      this->nodes[l] = n;
    }

    return n;
}
GEdge DGraph::addEdge(int weight, string from, string to) { 
    
    GNode src = this->nodes[from];
    GNode dst = this->nodes[to];
    printf("==================\n");
    printf("Src: %s\n",src.getName().c_str());
    printf("Dst: %s\n",dst.getName().c_str());
    printf("==================\n");
    int id = this->edges.size();

    GEdge edge(weight, src, dst, id);
    GNode* pSrc = &this->nodes[from];
    GNode* pDst = &this->nodes[to];

    pSrc->addEdge(edge);
    pSrc->setOutDeg(this->nodes.at(from).getOutDeg() + 1);
    pDst->setInDeg(this->nodes.at(to).getInDeg() + 1);

    this->edges.push_back(edge);

    
    return edge;
}
vector<GNode> DGraph::getNodes() {
  vector<GNode> resList;
  for(auto p : this->nodes) {
    resList.push_back(p.second);
  }
  return resList;
}
GNode DGraph::getNode(string l) {
  printf("Entered getNode\n");
  //GNode tar = this->nodes.find(l)->second;
  //printf("%s153\n", tar.getName().c_str());
  //GNode end = this->nodes.end()->second;
  
  if(nodes.count(l) >= 1) {
    return this->nodes.find(l)->second;
  }
  else {
    return GNode("INV", -1, -1);
  }
}
void DGraph::addSourceAndSink() {
  GNode source = this->addNode("SOURCE_", 0);
  GNode sink = this->addNode("SINK_", 0);
  source.setDevice(0);
  sink.setDevice(0);
  for(GNode n : this->getNodes()) {
    if(n.getInDeg() == 0 ){
      if(n.getName() != "SOURCE_" && n.getName() != "SINK_") {
        this->addEdge(0, "SOURCE_", n.getName());
      }   
    }
    if(n.getOutDeg() == 0) {
      if(n.getName() != "SOURCE_" && n.getName() != "SINK_"){
        this->addEdge(0, n.getName(), "SINK_");
      }
    }
  }
}
////////////////////DGraph///////////////////////////

stack<GNode> retractionStack;

void exportToDot(DGraph g) {
  ofstream dotfile;
  dotfile.open("sim.dot");

  dotfile << "digraph G{\n";

  //Traverse the nodes
  for(GNode n : g.getNodes()) {
    dotfile << n.getName().c_str()<< " [weight="<<n.getWeight()<<"];\n";
  }
  //Traverse the edges
  for(GEdge e: g.getEdges()) {
    dotfile << e.getSrc().getName().c_str() <<"->"<<e.getDst().getName().c_str()<<";\n";
    //printf("%s->%s;\n", e.getSrc().getName().c_str(), e.getDst().getName().c_str());
  }
  dotfile << "}\n";

  dotfile.close();
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
      GNode src = g.addNode("n"+to_string(j), 2 + (rand() % static_cast<int>(121)));
      for(k = 0; k < new_nodes; k++) {
        GNode dst = g.addNode("n"+to_string(k+nodes), 2 + (rand() % static_cast<int>(121)));
        if( (rand () % 100) < chance)
          
          g.addEdge(2 + (rand() % static_cast<int>(121)), src.getName(), dst.getName());

            //printf ("  %d -> %d;\n", j, k + nodes); /* An Edge*/.  
      }
    }
    nodes += new_nodes; /* Accumulate into old node set*/  
  }
  return g;
}

vector<GNode> findPostDom(GNode current, vector<GNode> path) {
  vector<GNode> res;
  bool found =false;
  while(!found) {
    for(GNode n : path) {
      if(current.getChildrenEdges().at(0).getDst().getName() == n.getName()) {
        found = true;
        res.push_back(n);
        break;
      }
      else {
        res.push_back(current);
      }
    }
    current = current.getChildrenEdges().at(0).getDst();
  }
  return res;
}

int atomicCost(vector<GNode> leftPath, vector<GNode> rightPath, int device ) {
 
  vector<TimePartition> timeLine;

  bool done = false;
  int tempStartLeft = 0;
  int tempEndLeft = 0;
  //Create the timeline
  for(int i=1; i < leftPath.size()-1; i++) {
    

    GNode cur = leftPath.at(i);
    GNode prev = leftPath.at(i-1);

    if(prev.getDevice() != cur.getDevice()) {
      
      tempEndLeft += prev.edgeTo(cur.getName()).getWeight();
      TimePartition comTP(tempStartLeft, tempEndLeft, -1);
      timeLine.push_back(comTP);
      tempStartLeft = tempEndLeft; 
      //delete(comTP);
    }
    tempEndLeft += cur.getWeight();
    TimePartition tp(tempStartLeft, tempEndLeft, cur.getDevice());

    timeLine.push_back(tp);
    //delete(tp);
  }
  int tempStartRight = 0;
  int tempEndRight = 0;
  TimePartition tprc; //communication cost
  //If we place the node on its parent's device -- no communication cost
  if(rightPath.at(0).getDevice() != leftPath.at(0).getDevice()) {

    tempStartRight += leftPath.at(0).edgeTo(rightPath.at(0).getName()).getWeight();
  }
  for(GNode n : rightPath) {
    tempEndRight = tempStartRight + n.getWeight();
    TimePartition temptp(tempStartRight, tempEndRight, device);
    //Scan the left path timeline to estimate idleness cost
    for(TimePartition t : timeLine) {
      if(!t.containsTP(temptp)) {
        tempEndRight += n.getWeight();
        break;
      }
      else {
        temptp.setStart(t.getEnd());
        temptp.setEnd(temptp.getStart() +n.getWeight());
        tempEndRight = temptp.getEnd();
      }
    }
    //delete(temptp);
  }
  if(rightPath.back().getDevice() != leftPath.back().getDevice()) { 
    tempEndRight += rightPath.back().edgeTo(leftPath.back().getName()).getWeight();
  }
  if(timeLine.back().getEnd() >= tempEndRight ) {
    return timeLine.back().getEnd(); // left bracnh's timeline dominates the execution time
  }
  else {
    return tempEndRight;
  }
}

bool isPostDom(GNode pdCand, GNode current) {
  bool isFound = false;

  if(current.getChildrenEdges().empty()) {
    printf("%s empty\n", current.getName().c_str());
  }
  while(!isFound) {
    if(current.getName() == pdCand.getName()){
      isFound = true;
    }
    else {
      current = current.getChildrenEdges().at(0).getDst();
    }
  }
  return isFound;
}
vector<GNode> getRightPath(GNode cur, GNode pd) {
  bool isPD = false;
  vector<GNode> path;
  while(!isPD) {
    if(cur.getName() == pd.getName()) {
      isPD = true;
    }
    else { 
      path.push_back(cur);
      cur = cur.getChildrenEdges().at(0).getDst();
    }
  }
  return path;
}
int assignDevice(GNode current, GNode parent) {
  vector<GNode> leftPath;
  vector<GNode> rightPath;

  //Extract left path
  leftPath.push_back(parent);
  retractionStack.pop(); //pop parent
  /*
  while(retractionStack.top()->getChildrenEdges().size() < 2) {
    leftPath.push_back(retractionStack.top());
    retractionStack.pop();
  }
  */
  bool pdFound = false;
  while(!pdFound) {
    GNode pdCand = retractionStack.top();
    pdFound = isPostDom(pdCand, current);
    if(!pdFound) {
      retractionStack.pop();
      leftPath.push_back(pdCand);
    }
  }
  //assert(pdFound);
  GNode postDominator = retractionStack.top();
  leftPath.push_back(postDominator);


  //Prepare the right path
  rightPath = getRightPath(current, postDominator);
  
  //GNode* parent = leftPath.at(0);
  GNode sibling = leftPath.at(1);


  //calc cost for all 4 scenarios
  set<int> avlDevices;

  avlDevices.insert(parent.getDevice());
  avlDevices.insert(sibling.getDevice());
  avlDevices.insert(postDominator.getDevice());

  /*
  int costParent = atomicCost(leftPath,rightPath,parent->getDevice());
  int costSibling = atomicCost(leftPath,rightPath,sibling->getDevice());
  int costPD = atomicCost(leftPath, rightPath, postDominator->getDevice());
  */
  int minCost = 0;
  int minDev = 0;
  for(auto d : avlDevices) {
    int tempCost = atomicCost(leftPath, rightPath, d);
    if(tempCost < minCost){
      minCost = tempCost;
      minDev = d;
    }
  }

  return minDev;
}

bool isImmmediateChild(GNode parent, GNode current) {
  if(parent.getChildrenEdges().at(0).getDst().getName() != current.getName()){
    return false;
  }
  else{
    return true;
  }
}

void customDFS(GNode parent, GNode current, bool turnPoint) {
  printf("Entered customDFS\n");
  //fflush(stdout);
  //DEBUG RECURSIVE CALL
  printf("Parent: %s \n", parent.getName().c_str());
  printf("Current: %s \n", current.getName().c_str());
  if(!turnPoint) {
    printf("Entered !turnPoint\n");
    current.setDevice(parent.getDevice());
  }
  else {
    printf("Entered turnPoint\n");
    current.setDevice(assignDevice(current, parent));
  }
  //printf()
  for(GEdge e : current.getChildrenEdges()) {
    printf("%s, ", e.getDst().getName().c_str());
  }
  printf("\n");
  for(GEdge e : current.getChildrenEdges()) {
    if(!e.getDst().getVisited()) {
      customDFS(current, e.getDst(), !isImmmediateChild(parent, current));
      retractionStack.push(current);
    }
  }
  current.setVisited(true);
}

void executeDFS(DGraph g) {
  printf("Before getNode for root\n");
  GNode root = g.getNode("SOURCE_");
  printf("%s428\n", root.getName().c_str());
  if(root.getChildrenEdges().empty()) {
    printf("True\n");
  }
  printf("Before getNode for root\n");


  if(root.getID() != -1) {
    GNode virParent = GNode();
    virParent.setName("ROOT_");
    GEdge rootToSrc = GEdge(0, virParent, root, -2);
    virParent.addEdge(rootToSrc);
    printf("Before customDFS\n");
    customDFS(virParent, root, false);
  }
  else{
    printf("Error: Graph is invalid");
  }
  printf("Exiting executeDFS\n");
}
int main ()
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

  */
  //map<int,vector<Node*>> testQueues;

  //Test for 
  /*
  GNode* testNode = new GNode("n1", 3, 1);
  if(testNode->getName() != "SOURCE_"){
    printf("It should work\n");
  }
  else {
    printf("It does not work\n");
  }
  */
  
  DGraph g = generateRandomDAG(CHANCE, MAX_RANKS, MIN_RANKS, MIN_PER_RANK, MAX_PER_RANK );
 
  g.addSourceAndSink();

  GNode testroot = g.getNode("SOURCE_");
  if(testroot.getChildrenEdges().empty()) {
    printf("True486\n");
  }
  else {
    printf("It works: %s\n",testroot.getChildrenEdges().at(0).getDst().getName().c_str());
  }
  exportToDot(g);
  //Test the graph implementation
  printf("Before executeDFS\n");
  executeDFS(g);
  //exportToDot(g);


  return 0;
 
  //set<int> avlDevices;

  //avlDevices.insert(1);
  //avlDevices.insert(2);
  //avlDevices.insert(3);

  //for(auto d : avlDevices) {
    //printf("%d ", d);
  //}
  
}
