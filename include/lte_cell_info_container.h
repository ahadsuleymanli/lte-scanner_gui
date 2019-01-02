#ifndef LTE_CELL_INFO_CONTAINER_H
#define LTE_CELL_INFO_CONTAINER_H
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct Port
{
  string portSNR_string;
  vector<double> plot;
};

struct Node
{
  string cellID, frameTiming, health;

  vector<Port> port_list;
  vector<double> plot;
  Node *next;
};

class Cell_info_LL
{
    private:
    Node *head, *tail;
    public:
    Cell_info_LL()
    {
    head=NULL;
    tail=NULL;
    }
    ~Cell_info_LL(){
        Node* current = head;
        Node* next;
        while (current != NULL) {
            next = current->next;
            delete current;
            current = next;
        }
    }
    Node *createNode(string cellID);
    Node * getHead(){
        return head;
    }
    Node * getNext(Node *currentNode){
        return currentNode->next;
    }

    void addNode(string cellID, vector<string> portSNR_list);
    void addNode(string cellID, vector<string> portSNR_list, vector<double> plot);
    string display();
    void print();
    Node *getNodeByCellID(string cellID);
    void setPlot(string cellID, vector<double> plot);
    void deleteByCellID(string cellID)
    {
      //Node *toDelete= getNodeByCellID(cellID);
      Node *previous;
      Node *current;
      current = head;
      if( head!=NULL && (head->cellID).compare(cellID) == 0 ){
        head = head->next;
        delete current;
        return;
      }

      while (current != NULL && (current->cellID).compare(cellID) !=0)
      {
          previous = current;
          current = current->next;
      }
      if (current == NULL) return;
      previous->next = current->next;
      delete current;
    }
    void empty(){
        Node* current = head;
        Node* next;
        while (current != NULL) {
            next = current->next;

            clearPortList(current);
            delete current;
            current = next;
        }
        head=NULL;
        tail=NULL;
    }
    void clearPortList(Node* node){

        for(int i=0 ; node!=nullptr && i < node->port_list.size() ; i++){
            node->port_list[i].plot.clear();
        };
       node->port_list.clear();
    }
};

#endif // LTE_CELL_INFO_CONTAINER_H
