#include "TensorizerCInterface.h"

tensorizer_handle_t construct() {
    return new mx::contrib::Tensorizer();
}

void destruct(tensorizer_handle_t handle) {
    delete handle;
}

void queue_ndarray(tensorizer_handle_t handle, const char* s3_bucket, const char* s3_object) {
    handle->queue_ndarray(s3_bucket, s3_object);
}

bool has_preloaded_tensor(tensorizer_handle_t handle) {
    return handle->has_preloaded_tensor();
}

NDArrayHandle get_next_ndarray(tensorizer_handle_t handle) {
    return handle->get_next_ndarray();
}
