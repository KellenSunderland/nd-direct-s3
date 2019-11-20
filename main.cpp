#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>

#define MSHADOW_USE_CUDA 0
#define MSHADOW_USE_MKL 0
#include <mxnet/ndarray.h>

#include <fstream>

const int NUM_THREADS = 16;
const int DOWNLOAD_ITERATIONS = 10;
std::atomic<int> thread_ids;
std::atomic<int> files_downloaded;

void download_loop() {
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
    std::cout << "THREAD: " << thread_id << " downloading new file"
              << std::endl;
    auto get_object_outcome = s3_client.GetObject(object_request);
    if (get_object_outcome.IsSuccess()) {
      auto &retrieved_file =
          get_object_outcome.GetResultWithOwnership().GetBody();
      std::vector<char> output_ndarray_bytes;
      output_ndarray_bytes.reserve(163840080);
      retrieved_file.read(output_ndarray_bytes.data(), 163840080);
      // Todo: load from stream using mxnet::NDArray::Load();
      // Reference: https://github.com/dmlc/dmlc-core/blob/master/src/io/s3_filesys.cc
      files_downloaded.fetch_add(1);
      std::cout << "THREAD: " << thread_id << " download finished" << std::endl;
    } else {
      const auto &error = get_object_outcome.GetError();
      std::cout << "THREAD: " << thread_id
                << " ERROR: " << error.GetExceptionName() << ": "
                << error.GetMessage() << std::endl;
    }
  }
}

int main(int argc, char **argv) {
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  Aws::InitAPI(options);
  {
    std::vector<std::thread> download_threads;
    download_threads.reserve(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
      download_threads.emplace_back(std::thread(download_loop));
    }

    std::for_each(download_threads.begin(), download_threads.end(),
                  [](auto &t) { t.join(); });
    std::cout << "Total downloaded files: " << files_downloaded << std::endl;
  }
  Aws::ShutdownAPI(options);
}
