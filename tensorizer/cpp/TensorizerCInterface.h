// Interface to expose Tensorizer to Python code.

#include <mxnet/c_api.h>
#include "Tensorizer.h"

#ifndef TENSORIZER_C_INTERFACE
#define TENSORIZER_C_INTERFACE

typedef mx::contrib::Tensorizer* tensorizer_handle_t;

extern "C" {

  // Create a Tensorizer instance.
  tensorizer_handle_t tensorizer_construct();

  // Cleanup a Tensorizer instance.
  void tensorizer_destruct(tensorizer_handle_t handle);

  // This call queues an S3 object for download when there's an availible thread. Once downloaded
  // the tensor can be retured with get_next_ndarray. Arrays will be returned in the same order they
  // are enqueued.
  void queue_ndarray(tensorizer_handle_t handle, const char* s3_bucket, const char* s3_object);

  // Lets the caller know if an array has been downloaded, processed, and is ready to be returned.
  bool has_preloaded_tensor(tensorizer_handle_t handle);

  // Blocking call to return the next NDArray as soon as it is ready. This call returns arrays in
  // the same order they are passed into queue_ndarray.
  NDArrayHandle get_next_ndarray(tensorizer_handle_t handle);

}

#endif // TENSORIZER_C_INTERFACE
