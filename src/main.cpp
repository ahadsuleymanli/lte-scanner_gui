#include "mainwindow.h"
#include "qcustomplot.h"
#include <boost/thread/scoped_thread.hpp>
#include <QApplication>
#include <LTE-Tracker_adapter.h>
#include "lte-cell_data.h"
#include "lte_cell_info_container.h"
Cell_info_LL *cellsList;


void runLTETracker()
{
    const int argc1 = 3;
    char * const argv1[3] = {" ","-f","18501e5"};
    lteTracker(argc1,argv1,cellsList);
}

void run2()
{

    while(TRUE){

    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
    //cout<<"printing: ";
    cellsList->display();
    cout<<endl;
    }

}

int main(int argc, char *argv[])
{
    cellsList = new Cell_info_LL();
    boost::scoped_thread<> t{runLTETracker}; //starting LTE-Tracker
    boost::scoped_thread<> t2{run2};
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
