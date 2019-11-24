#include <aws/core/Aws.h>
#include "Tensorizer.h"

#include <chrono>

using namespace mx::contrib;

int main(int argc, char **argv) {
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  Aws::InitAPI(options);

  Tensorizer nd_array_fetcher;

  for (int i = 0; i < 100; i++) {
    const Aws::String bucket_name = "random-nd-array-bucket";
    const Aws::String object_name = "rand.ndarray";
    nd_array_fetcher.push_s3_object(bucket_name.c_str(), object_name.c_str());
  }

  std::this_thread::sleep_for(std::chrono::seconds(300));

  Aws::ShutdownAPI(options);
}
