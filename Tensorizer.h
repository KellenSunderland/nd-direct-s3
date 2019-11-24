#pragma once

#include <mxnet/c_api.h>
#include <fstream>
#include <atomic>

namespace mx::contrib {

const int NUM_THREADS = 20;
const int DOWNLOAD_ITERATIONS = 1000;

class Tensorizer {
    public:
        Tensorizer();
        void push_s3_uri(const char* uri);
        NDArrayHandle* get_next_array();

    private:
        void download_loop();

        std::atomic<int> thread_ids;
        std::atomic<int> files_downloaded;
};
} // mx::contrib
