import boto3
import numpy as np

from functools import lru_cache

TENSOR_SHAPE = (1024, 3, 480, 640)
BUCKET_NAME = "ndarray-test-bucket"

@lru_cache(maxsize=None)
def get_s3_client():
    return boto3.client('s3')


def generate_tensor():
    new_tensor = np.random.random_sample(TENSOR_SHAPE)
    return new_tensor


def create_bucket_if_needed():
    s3 = get_s3_client()
    try:
        s3.create_bucket(BucketBUCKET_NAME)
    except:
        pass


def main():
    create_bucket_if_needed()
    for i in range(0, 3):
        s3 = get_s3_client()
        tensor = generate_tensor()
        print("Todo - Something with this tensor")


if __name__=="__main__":
    main()
