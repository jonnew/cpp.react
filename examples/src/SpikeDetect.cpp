#include "SpikeDetect.h"

int main()
{
    REACTIVE_DOMAIN(D, sequential)

    const size_t N = 256;

    using raw_size_t = std::array<EventSource<D, double>, N>::size_type;

    // Time
    Time<D> time(30.0e3);
    std::cout << "Sample period: " << time.Ts << std::endl;

    // 256 electrodes
    //std::vector<Electrode<D>> electrodes {256, time};

    // 256 raw data sources, bandpass filters
    std::array<EventSource<D, double>, N> raw;
   
    for (raw_size_t i = 0; i < N; i++)
        raw[i] = MakeEventSource<D, double>();

    //BandPassFilter<D> filter(time, raw[0]);
    std::vector<BandPassFilter<D>> filters;
    for (auto &r :raw)
        filters.emplace_back(time, r);

    //std::cout << "Filter N: " << filters.size() << std::endl;

    // Are merged into 1 TT
    using Tetrode = NTrode<D,
                           Events<D, double>,
                           Events<D, double>,
                           Events<D, double>,
                           Events<D, double>>;

    //Tetrode tt(time,
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

    // /////////////////////
    // HARDWARE SIMULATION
    // This is simulating 100 transactions hardware
    for (int i = 0; i < 100; i++) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(0, 1);

        // Make a sample block coming from 4 contacts
        for (int j = 0; j < BLOCKSIZE; j++) {
        
            // Graph update step
            DoTransaction<D>([&] {

                // Increment sample counter
                time.NewSample(); 

                // Make noise on all electrodes
                for (auto &r : raw)
                    r << d(gen);
            });
        }
    }

    return 0;
}
