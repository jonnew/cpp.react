#pragma once

#include <deque>

#include "DecoderTypes.h"
#include "react/Domain.h"

namespace decoder {

using namespace react;
using namespace decoder::types;

// Transforms LFPs int Spikes
template <typename D, size_t N, size_t PRE=10, size_t POST=22>
class Trode {

static constexpr size_t L {PRE + POST};
using Waveform = SpikeWaveform<N, L>;
using Buffer = std::deque<LFP<N>>;

public:
    USING_REACTIVE_DOMAIN(D)

    LFPs<D,N> &lfp;
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

    Trode(Time<D>& t, LFPs<D,N> &l, VoltageT thr)
    : time(t)
    , lfp(l)
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

template <typename D>
using Electrode = Trode<D, 1>;

template <typename D>
using StereoTrode = Trode<D, 2>;

template <typename D>
using Tetrode = Trode<D,4>;

} // namespace decoder
