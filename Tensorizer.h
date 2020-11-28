#pragma once

#include <mxnet/c_api.h>

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

namespace mx::contrib {

const int NUM_THREADS = 20;

// Tensorizer allows you to load ND Arrays directly from S3 to GPU tensors. It uses background
// threads to pre-emptively download a fixed number of arrays so that new arrays are always
// retrieved quickly. It follows a first in first out ordering. A user first calls queue_ndarray for
// the arrays they're interested in. They can then retrieve arrays in order with get_next_ndarray.

class Tensorizer {
 public:
  Tensorizer();
  ~Tensorizer();

  // This call queues an S3 object for download when there's an availible thread. Once downloaded
  // the tensor can be retured with get_next_ndarray. Arrays will be returned in the same order they
  // are enqueued.
  void queue_ndarray(const char* s3_bucket, const char* s3_object);

  // Lets the caller know if an array has been downloaded, processed, and is ready to be returned.
  bool has_preloaded_tensor();

  // Blocking call to return the next NDArray as soon as it is ready. This call returns arrays in
  // the same order they are passed into queue_ndarray.
  NDArrayHandle get_next_ndarray();

 private:
  void download_loop();

  std::atomic<int> m_thread_ids{};
  std::vector<std::thread> m_download_threads;
  std::queue<std::pair<std::string, std::string>> m_s3_input_queue;
  std::queue<NDArrayHandle> m_ndarray_output_queue;
  std::mutex m_mutex;
  bool m_is_running = true;
};
}  // namespace mx::contrib
