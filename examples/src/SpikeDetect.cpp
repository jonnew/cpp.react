#include <random>

#include "EphysInput.hpp"
#include "Time.hpp"
#include "Trode.hpp"

using namespace decoder;

int main()
{
    REACTIVE_DOMAIN(D, sequential)

    const size_t M = 8;

    // Time
    Time<D> time(30.0e3);

    std::vector<VoltageSource<D>> e;
    for (int i = 0; i < M; i++)
        e.push_back(MakeEventSource<D, VoltageT>());

    //EphysInput4<D> lfp(time, e[0], e[1], e[2], e[3]);



    //    //BandPassFilter<D> filter(time, raw[0]);
    //    std::vector<BandPassFilter<D>> filters;
    //    for (auto &r :raw)
    //        filters.emplace_back(time, r);
    //
    //    std::cout << "Sample period: " << time.Ts << std::endl;
    //    //std::cout << "Filter N: " << filters.size() << std::endl;
    //
    //    // Are merged into 1 TT
    //    using Tetrode = NTrode<D,
    //                           Events<D, double>,
    //                           Events<D, double>,
    //                           Events<D, double>,
    //                           Events<D, double>>;
    //
    // Tetrode tt(time,
    //           0.01,
    //           filters[0].Out,
    //           filters[1].Out,
    //           filters[2].Out,
    //           filters[3].Out);

    ////SpikeDetector detector(filter.Out, 0.1);

    Observe(time.Count, [](uint64_t count) {
        std::cout << "Current sample: " << count << std::endl;
    });

    Observe(time.Seconds, [](double sec) {
        std::cout << "Current seconds: " << sec << std::endl;
    });

    //Observe(filter.Out, [](double out) {
    //    std::cout << "Current value: " << out << std::endl;
    //});

    ///////////////////////
    // HARDWARE SIMULATION
    //////////////////////
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 1);

    // Make a sample block coming from 4 contacts
    for (int j = 0; j < 1000; j++) {
    
        // Graph update step
        DoTransaction<D>([&] {

            // Increment sample counter
            time.NewSample(); 

            // Make noise on all electrodes
            for (auto &electrode : e)
                electrode << d(gen);
        });
    }

    return 0;
}
