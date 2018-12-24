#include "lte_cell_info_container.h"


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

    nodeToAdd->portSNR_list.clear();
    for(int i = 0 ; i < portSNR_list.size() && nodeToAdd!=NULL ; i++){

        nodeToAdd->portSNR_list.push_back(portSNR_list[i]);


    }

}
void Cell_info_LL::setPlot(string cellID, vector<double> x,vector<double> y){
    Node *nodeToAdd;
    //check if cellID exists
    nodeToAdd = getNodeByCellID(cellID);
    if (nodeToAdd==NULL){
        nodeToAdd = createNode(cellID);
    }
    nodeToAdd->x.clear();
    nodeToAdd->y.clear();
    for(int i = 0 ; i < x.size() && nodeToAdd!=NULL ; i++){
        nodeToAdd->x.push_back(x[i]);
    }
    for(int i = 0 ; i < y.size() && nodeToAdd!=NULL ; i++){
        nodeToAdd->y.push_back(y[i]);
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

void Cell_info_LL::display()
{
  Node *temp;
  temp=head;
  while(temp!=NULL)
  {
    cout<<temp->cellID <<" ";
    cout<< (temp->portSNR_list)[0] <<"\t";
    temp=temp->next;
  }
  delete temp;
}
