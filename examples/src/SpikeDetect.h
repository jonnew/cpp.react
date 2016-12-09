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

#include "DecoderTypes.h"

#define BLOCKSIZE 10

namespace decoder {

using namespace react;
using namespace decoder::types;

template <typename T>
struct Incrementer {
    T operator()(Token, T v) const { return v + 1; }
};

template <typename D>
class Time
{
public:
    USING_REACTIVE_DOMAIN(D)

    TimeT Ts;
    EventSourceT<> NewSample = MakeEventSource<D>();
    SignalT<SampleIndexT> Count
        = Iterate(NewSample, (SampleIndexT)0, Incrementer<SampleIndexT>());
    SignalT<TimeT> Seconds
        = Iterate(NewSample, 0.0, [&](Token, TimeT sec) { return sec + Ts; });

    Time(TimeT sample_rate_hz)
    : Ts(1.0 / sample_rate_hz)
    {
        // Nothing
    }
};


// Transforms LFPs int Spikes
template <typename D, size_t N, size_t PRE=10, size_t POST=22>
class Trode {

static constexpr size_t L {PRE + POST};
using Waveform = SpikeWaveform<N, L>;
using Buffer = std::deque<LFP<N>>;

public:
    USING_REACTIVE_DOMAIN(D)

    LFPs<D,N> lfp;
    Spikes<D, L, N> spikes = Transform(lfp, [&](LFP<N> l, SpikeWaveform<L, N> s) {

        // Update buffer
        buffer.push_front(l);
        buffer.pop_back();

        // If not already triggered, check all voltage samples to see any
        // crossed threshold
        if (!triggered ) {
            for (const auto &volt : l) {
                triggered = l < threshold;
                if (triggered)
                    break;
            }
        } else if(!--dead_count) {
            triggered = false;
            dead_count = POST;
            return snapshot(buffer);
        }
    });

    Trode(Time<D>& t, VoltageT thr)
    : time(t)
    , threshold(thr)
    {
        buffer.resize(L);
    }

private:

    Time<D> &time;
    bool triggered {false};
    Buffer buffer;
    VoltageT threshold;
    size_t dead_count {POST};

    Waveform snapshot(const Buffer &buffer) {

       Waveform w;
       for (int i = 0; i < L; i++)
           w[i] = buffer[i];

       return w;
    }
};

//template <typename D>
//class BandPassFilter {
//public:
//    USING_REACTIVE_DOMAIN(D)
//
//    // Samples collected from API
//    LFP EventsT<double> In;
//    EventsT<double> Out
//        = react::Transform(In, [&](double in) { return filt_.do_sample(in); });
//
//    BandPassFilter(Time<D> &time, const EventsT<double> s)
//    : time_(time)
//    , In(s)
//    //, period(1/time.Ts);
//    , filt_(dsp::BPF, 21, 30e3, 300, 3e3)
//    {
//        // Nothing
//        std::cout << 1.0/time.Ts;
//    }
//
//private:
//    dsp::Filter filt_;
//    Time<D> &time_;
//};

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
//
} // namespace decoder
