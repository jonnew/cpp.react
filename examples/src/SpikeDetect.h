#include <deque>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "filt.h"
#include "react/Algorithm.h"
#include "react/Domain.h"
#include "react/Event.h"
#include "react/Observer.h"
#include "react/Signal.h"
#include "react/common/Util.h"

#define BLOCKSIZE 10
#define SPK_PRE_SAMP 10
#define SPK_POST_SAMP 22

using namespace react;

template <typename T>
struct Incrementer {
    T operator()(Token, T v) const { return v + 1; }
};

enum {out_spike, in_spike};

template <typename D>
class Time
{
public:
    USING_REACTIVE_DOMAIN(D)

    double Ts;
    EventSourceT<> NewSample = MakeEventSource<D>();
    SignalT<uint64_t> Count = Iterate(NewSample, (uint64_t)0, Incrementer<uint64_t>());
    SignalT<double> Seconds
        = Iterate(NewSample, 0.0, [&](Token, double sec) { return sec + Ts; });

    Time(double sample_rate_hz)
    : Ts(1.0 / sample_rate_hz)
    {
        // Nothing
    }
};

//template <typename D>
//class Electrode {
//public:
//    USING_REACTIVE_DOMAIN(D)
//
//    // Samples collected from API
//    EventSourceT<double> Out = MakeEventSource<D, double>();
//
//    Electrode(Time<D> &time)
//    : time_(time)
//    {
//        // Nothing
//    }
//
//private:
//    Time<D> &time_;
//};

//template <int E, int L>
//class Spike
//{
//public:
//
//    double Waveform[E][L];
//
//    // Note: To be used as a signal value type,
//    // values of the type must be comparable
//    //bool operator==(const Spike& other) const
//    //{
//    //    return this == &other;
//    //}
//};

template <typename D, typename ...Ts>
class NTrode {
public:
    USING_REACTIVE_DOMAIN(D)

    //SignalT<Spike<sizeof...(Ts),SPK_LEN>> Spikes = Iterate(voltages_, [&]() {

    Signal<D, double> Spikes
        = Iterate(voltages_, 0.0, [&](double v, double spk) {

            // If detector is set and we are over threshold
            set_ = !(set_ && v > thresh_);

            // If we are at at the end of detected spike and we are ready to
            // transmitted the buffer
            if (!set_ && !dead_count_--) {
                set_ = true;
                dead_count_ = sizeof...(Ts) * SPK_POST_SAMP;
                return 42.0;
            } else {
                return 0.0;
            }

          });

    // Update buffer
    // waveform_buffer_.emplace{
    // voltages_
    // Is current sample over threshold? If so, emit

    NTrode(Time<D>& time, double thresh, Ts& ...sources)
    : time_(time)
    , thresh_(thresh)
    {
        voltages_ = Merge(sources...);
        waveform_buffer_.resize(SPK_POST_SAMP + SPK_PRE_SAMP);
    }

private:
    Time<D> &time_;
    EventsT<double> voltages_;
    std::deque<std::array<double, sizeof...(Ts)>> waveform_buffer_;
    size_t dead_count_ {sizeof...(Ts) * SPK_POST_SAMP};
    bool set_ {true};
    double thresh_;
};

template <typename D>
class BandPassFilter {
public:
    USING_REACTIVE_DOMAIN(D)

    // Samples collected from API
    EventsT<double> In;
    EventsT<double> Out
        = react::Transform(In, [&](double in) { return filt_.do_sample(in); });

    BandPassFilter(Time<D> &time, const EventsT<double> s)
    : time_(time)
    , In(s)
    //, period(1/time.Ts);
    , filt_(dsp::BPF, 21, 30e3, 300, 3e3)
    {
        // Nothing
        std::cout << 1.0/time.Ts;
    }

private:
    dsp::Filter filt_;
    Time<D> &time_;
};

//template <typename D>
//class SpikeDetector {
//public:
//    USING_REACTIVE_DOMAIN(D)
//
//    // Samples collected from API
//    EventsT<double> In;
//    EventsT<double> Out = react::Filter(In, [&](double in) {
//
//        if (in_spike_ && counter_--) {
//            return true;
//        } else if (in > threshold_) {
//            in_spike_ = true;
//            return true;
//        } else {
//            counter_ = SPK_LEN;
//            return false;
//        }
//    });
//
//    SpikeDetector(const EventsT<double> s, const double threshold)
//    : In(s), threshold_(threshold)
//    {
//        // Nothing
//    }
//
//private:
//    bool in_spike_{false};
//    int counter_{SPK_LEN};
//    const double threshold_;
//
//    // Buffer [pre i post], i is the current sample being checked. One threshold crossing, The whole buffer is sent as a signal
//};
