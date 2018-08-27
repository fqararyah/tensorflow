#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define CHANCE 20     /* Chance of having an Edge.  */


using namespace std;

class DGraph;
class Node;
class Edge;

void exportToDot(DGraph g);

class Node{
  int id;
  string name;
  int weight;
  vector<Edge *> childrenEdges;
  bool visited;
  int device;

public:
  int getWeight() { return weight;}
  int getID() { return id;}
  int getDevice(){ return device;}
  string getName() { return name;}
  vector<Edge *> getChildrenEdges() { return childrenEdges;}
  void setVisited(bool v) { visited=v;}
  bool getVisited() { return visited;}
  void setWeight(int w) { weight=w;}
  void setDevice(int d) { device=d;}

  void addEdge(Edge* e) { childrenEdges.push_back(e); }
  //void removeEdge(Edge* e) { childrenEdges.erase(remove(childrenEdges.begin(), childrenEdges.end(), e), childrenEdges.end());}

  //Constructor
  Node(string n, int w, int i)
      : name(n),
      visited(false),
      weight(w),
      id(i){}
};

class Edge{
  int id;
  Node* src;
  Node* dst;
  int weight;
public:
  int getWeight() { return weight;}
  int getID() { return id;}
  Node* getSrc() { return src;}
  Node* getDst() { return dst;}
  void setWeight(int w){ weight = w;}
  void setSrc(Node* n) { src = n;}
  void setDst(Node* n) { dst = n;}

  //Constructor 1
  Edge(int w, Node* s, Node* d, int i)
      : weight(w),
      src(s),
      dst(d),
      id(i){}
  Edge() {}
};

class DGraph { 
  vector<Edge*> edges;
  vector<Node*> nodes;
public:
  vector<Edge*> getEdges() { return edges;}
  vector<Node*> getNodes() { return nodes;}

  Node* addNode(string l, int weight) {
    int id = nodes.size();

    Node* n = new Node(l, weight, id);
    nodes.push_back(n);

    return n;
  }
  Edge* addEdge(int weigth, int from, int to) { 
    if((from >=0 && to >=0) && (from < nodes.size() && to < nodes.size())) {
      Node* src = nodes.at(from);
      Node* dst = nodes.at(to);

      int id = edges.size();

      Edge* edge = new Edge(weigth, src, dst, id);

      (*src).addEdge(edge);
      edges.push_back(edge);

      return edge;

    }
    else {
      return new Edge();
    }
  }
};

void exportToDot(DGraph g) {
  printf("digraph G{\n");

  //Traverse the edges
  for(Edge* e: g.getEdges()) {
    printf("%s->%s;\n", e->getSrc()->getName().c_str(), e->getDst()->getName().c_str());
  }
  printf("}");
}

DGraph generateRandomDAG(int chance, int maxRanks, int minRanks, int minPerRank, int maxPerRank) {
  
  int i, j, k, nodes = 0;
  srand (time (NULL));
  DGraph g;
  int ranks = minRanks
              + (rand () % (maxRanks - minRanks + 1));
  for (i = 0; i < ranks; i++) {
    // New nodes of 'higher' rank than all nodes generated till now.  
    int new_nodes = minPerRank
                    + (rand () % (maxPerRank - minPerRank + 1));

    // Edges from old nodes ('nodes') to new ones ('new_nodes').  
    for(j = 0; j < nodes; j++) {
      Node* src = g.addNode("n"+to_string(j), 10);
      for(k = 0; k < new_nodes; k++) {
        Node* dst = g.addNode("n"+to_string(k+nodes), 5);
        if( (rand () % 100) < chance)
          g.addEdge(5, src->getID(), dst->getID());

            //printf ("  %d -> %d;\n", j, k + nodes); /* An Edge*/.  
      }
    }
    nodes += new_nodes; /* Accumulate into old node set*/  
  }
  return g;
}

Node* findPostDom(Node* current, vector<Node*> path) {

}

int atomicCost(vector<Node*> leftPath, vector<Node*> rightPath, int device ) {
  int leftCost = 0;
  int rightCost = 0;
  int maxCost = 0;

  int timeLineStart = 0;
  
  Node* parent = leftPath.at(0);
  Node* current = rightPath.at(0);
  Node* postDom = leftPath.at(leftPath.size()-1);

  for(Node* n: leftPath) {
    leftCost += n->getWeight();
  }

  for(Node* n: rightPath) {
    if(parent.device != current.device) {
      rightCost += parent->edgeTo(current);
    }
    rightCost += n->getWeight();
  }

  if(rightCost > leftCost) {

  }
}
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

int main (void)
{
  
  DGraph g = generateRandomDAG(CHANCE, MAX_RANKS, MIN_RANKS, MIN_PER_RANK, MAX_PER_RANK );
  //Test the graph implementation
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
