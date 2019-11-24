#pragma once

#include <mxnet/c_api.h>

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

namespace mx::contrib {

const int NUM_THREADS = 2;

class Tensorizer {
 public:
  Tensorizer();
  ~Tensorizer();
  void push_s3_object(const char* s3_bucket, const char* s3_object);
  bool has_preloaded_tensor();
  NDArrayHandle* get_next_ndarray();

 private:
  void download_loop();

  std::atomic<int> m_thread_ids;
  std::vector<std::thread> m_download_threads;
  std::queue<std::pair<std::string, std::string>> m_s3_input_queue;
  std::queue<NDArrayHandle*> m_ndarray_output_queue;
  std::mutex m_mutex;
  bool is_running = true;
};
}  // namespace mx::contrib
