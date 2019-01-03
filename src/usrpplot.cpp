#include "usrpplot.h"
#include <iostream>
#include <QApplication>
#include "udp_client_server.h"
using namespace std;

usrpPlot::usrpPlot()
{

}
/*
int main(int argc, char *argv[])
{
    QString addr= "192.168.1.2";
    int port =7124;

    udp_client_server::udp_server udpServer = new udp_client_server::udp_server();
    QString s;
    cout << "hellow world!\n";
    return 0;
}
*/

#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>

int main(int argc, char *argv[]) {

    return EXIT_SUCCESS;
}

