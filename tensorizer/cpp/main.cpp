#include <aws/core/Aws.h>
#include "Tensorizer.h"

#include <chrono>

using namespace mx::contrib;

int main(int argc, char **argv) {
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  Aws::InitAPI(options);

  Tensorizer nd_array_fetcher;

  for (int i = 0; i < 10000; i++) {
    const Aws::String bucket_name = "nd-array-test-3";
    std::string object_name("random-tensors/" + std::to_string(i) + ".ndarray");
    nd_array_fetcher.queue_ndarray(bucket_name.c_str(), object_name.c_str());
  }

  for (int i = 0; i < 10000; i++) {
    NDArrayHandle handle = nd_array_fetcher.get_next_ndarray();
    std::cout << "Finished downloading tensor # " << i << std::endl;
    // Todo: cleanup handle here once ownership clearer.
    // delete handle;
  }

  Aws::ShutdownAPI(options);
}
