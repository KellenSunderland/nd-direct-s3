#include <aws/core/Aws.h>
#include "Tensorizer.h"

using namespace mx::contrib;

int main(int argc, char **argv) {
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  Aws::InitAPI(options);

  Tensorizer nd_array_fetcher;

  Aws::ShutdownAPI(options);
}
