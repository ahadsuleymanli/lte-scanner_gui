#include "mainwindow.h"
#include "qcustomplot.h"
#include <boost/thread/scoped_thread.hpp>
#include <QApplication>
#include <LTE-Tracker_adapter.h>
#include "lte-cell_data.h"
#include "lte_cell_info_container.h"
#include "ascii_art_dft.hpp"
#include <pthread.h>
#include <stdlib.h>






Cell_info_LL *cellsList;
double freq(8160e5);
bool stopFlag = false;
vector<float> lpdftVect;
double bw(0.1e6);
string freqStr = "18501e5";
bool cancelBIt = false;






void runLTETracker()
{
    const int argc1 = 3;
    const char * c = (boost::lexical_cast<std::string>(freq)).c_str();

    char * const argv1[3] = {" ","-f",const_cast<char*>(c)};

lteTracker(argc1,argv1,cellsList, cancelBIt);
    int n1 = fork();
    if (n1 > 0 ) {

         printf("parent\n");
         printf("%d %d \n", n1);
         printf(" my id is %d \n", getpid());
         return;
     }
    else if (n1 == 0 )
    {

        printf("First child\n");
        printf("%d %d \n", n1);
        printf("my id is %d  \n", getpid());
        return;
    }


}

void checkForFreqUpdate()
{

    boost::thread lteTrackerThread{runLTETracker}; //starting LTE-Tracker

    lteTrackerThread.detach();

    double prevFreq = freq;
    while(true){

        //cout<<"hi";
        if(freq!=prevFreq){
            const char * c = (boost::lexical_cast<std::string>(freq)).c_str();
            cout<<"freq changed\n-------------------\n------------------\n"<<c;
        //lteTrackerThread.interrupt();
        //cancelBIt = true;
        cancelBIt = true;

        prevFreq = freq;

        //cellsList->empty();
        //cancelBIt = false;
        //boost::thread lteTrackerThread{runLTETracker};
        //pthread_create(&lteTrackerThread, NULL, runLTETracker, (void *) message1);
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

}

void run2()
{

    while(TRUE){

    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
    //cout<<"printing: ";
    //cout<<cellsList->display();
    cellsList->print();
    cout<<endl;
    }

}

int main(int argc, char *argv[])
{
    cellsList = new Cell_info_LL();


    //boost::thread thread1{get_fft,boost::ref(stopFlag),boost::ref(freq),boost::ref(bw),boost::ref(freqStr),boost::ref(lpdftVect)};

    QApplication a(argc, argv);
    MainWindow w;

    w.setUsrpGetterParams( &stopFlag,  &freq, &bw, &freqStr, &lpdftVect);
    w.setCellsList(cellsList);
    w.show();
    //boost::thread runLteTracker{runLTETracker};
    boost::thread checkForFreqUpdate_thread{checkForFreqUpdate};

    a.exec();
//checkForFreqUpdate_thread.join();
    return 0;
}
