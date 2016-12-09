#pragma once

#include "DecoderTypes.h"
#include "react/Domain.h"

namespace decoder {

using namespace react;
using namespace decoder::types;

namespace detail {

    template <size_t N>
    struct VoltageTuple {
        typedef std::tuple<VoltageT> type;
    };

    template <>
    struct VoltageTuple<1> {
        typedef std::tuple<VoltageT> type;
    };

    template <>
    struct VoltageTuple<2> {
        typedef std::tuple<VoltageT, VoltageT> type;
    };

    template <>
    struct VoltageTuple<4> {
        typedef std::tuple<VoltageT, VoltageT, VoltageT, VoltageT> type;
    };

    template <>
    struct VoltageTuple<8> {
        typedef std::tuple<VoltageT,
                           VoltageT,
                           VoltageT,
                           VoltageT,
                           VoltageT,
                           VoltageT,
                           VoltageT,
                           VoltageT> type;
    };

    template <size_t N>
    using VTuple = typename VoltageTuple<N>::type;
}

// Voltage Event streams to LFPs
template <typename D, typename... Vs>
class EphysInput {

static constexpr size_t N {sizeof...(Vs)};
public:
    USING_REACTIVE_DOMAIN(D)

    Events<D, detail::VTuple<N>> voltages; // Input
    LFPs<D,N> lfp = Transform(voltages, [](detail::VTuple<N> v, LFP<N> l) {

        for (size_t i = 0; i < N; i++)
            l[i] = std::get<0>(v);

        return l;

    }); // Output

    EphysInput(Time<D>& t, Vs& ...sources)
    : time(time)
    {
        voltages = Join(sources...);
    }

private:
    Time<D> &time;
};


template <typename D>
using EphysInput1 = EphysInput<D, VoltageSource<D>>;

template <typename D>
using EphysInput2 = EphysInput<D, VoltageSource<D>, VoltageSource<D>>;

template <typename D>
using EphysInput4 = EphysInput<D,
                               VoltageSource<D>,
                               VoltageSource<D>,
                               VoltageSource<D>,
                               VoltageSource<D>>;

template <typename D>
using EphysInput8 = EphysInput<D,
                               VoltageSource<D>,
                               VoltageSource<D>,
                               VoltageSource<D>,
                               VoltageSource<D>>;
} // namespace decoder

