//
// Copyright 2010 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef ASCII_ART_DFT_HPP
#define ASCII_ART_DFT_HPP

#include <string>
#include <cstddef>
#include <vector>
#include <complex>
#include <stdexcept>
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
#include <boost/thread/scoped_thread.hpp>
//void get_fft(double &freq, bool &stopFlag, std::vector<float> &lpdftVect );
namespace usrp_get_fft{

    //! Type produced by the log power DFT function
    typedef std::vector<float> log_pwr_dft_type;

    /*!
     * Get a logarithmic power DFT of the input samples.
     * Samples are expected to be in the range [-1.0, 1.0].
     * \param samps a pointer to an array of complex samples
     * \param nsamps the number of samples in the array
     * \return a real range of DFT bins in units of dB
     */
    template <typename T> log_pwr_dft_type log_pwr_dft(
        const std::complex<T> *samps, size_t nsamps
    );

    /*!
     * Convert a DFT to a piroundable ascii plot.
     * \param dft the log power dft bins
     * \param width the frame width in characters
     * \param height the frame height in characters
     * \param samp_rate the sample rate in Sps
     * \param dc_freq the DC frequency in Hz
     * \param dyn_rng the dynamic range in dB
     * \param ref_lvl the reference level in dB
     * \return the plot as an ascii string
     */


} //namespace ascii_dft

/***********************************************************************
 * Implementation includes
 **********************************************************************/
#include <cmath>
#include <sstream>
#include <algorithm>

/***********************************************************************
 * Helper functions
 **********************************************************************/
namespace {/*anon*/

    static const double pi = double(std::acos(-1.0));

    //! Round a floating-point value to the nearest integer
    template <typename T> int iround(T val){
        return (val > 0)? int(val + 0.5) : int(val - 0.5);
    }

    //! Pick the closest number that is nice to display
    template <typename T> T to_clean_num(const T num){
        if (num == 0) return 0;
        const T pow10 = std::pow(T(10), int(std::floor(std::log10(std::abs(num)))));
        const T norm = std::abs(num)/pow10;
        static const int cleans[] = {1, 2, 5, 10};
        int clean = cleans[0];
        for (size_t i = 1; i < sizeof(cleans)/sizeof(cleans[0]); i++){
            if (std::abs(norm - cleans[i]) < std::abs(norm - clean))
                clean = cleans[i];
        }
        return ((num < 0)? -1 : 1)*clean*pow10;
    }

    //! Compute an FFT with pre-computed factors using Cooley-Tukey
    template <typename T> std::complex<T> ct_fft_f(
        const std::complex<T> *samps, size_t nsamps,
        const std::complex<T> *factors,
        size_t start = 0, size_t step = 1
    ){
        if (nsamps == 1) return samps[start];
        std::complex<T> E_k = ct_fft_f(samps, nsamps/2, factors+1, start,      step*2);
        std::complex<T> O_k = ct_fft_f(samps, nsamps/2, factors+1, start+step, step*2);
        return E_k + factors[0]*O_k;
    }

    //! Compute an FFT for a particular bin k using Cooley-Tukey
    template <typename T> std::complex<T> ct_fft_k(
        const std::complex<T> *samps, size_t nsamps, size_t k
    ){
        //pre-compute the factors to use in Cooley-Tukey
        std::vector<std::complex<T> > factors;
        for (size_t N = nsamps; N != 0; N /= 2){
            factors.push_back(std::exp(std::complex<T>(0, T(-2*pi*k/N))));
        }
        return ct_fft_f(samps, nsamps, &factors.front());
    }

    //! Helper class to build a DFT plot frame
    class frame_type{
    public:
        frame_type(size_t width, size_t height):
            _frame(width-1, std::vector<char>(height, ' '))
        {
            /* NOP */
        }

        //accessors to parts of the frame
        char &get_plot(size_t b, size_t z){return _frame.at(b+albl_w).at(z+flbl_h);}
        char &get_albl(size_t b, size_t z){return _frame.at(b)       .at(z+flbl_h);}
        char &get_ulbl(size_t b)          {return _frame.at(b)       .at(flbl_h-1);}
        char &get_flbl(size_t b)          {return _frame.at(b+albl_w).at(flbl_h-1);}

