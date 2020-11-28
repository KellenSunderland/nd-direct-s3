#include "Tensorizer.h"
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <mxnet-cpp/MxNetCpp.h>
#include <mxnet-cpp/ndarray.h>

namespace mx::contrib {

Tensorizer::Tensorizer() {
  m_download_threads.reserve(NUM_THREADS);
  for (int i = 0; i < NUM_THREADS; i++) {
    m_download_threads.emplace_back(std::thread(&Tensorizer::download_loop, this));
  }
}

Tensorizer::~Tensorizer() {
  if (m_is_running) {
    m_is_running = false;
    std::for_each(m_download_threads.begin(), m_download_threads.end(), [](auto& t) { t.join(); });
  }
}

void Tensorizer::queue_ndarray(const char* s3_bucket, const char* s3_object) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_s3_input_queue.push(std::pair(s3_bucket, s3_object));
  }
}

bool Tensorizer::has_preloaded_tensor() {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_ndarray_output_queue.size() > 0;
}

NDArrayHandle Tensorizer::get_next_ndarray() {
  while (m_ndarray_output_queue.size() == 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_ndarray_output_queue.size() > 0) {
      NDArrayHandle handle = m_ndarray_output_queue.front();
      m_ndarray_output_queue.pop();
      std::cout << "Returning downloaded array" << std::endl;
      return handle;
    }
  }
  return get_next_ndarray();
}

void Tensorizer::download_loop() {
  int thread_id = m_thread_ids.fetch_add(1);
  std::cout << "Download thread " << thread_id << " started" << std::endl;

  while (m_is_running) {
    while (m_s3_input_queue.size() == 0 || m_ndarray_output_queue.size() > 100) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // TODO: Signal to wake up
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "us-west-2";
    Aws::S3::S3Client s3_client(clientConfig);
    std::pair<std::string, std::string> s3_object_location;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_s3_input_queue.size() > 0) {
        s3_object_location = m_s3_input_queue.front();
        m_s3_input_queue.pop();
        std::cout << "Downloading new file - " << s3_object_location.first << s3_object_location.second <<std::endl;
      } else {
        std::cout << "no items in queue, waiting" << std::endl;
        continue;
      }
    }

    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(Aws::String(s3_object_location.first));
    object_request.SetKey(Aws::String(s3_object_location.second));
    std::cout << "THREAD: " << thread_id << " downloading new file" << std::endl;
    auto get_object_outcome = s3_client.GetObject(object_request);
    if (get_object_outcome.IsSuccess()) {
      int file_size = get_object_outcome.GetResult().GetContentLength();
      auto& retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();
      // TODO: move this to the heap and transfer ownership out of code block.
      std::vector<char> output_ndarray_bytes;
      output_ndarray_bytes.reserve(file_size);
      retrieved_file.read(output_ndarray_bytes.data(), file_size);
      auto data_arrays =
          mxnet::cpp::NDArray::LoadFromBufferToList(output_ndarray_bytes.data(), file_size);

      std::cout << "Downloaded " << data_arrays.size() << " arrays" << std::endl;
      for (auto data_array : data_arrays) {
        std::cout << "Array shape is: ";
        for (auto i : data_array.GetShape()) {
          std::cout << " " << i;
        }
        std::cout << std::endl;
        std::lock_guard<std::mutex> lock(m_mutex);
        m_ndarray_output_queue.push(data_array.GetHandle());
      }

      std::cout << "THREAD: " << thread_id << " download finished" << std::endl;
    } else {
      const auto& error = get_object_outcome.GetError();
      std::cout << "THREAD: " << thread_id << " ERROR: " << error.GetExceptionName() << ": "
                << error.GetMessage() << std::endl;
    }
  }
}

}  // namespace mx::contrib
