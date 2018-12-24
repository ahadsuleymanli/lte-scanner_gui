
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

// Helper function to plot a transfer function on the screen.
void plot_trace(
  // Trace to plot
  const vec & Y,
  const vec & X,
  // Range of trace that is to be plotted
  const double & x_min,
  const double & x_max,
  const double & x_tick,
  const double & x_supermark,
  const double & y_min,
  const double & y_max,
  const double & y_tick,
  // Coordinate of the upper left corner of the plot.
  const uint16 & ul_row,
  const uint16 & ul_col,
  // Coordinate of the lower right corner of the plot.
  const uint16 & lr_row,
  const uint16 & lr_col,
  // Whether to connect the dots in the y dimension.
  bool connect_the_dots
) {
  ASSERT(ul_row<lr_row);
  ASSERT(ul_col<lr_col);

  // This is the area including the axes
  const uint16 canvas_width=lr_col-ul_col+1;
  const uint16 canvas_height=lr_row-ul_row+1;
  // This is the actual plot area, not including axes
  const uint16 plot_width=canvas_width-5;
  const uint16 plot_height=canvas_height-1;
  const uint16 plot_ul_row=ul_row;
  const uint16 plot_ul_col=ul_col+5;
  const uint16 plot_lr_row=lr_row-1;
  const uint16 plot_lr_col=lr_col;

  //--------------------------------------------------
  // Plot trace

  vec x=linspace(x_min,x_max,plot_width);
  vec y=interp1(X,Y,x);

  //cellsList_global->setPlot(currentCID,x,y);
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




  // Record where a particular cell was most recently printed.


  // Settings that the user can change in realtime
  bool auto_refresh=true;
  double refresh_delay_sec=1;
  bool fifo_status=false;
  bool avg_values=true;
  //disp_mode_t disp_mode=STD;
  int8 detail_type=0;
#define N_DETAILS 2
  // Send control chars directly to program.
  //cbreak();
  //halfdelay(round_i(refresh_delay_sec*10.0));
  int16 highlight_row=-1;
  while (true) {

      boost::mutex::scoped_lock lock(tracked_cell_list.mutex);
      list <tracked_cell_t *>::iterator it=tracked_cell_list.tracked_cells.begin();
      vector <tracked_cell_t *> pass2_display;
      while (it!=tracked_cell_list.tracked_cells.end()) {
        tracked_cell_t & tracked_cell=(*(*it));

        // Deadlock possible???
        boost::mutex::scoped_lock lock1(tracked_cell.fifo_mutex);
        boost::mutex::scoped_lock lock2(tracked_cell.meas_mutex);

        uint8 n_rows_required=tracked_cell.n_ports+2;

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
          portString =" S ";
          stringstream stream;
          stream << fixed << setprecision(1) << db10(tracked_cell.sync_sp_av/tracked_cell.sync_np_av);
          portString += stream.str()+ " dB SNR";
        } else {
          portString = "  S   -Inf dB SNR\n";
        }
        portSNR_list.push_back(portString);

        cellsList->addNode(to_str(tracked_cell.n_id_cell), portSNR_list);
        portSNR_list.clear();
        // If this cell has been displayed before, try to display it
        // in the same location.

              //display_cell(tracked_cell,row_desired+CELL_DISP_START_ROW,fifo_status,avg_values,expert_mode);

        ++it;
      }



     if(0) {
      // Zoom into port details

      // Shortcuts
      const int16 & n_id_cell=stoi(cellsList->getHead()->cellID);
      const int8 & port_num=0;

      {
        boost::mutex::scoped_lock lock(tracked_cell_list.mutex);
        list <tracked_cell_t *>::iterator it=tracked_cell_list.tracked_cells.begin();
        vector <tracked_cell_t *> pass2_display;
        bool cell_found=false;
        while (it!=tracked_cell_list.tracked_cells.end()) {
          tracked_cell_t & tracked_cell=(*(*it));
          if ((tracked_cell.n_id_cell==n_id_cell)&&((port_num==-1)||(port_num<tracked_cell.n_ports))) {
            cell_found=true;
            break;
          }
          ++it;
        }
        if (cell_found) {
          tracked_cell_t & tracked_cell=(*(*it));
          boost::mutex::scoped_lock lock2(tracked_cell.meas_mutex);
          if (detail_type==0) {
            // Plot transfer function mag
            vec trace;
            if (port_num==-1) {
              trace=db10(sqr(tracked_cell.sync_ce));
            } else {
              trace=db10(sqr(tracked_cell.ce.get_row(port_num)));
            }

            plot_trace(
              // Trace desc.
              trace,itpp_ext::matlab_range(0.0,71.0),
              // X axis
              0,71,12,NAN,
              // Y axis
              -50,0,10,
              // UL corner
              1,0,
              // LR corner
              72-2,0+72+4,
              true
            );

          }
 /*
          else if (detail_type==1) {
            // Plot transfer function phase
            vec trace;
            double mean_ang;
            if (port_num==-1) {
              trace=arg(tracked_cell.sync_ce);
              mean_ang=arg(sum(exp(J*trace(5,66))));
              trace=arg(tracked_cell.sync_ce*exp(J*-mean_ang));
              trace(0)=NAN;
              trace(1)=NAN;
              trace(2)=NAN;
              trace(3)=NAN;
              trace(4)=NAN;
              trace(67)=NAN;
              trace(68)=NAN;
              trace(69)=NAN;
              trace(70)=NAN;
              trace(71)=NAN;
            } else {
              trace=arg(tracked_cell.ce.get_row(port_num));
              mean_ang=arg(sum(exp(J*trace)));
              trace=arg(tracked_cell.ce.get_row(port_num)*exp(J*-mean_ang));
            }
            trace=trace/pi*180;
            plot_trace(
              // Trace desc.
              trace,itpp_ext::matlab_range(0.0,71.0),
              // X axis
              0,71,12,(mean_ang+pi)/(2*pi)*71,
              // Y axis
              -40,40,10,
              // UL corner
              1,0,
              // LR corner
              LINES-2,0+72+4,
              false
            );
            move(0,0);
            stringstream ss;
            ss << "Cell " << n_id_cell;
            if (port_num==-1) {
              ss << " Sync channel phase\n";
            } else {
              ss << " port " << port_num << " phase\n";
            }
            attron(COLOR_PAIR(CYAN));

            attroff(COLOR_PAIR(CYAN));
            move(LINES-1,0);
            attron(COLOR_PAIR(GREEN));

            //print_right("right arrow > phase response");

            attroff(COLOR_PAIR(GREEN));
          } else if (detail_type==2) {
            // Frequency domain autocorrelation
            const vec trace=abs(tracked_cell.ac_fd);
            plot_trace(
              // Trace desc.
              trace,itpp_ext::matlab_range(0.0,11.0),
              // X axis
              0,11,2,NAN,
              // Y axis
              0,1.2,.5,
              // UL corner
              0,0,
              // LR corner
              LINES-3,0+72+4,
              true
            );
            move(LINES-2,0);
            printw("Cell ID: %i\n",n_id_cell);
            printw("Frequency domain channel autocorrelation function. x-axis spans 1.26MHz\n");
          } else if (detail_type==3) {

            // Time domain autocorrelation
            const vec trace=abs(tracked_cell.ac_td);
            plot_trace(
              // Trace desc.
              trace,itpp_ext::matlab_range(0.0,71.0)*.0005,
              // X axis
              0,71*.0005,.010,NAN,
              // Y axis
              0,3.2,.5,
              // UL corner
              0,0,
              // LR corner
              LINES-3,0+72+4,
              true
            );
            move(LINES-2,0);
            printw("Cell ID: %i\n",n_id_cell);
            printw("Time domain channel autocorrelation function. x-axis spans 35.5ms\n");
          }
        */
        } else {
          move(1,0);
          printw("Cell is no longer being tracked. Press left arrow to go back!\n");
        }
      }

    }



    // Handle keyboard input.


  }
}

