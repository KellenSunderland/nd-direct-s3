#include <aws/core/Aws.h>
#include "Tensorizer.h"

#include <chrono>

using namespace mx::contrib;

int main(int argc, char **argv) {
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
  Aws::InitAPI(options);

  Tensorizer nd_array_fetcher;
  // const std::string bucket_name = "kellen-ndarray-test-bucket";
  const std::string bucket_name = "kellen-small-ndarray-test-bucket";

  const std::string model_name = "test_model";

  for (int i = 0; i < 1; i++) {
    for (int v = 0; v < 1; v++) {
      std::string object_name(model_name + "_" + std::to_string(i) + "_" + std::to_string(v));
      nd_array_fetcher.queue_ndarray(bucket_name.c_str(), object_name.c_str());
    }
  }
  std::cout << "Finished queueing tensors" << std::endl;

  for (int i = 0; i < 1; i++) {
    NDArrayHandle handle = nd_array_fetcher.get_next_ndarray();
    std::cout << "Finished downloading tensor # " << i << std::endl;
    // Todo: cleanup handle here once ownership clearer.
    // delete handle;
  }

  Aws::ShutdownAPI(options);
}
