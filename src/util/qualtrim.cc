/*
 * ============================================================================
 *
 *       Filename:  trim.cc
 *    Description:  Trim reads using the windowed quality trimming algorithm
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <iostream>
#include <string>

#include "qcpp.hh"
#include "qc-qualtrim.hh"
#include "qc-measure.hh"

#if 0
#include <thread>
std::mutex _cout_mutex;

void parse_and_print(qcpp::ProcessedReadStream *stream)
{
    qcpp::ReadPair rp;
    bool not_at_end = stream->parse_read_pair(rp);
    while (not_at_end) {
        std::string rp_str = rp.str();
        {
            std::lock_guard<std::mutex> lg(_cout_mutex);
            std::cout << rp_str;
        }
        not_at_end = stream->parse_read_pair(rp);
    }
}

int
main (int argc, char *argv[])
{
    qcpp::ProcessedReadStream stream;
    std::vector<std::thread> threads;
    unsigned int n_threads = std::thread::hardware_concurrency() - 1;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    //stream.append_processor<qcpp::PerBaseQuality>("Before QC");
    stream.append_processor<qcpp::WindowedQualTrim>("Qual Trim", 33, 20, 4);
    //stream.append_processor<qcpp::PerBaseQuality>("after qc");


    for (size_t i = 0; i < n_threads; i++) {
        threads.push_back(std::thread(parse_and_print, &stream));
    }
    for (size_t i = 0; i < n_threads; i++) {
        threads[i].join();
    }
    std::cerr << stream.report();
    return EXIT_SUCCESS;
}

#else
int
main (int argc, char *argv[])
{
    qcpp::ProcessedReadStream stream;
    qcpp::ReadPair rp;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    //stream.append_processor<qcpp::PerBaseQuality>("Before QC");
    stream.append_processor<qcpp::WindowedQualTrim>("Qual Trim", 33, 20, 4);
    //stream.append_processor<qcpp::PerBaseQuality>("after qc");

    while (stream.parse_read_pair(rp)) {
        std::cout << rp.str();
    }

    std::cerr << stream.report();
    return EXIT_SUCCESS;
}
#endif
