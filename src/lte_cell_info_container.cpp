#include "lte_cell_info_container.h"
#include <sstream>


Node * Cell_info_LL::createNode(string cellID)
{

  if(head==NULL)
  {
    head=new Node;
    head->cellID=cellID;
    head->next=NULL;
    tail=head;
    return head;
  }
  else
  {
    //Node *temp=new Node;
    //temp->cellID=cellID;
    //temp->next=NULL;

    tail->next=new Node;
    tail->next->cellID=cellID;
    tail->next->next=NULL;
    tail = tail->next;
    //tail=temp;
    //temp = NULL;
    return tail;
  }


}
void Cell_info_LL::addNode(string cellID, vector<string> portSNR_list){
    Node *nodeToAdd;
    //check if cellID exists
    nodeToAdd = getNodeByCellID(cellID);
    if (nodeToAdd==NULL){
        nodeToAdd = createNode(cellID);
    }

    clearPortList(nodeToAdd);
    for(int i = 0 ; i < portSNR_list.size() && nodeToAdd!=NULL ; i++){
        nodeToAdd->port_list.push_back(Port());
        nodeToAdd->port_list[i].portSNR_string = portSNR_list[i];
    }

}

void Cell_info_LL::addNode(string cellID, vector<string> portSNR_list, vector<double> plot){
    Node *nodeToAdd;
    //check if cellID exists
    nodeToAdd = getNodeByCellID(cellID);
    if (nodeToAdd==NULL){
        nodeToAdd = createNode(cellID);
    }

    clearPortList(nodeToAdd);
    for(int i = 0 ; i < portSNR_list.size() && nodeToAdd!=NULL ; i++){
        nodeToAdd->port_list.push_back(Port());
        nodeToAdd->port_list[i].portSNR_string = portSNR_list[i];
        for(int j = 0; j < plot.size() ; j++){
            nodeToAdd->port_list[i].plot.push_back(plot[j]);
        }
    }

}




void Cell_info_LL::setPlot(string cellID, vector<double> plot){
    Node *nodeToAdd;
    //check if cellID exists

    if (cellID.compare("")){
        nodeToAdd = head;
    }
    else {
        nodeToAdd = getNodeByCellID(cellID);
        if (nodeToAdd==NULL){
            nodeToAdd = createNode(cellID);
        }
    }

    (nodeToAdd->plot).clear();
    for(int i = 0 ; i < plot.size() && nodeToAdd!=nullptr ; i++){
        (nodeToAdd->plot).push_back(plot[i]);
    }

}

Node * Cell_info_LL::getNodeByCellID(string cellID)
{
  Node *current;
  current=head;
  while (current!=NULL && (current->cellID).compare(cellID) !=0 ){  //while they are not equal
    current = current->next;
  }
  return current;
}

string Cell_info_LL::display()
{
  Node *temp;
  temp=head;

  string printStr = "";
  while(temp!=NULL)
  {
    printStr += temp->cellID +" ";
    printStr +=temp->port_list[0].portSNR_string + "\t";
    temp=temp->next;
  }
  delete temp;
  return printStr;
}

void Cell_info_LL::print()
{
  Node *temp;
  temp=head;

  while(temp!=NULL)
  {
    cout << temp->cellID <<" ";
    cout << temp->port_list[0].portSNR_string << "\t";
    temp=temp->next;
  }
  delete temp;

}
