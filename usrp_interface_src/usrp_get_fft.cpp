#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <iostream>
#include "ascii_art_dft.hpp"
#include <boost/thread/scoped_thread.hpp>
#include <stdlib.h>



int UHD_SAFE_MAIN(int argc, char *argv[]){
    double freq(915e6);
    bool stopFlag;
    stopFlag = false;
    std::vector<float> lpdftVect;
    //boost::thread thread1{get_fft,boost::ref(freq),boost::ref(stopFlag),boost::ref(lpdftVect)};

    boost::timer t1;
    t1.restart();

    int n1 = fork();
    if (n1 > 0 ) {
         printf("parent\n");
         printf("%d %d \n", n1);
         printf(" my id is %d \n", getpid());
     }
    else if (n1 == 0 )
    {
        printf("First child\n");
        exit(0);
        printf("%d %d \n", n1);
        printf("my id is %d  \n", getpid());
    }
    /*
    while( t1.elapsed()<0.5 ){
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        if(lpdftVect.size())
            std::cout<<lpdftVect[0]<<std::endl;
    }
*/
    stopFlag=true;
    std::cout<<"\nmain done too!\n";
    return 0;
}



