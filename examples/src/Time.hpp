#pragma once

#include "DecoderTypes.h"
#include "react/Algorithm.h"
#include "react/Domain.h"
#include "react/Signal.h"

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

} // namespace decoder
