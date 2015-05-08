/*
 * ============================================================================
 *
 *       Filename:  qc-processor.hh
 *    Description:  Trim GBS reads using the GBSTrimPE processor.
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <iostream>
#include <string>


#include "qcpp.hh"

#include "qc-gbs.hh"

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

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    stream.append_processor<qcpp::GBSTrimPE>("trim Pst1 reads", "CTGCAG", 1);

    std::thread threads[4];
    for (size_t i = 0; i < 4; i++) {
        threads[i] = std::thread(parse_and_print, &stream);
    }
    for (size_t i = 0; i < 4; i++) {
        threads[i].join();
    }
    std::cerr << stream.report();
    return EXIT_SUCCESS;
}
#else

int
main (int argc, char *argv[])
{
    qcpp::ReadPair rp;
    qcpp::ProcessedReadStream stream;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    stream.append_processor<qcpp::GBSTrimPE>("trim Pst1 reads", "CTGCAG", 1);

    while (stream.parse_read_pair(rp)) {
        std::cout << rp.str();
    }
    std::cerr << stream.report();
    return EXIT_SUCCESS;
}

#endif
