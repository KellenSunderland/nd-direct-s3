import boto3
import numpy as np
import logging

from functools import lru_cache

logging.basicConfig(level=logging.INFO)

TENSOR_SHAPE = (512, 3, 480, 640)
BUCKET_NAME = "kellen-ndarray-test-bucket"
MODEL_NAME = "test_model"

@lru_cache(maxsize=None)
def get_s3_client():
    return boto3.client('s3', region_name='us-west-2')


def generate_tensor():
    logging.info("Creating new random tensor of shape: " + str(TENSOR_SHAPE))
    dtype = np.float32
    type_max = 1 << np.finfo(dtype).nmant
    new_tensor = np.empty(TENSOR_SHAPE, dtype=dtype)
    new_tensor[...] = np.random.randint(0, type_max, size=TENSOR_SHAPE) / dtype(type_max)
    logging.info("Finished tensor creation")
    return new_tensor


def create_bucket_if_needed():
    s3 = get_s3_client()
    try:
        location = {'LocationConstraint': "us-west-2"}
        s3.create_bucket(Bucket=BUCKET_NAME, CreateBucketConfiguration=location)
    except Exception as e:
        logging.exception("Error while creating bucket")
        pass


def main():
    create_bucket_if_needed()
    for epoch_index in range(0, 10):
        for minibatch_index in range(0, 100):
            s3 = get_s3_client()
            tensor = generate_tensor()
            tensor_key = MODEL_NAME + "_" + str(epoch_index) + "_" + str(minibatch_index)
            s3.put_object(Body=tensor.data.tobytes(), Bucket=BUCKET_NAME, Key=tensor_key)
            print("Todo - Something with tensor key: " + tensor_key)


if __name__=="__main__":
    main()