        //dimension accessors
        size_t get_plot_h(void) const{return _frame.front().size() - flbl_h;}
        size_t get_plot_w(void) const{return _frame.size() - albl_w;}
        size_t get_albl_w(void) const{return albl_w;}

        std::string to_string(void){
            std::stringstream frame_ss;
            for (size_t z = 0; z < _frame.front().size(); z++){
                for (size_t b = 0; b < _frame.size(); b++){
                    frame_ss << _frame[b][_frame[b].size()-z-1];
                }
                frame_ss << std::endl;
            }
            return frame_ss.str();
        }

    private:
        static const size_t albl_w = 6, flbl_h = 1;
        std::vector<std::vector<char> > _frame;
    };

} //namespace /*anon*/

/***********************************************************************
 * Implementation code
 **********************************************************************/
namespace usrp_get_fft{

    //! skip constants for amplitude and frequency labels
    static const size_t albl_skip = 5, flbl_skip = 20;

    template <typename T> log_pwr_dft_type log_pwr_dft(
        const std::complex<T> *samps, size_t nsamps, double w_n
    ){
        if (nsamps & (nsamps - 1))
            throw std::runtime_error("num samps is not a power of 2");

        //compute the window
        double win_pwr = 0;
        std::vector<std::complex<T> > win_samps;
        for(size_t n = 0; n < nsamps; n++){
            //double w_n = 1;
            //double w_n = 0.54 //hamming window
            //    -0.46*std::cos(2*pi*n/(nsamps-1))
            //;
           // w_n = 0.35875 //blackman-harris window
                -0.48829*std::cos(2*pi*n/(nsamps-1))
                +0.14128*std::cos(4*pi*n/(nsamps-1))
                -0.01168*std::cos(6*pi*n/(nsamps-1))
            ;
            //double w_n = 1 // flat top window
            //    -1.930*std::cos(2*pi*n/(nsamps-1))
            //    +1.290*std::cos(4*pi*n/(nsamps-1))
            //    -0.388*std::cos(6*pi*n/(nsamps-1))
            //    +0.032*std::cos(8*pi*n/(nsamps-1))
            //;
            win_samps.push_back(T(w_n)*samps[n]);
            win_pwr += w_n*w_n;
        }

        //compute the log-power dft
        log_pwr_dft_type log_pwr_dft;
        for(size_t k = 0; k < nsamps; k++){
            std::complex<T> dft_k = ct_fft_k(&win_samps.front(), nsamps, k);
            log_pwr_dft.push_back(float(
                + 20*std::log10(std::abs(dft_k))
                - 20*std::log10(T(nsamps))
                - 10*std::log10(win_pwr/nsamps)
                + 3
            ));
        }

        return log_pwr_dft;
    }

} //namespace ascii_dft

