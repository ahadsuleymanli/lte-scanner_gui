
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <itpp/itbase.h>
#include <itpp/signal/transforms.h>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <list>
#include <sstream>
#include <signal.h>
#include <queue>
#include <curses.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "rtl-sdr.h"
#include "common.h"
#include "macros.h"
#include "lte_lib.h"
#include "constants.h"
#include "capbuf.h"
#include "itpp_ext.h"
#include "searcher.h"
#include "dsp.h"
#include "rtl-sdr.h"
#include "LTE-Tracker_adapter.h"
#include "lte_cell_info_container.h"
#include <string>
#include <cmath>

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

// Minimum terminal screensize required to run this program.
#define LINES_MIN 24
#define COLS_MIN 80

using namespace itpp;
using namespace std;

// Struct used to keep trach of which cell and which port has been displayed
// on a particular line.
typedef struct {
  bool occupied;
  // -1 indicates this row is is not one of the ports
  int16 n_id_cell;
  // -1 indicates this is the 'synchronization' port.
  int8 port_num;
} row_desc_t;


Cell_info_LL * cellsList_global;
string currentCID;





// Indicate that certain rows are occupied
void set_occupied(
  const tracked_cell_t & tracked_cell,
  vector <row_desc_t> & row_desc,
  const uint16 & print_row
) {
  row_desc[print_row].occupied=true;
  for (uint8 t=0;t<tracked_cell.n_ports;t++) {
    row_desc[print_row+t+1].occupied=true;
    row_desc[print_row+t+1].n_id_cell=tracked_cell.n_id_cell;
    row_desc[print_row+t+1].port_num=t;
  }
  row_desc[print_row+tracked_cell.n_ports+1].occupied=true;
  row_desc[print_row+tracked_cell.n_ports+1].n_id_cell=tracked_cell.n_id_cell;
  row_desc[print_row+tracked_cell.n_ports+1].port_num=-1;
}

// Process that displays the status of all the tracker threads.
enum disp_mode_t {STD, DETAIL};

void display_thread(
  sampbuf_sync_t & sampbuf_sync,
  global_thread_data_t & global_thread_data,
  tracked_cell_list_t & tracked_cell_list,
  bool & expert_mode,
  Cell_info_LL *cellsList
) {
  cellsList_global = cellsList;
  global_thread_data.display_thread_id=syscall(SYS_gettid);


  while (true) {
      boost::mutex::scoped_lock lock(tracked_cell_list.mutex);
      list <tracked_cell_t *>::iterator it=tracked_cell_list.tracked_cells.begin();
      vector <tracked_cell_t *> pass2_display;
      while (it!=tracked_cell_list.tracked_cells.end()) {
        tracked_cell_t & tracked_cell=(*(*it));

        // Deadlock possible???
        boost::mutex::scoped_lock lock1(tracked_cell.fifo_mutex);
        boost::mutex::scoped_lock lock2(tracked_cell.meas_mutex);

        vector<string> portSNR_list;
        for (uint8 t=0;t<tracked_cell.n_ports;t++) {
            string portString =" P" + to_string(t) + " ";
            if (isfinite(db10(tracked_cell.crs_sp_raw_av(t)/tracked_cell.crs_np_av(t)))) {
              stringstream stream;
              stream << fixed << setprecision(1) << db10(tracked_cell.crs_sp_raw_av(t)/tracked_cell.crs_np_av(t));
              portString += stream.str() + " dB SNR";
            } else {
              portString += " -Inf dB SNR";
            }
            portSNR_list.push_back(portString);
        }
        string portString;
        if (isfinite(db10(tracked_cell.sync_sp_av/tracked_cell.sync_np_av))) {
          portString =" S  ";
          stringstream stream;
          stream << fixed << setprecision(1) << db10(tracked_cell.sync_sp_av/tracked_cell.sync_np_av);
          portString += stream.str()+ " dB SNR";
        } else {
          portString = "  S   -Inf dB SNR";
        }
        portSNR_list.push_back(portString);

        cellsList->addNode(to_str(tracked_cell.n_id_cell), portSNR_list);
        portSNR_list.clear();

        // Plot transfer function mag
        vec trace;
        int port_num = 0;
        if (port_num==-1) {
          trace=db10(sqr(tracked_cell.sync_ce));
        } else {
          trace=db10(sqr(tracked_cell.ce.get_row(port_num)));
        }

        vector<double> traceVector;
        for(int i = 0 ;i<trace.size(); i++){
            traceVector.push_back(trace[i]);
        }
        cellsList->setPlot(to_str(tracked_cell.n_id_cell),traceVector);

        ++it;
      }

  }
}

