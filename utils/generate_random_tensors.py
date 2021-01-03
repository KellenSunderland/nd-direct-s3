import boto3
import numpy as np
import logging

import tensor_constants
import aws_clients

logging.basicConfig(level=logging.INFO)


def generate_tensor():
    logging.info("Creating new random tensor of shape: " + str(tensor_constants.TENSOR_SHAPE_SMALL))
    dtype = np.float32
    type_max = 1 << np.finfo(dtype).nmant
    new_tensor = np.empty(tensor_constants.TENSOR_SHAPE_SMALL, dtype=dtype)
    new_tensor[...] = np.random.randint(0, type_max, size=tensor_constants.TENSOR_SHAPE_SMALL) / dtype(type_max)
    logging.info("Finished tensor creation")
    return new_tensor


def create_bucket_if_needed():
    s3 = aws_clients.get_s3_client()
    try:
        location = {'LocationConstraint': "us-west-2"}
        s3.create_bucket(Bucket=tensor_constants.BUCKET_NAME, CreateBucketConfiguration=location)
    except Exception as e:
        logging.exception("Error while creating bucket")
        pass


def main():
    create_bucket_if_needed()
    for epoch_index in range(0, 10):
        for minibatch_index in range(0, 100):
            s3 = aws_clients.get_s3_client()
            tensor = generate_tensor()
            tensor_key = tensor_constants.MODEL_NAME + "_" + str(epoch_index) + "_" + str(minibatch_index)
            s3.put_object(Body=tensor.data.tobytes(), Bucket=tensor_constants.BUCKET_NAME, Key=tensor_key)
            print("Todo - Something with tensor key: " + tensor_key)


if __name__=="__main__":
    main()