void get_fft(bool &stopFlag, double &freq, double &bw, std::string &freqStr ,std::vector<float> &lpdftVect ) {
    uhd::set_thread_priority_safe();

    std::string device_args("addr=192.168.10.2");
    std::string subdev("A:0");
    std::string ant("TX/RX");
    std::string ref("internal");

    double rate(1e6);
    //double freq(915e6);
    double gain(30);
    //double bw(20e6);
    //double bw;
    //bw = 20e6;
    //bw = 1e6;

    size_t num_bins = 512 * 2 ;

    //create a usrp device
    std::cout << std::endl;
    std::cout << boost::format("Creating the usrp device with: %s...") % device_args << std::endl;
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(device_args);

    // Lock mboard clocks
    std::cout << boost::format("Lock mboard clocks: %f") % ref << std::endl;
    usrp->set_clock_source(ref);

    //always select the subdevice first, the channel mapping affects the other settings
    std::cout << boost::format("subdev set to: %f") % subdev << std::endl;
    usrp->set_rx_subdev_spec(subdev);
    std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

    //set the sample rate
    if (rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
        return;
    }

    // set sample rate
    std::cout << boost::format("Setting RX Rate: %f Msps...") % (rate / 1e6) << std::endl;
    usrp->set_rx_rate(rate);
    std::cout << boost::format("Actual RX Rate: %f Msps...") % (usrp->get_rx_rate() / 1e6) << std::endl << std::endl;

    // set freq
    std::cout << boost::format("Setting RX Freq: %f MHz...") % (freq / 1e6) << std::endl;
    uhd::tune_request_t tune_request(freq);
    usrp->set_rx_freq(tune_request);
    std::cout << boost::format("Actual RX Freq: %f MHz...") % (usrp->get_rx_freq() / 1e6) << std::endl << std::endl;

    // set the rf gain
    std::cout << boost::format("Setting RX Gain: %f dB...") % gain << std::endl;
    usrp->set_rx_gain(gain);
    std::cout << boost::format("Actual RX Gain: %f dB...") % usrp->get_rx_gain() << std::endl << std::endl;

    // set the IF filter bandwidth
    std::cout << boost::format("Setting RX Bandwidth: %f MHz...") % (bw / 1e6) << std::endl;
    usrp->set_rx_bandwidth(bw);
    std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (usrp->get_rx_bandwidth() / 1e6) << std::endl << std::endl;


    // set the antenna
    std::cout << boost::format("Setting RX Antenna: %s") % ant << std::endl;
    usrp->set_rx_antenna(ant);
    std::cout << boost::format("Actual RX Antenna: %s") % usrp->get_rx_antenna() << std::endl << std::endl;

    boost::this_thread::sleep(boost::posix_time::seconds(1));

    //Check Ref and LO Lock detect
    std::vector<std::string> sensor_names;
    sensor_names = usrp->get_rx_sensor_names(0);
    if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked") != sensor_names.end()) {
        uhd::sensor_value_t lo_locked = usrp->get_rx_sensor("lo_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % lo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }
    sensor_names = usrp->get_mboard_sensor_names(0);
    if ((ref == "mimo") and (std::find(sensor_names.begin(), sensor_names.end(), "mimo_locked") != sensor_names.end())) {
        uhd::sensor_value_t mimo_locked = usrp->get_mboard_sensor("mimo_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % mimo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(mimo_locked.to_bool());
    }
    if ((ref == "external") and (std::find(sensor_names.begin(), sensor_names.end(), "ref_locked") != sensor_names.end())) {
        uhd::sensor_value_t ref_locked = usrp->get_mboard_sensor("ref_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % ref_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(ref_locked.to_bool());
}

    //create a receive streamer
    uhd::stream_args_t stream_args("fc32"); //complex floats
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

    //allocate recv buffer and metatdata
    uhd::rx_metadata_t md;
    std::vector<std::complex<float> > buff(num_bins);

    //------------------------------------
    // Initialize
    //------------------------------------
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    //boost::timer t;
    //t.restart();
    double freq2=freq;
    double bw2=freq;
    while( !stopFlag /*&& t.elapsed()<=1.1*/){

        if(freq2!=freq){
            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

            freq2=freq;

            uhd::tune_request_t tune_request(freq);
            usrp->set_rx_freq(tune_request);
            //boost::this_thread::sleep(boost::posix_time::seconds(1));
            rx_stream = usrp->get_rx_stream(stream_args);
            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
            //usrp->set_rx_bandwidth(bw);

        }
        if(bw2!=bw){
            bw2 = bw;

            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
            uhd::tune_request_t tune_request(freq);
            usrp->set_rx_freq(tune_request);
            usrp->set_rx_bandwidth(bw);
            //boost::this_thread::sleep(boost::posix_time::seconds(1));
            rx_stream = usrp->get_rx_stream(stream_args);
            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        }
        freqStr = "usrp freq: " + std::to_string(usrp->get_rx_freq()/1e6) + " MHZ, bw: " + std::to_string(usrp->get_rx_bandwidth()/1e6 );
        freqStr+=" MHZ";
        size_t num_rx_samps = rx_stream->recv(
            &buff.front(), buff.size(), md
        );
        if (num_rx_samps != buff.size()) continue;
        boost::this_thread::sleep(boost::posix_time::milliseconds(20));

        usrp_get_fft::log_pwr_dft_type lpdft = (
            usrp_get_fft::log_pwr_dft(&buff.front(), num_rx_samps, bw / 1e6)
        );

        lpdftVect.clear();
        for(int i = 0 ; i<lpdft.size(); i++){
            lpdftVect.push_back(lpdft[i]);
        }

    }


    //------------------------------------
    //Finishing up
    //------------------------------------
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    std::cout << std::endl << "get_fft thread Done!" << std::endl << std::endl;


}

#endif /*ASCII_ART_DFT_HPP*/



