#include "Tensorizer.h"
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <mxnet-cpp/MxNetCpp.h>
#include <mxnet-cpp/ndarray.h>

namespace mx::contrib {

Tensorizer::Tensorizer() {
  std::vector<std::thread> download_threads;
  download_threads.reserve(NUM_THREADS);
  for (int i = 0; i < NUM_THREADS; i++) {
    download_threads.emplace_back(std::thread(&Tensorizer::download_loop, this));
  }
  std::for_each(download_threads.begin(), download_threads.end(), [](auto &t) { t.join(); });
}

void Tensorizer::download_loop() {
  int thread_id = thread_ids.fetch_add(1);
  std::cout << "Download thread " << thread_id << " started" << std::endl;
  const Aws::String bucket_name = "random-nd-array-bucket";
  const Aws::String object_name = "rand.ndarray";
  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.region = "us-west-2";
  Aws::S3::S3Client s3_client(clientConfig);
  for (int i = 0; i < DOWNLOAD_ITERATIONS; i++) {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(bucket_name);
    object_request.SetKey(object_name);
    std::cout << "THREAD: " << thread_id << " downloading new file" << std::endl;
    auto get_object_outcome = s3_client.GetObject(object_request);
    if (get_object_outcome.IsSuccess()) {
      int file_size = get_object_outcome.GetResult().GetContentLength();
      auto &retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();
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
      }

      files_downloaded.fetch_add(1);
      std::cout << "THREAD: " << thread_id << " download finished" << std::endl;
    } else {
      const auto &error = get_object_outcome.GetError();
      std::cout << "THREAD: " << thread_id << " ERROR: " << error.GetExceptionName() << ": "
                << error.GetMessage() << std::endl;
    }
  }
}

}  // namespace mx::contrib
