#pragma once

#include <array>
#include "react/Event.h"

namespace decoder {
namespace types {

using namespace react;

// Primatives
using VoltageT = float;
using LocationT = float;
using SampleIndexT = uint64_t;
using TimeT = double;

// Voltage
template <typename D>
using VoltageSource = EventSource<D, VoltageT>;

template <typename D, size_t N>
using VoltateSourceVec = std::array<VoltageSource<D>, N>;

template <typename D>
using Voltages = Events<D, VoltageT>;

//template <typename D, typename... Args>
//using VoltagePack = std::tuple<Args...>;
//
//template <typename D, typename... Args>
//using VoltagePacks = Events<D, std::tuple<Args...>>;

// LFP
template <size_t N>
using LFP = std::array<VoltageT, N>;

template <typename D, size_t N>
using LFPs = Events<D, LFP<N>>;

// Spikes
template <size_t L, size_t N>
using SpikeWaveform = std::array<LFP<N>, L>;

template <typename D, size_t L, size_t N>
using Spikes = Events<D, SpikeWaveform<L, N>>;

// Amplitudes
template <size_t N>
using Amplitude = std::array<VoltageT, N>;

template <typename D, size_t N>
using Amplitudes = Signal<D, Amplitude<N>>;

// Position
using Position = std::tuple<LocationT, LocationT, LocationT>;

template <typename D>
using Positions = Events<D, Position>;

} // namespace types
} // namespace decoder
