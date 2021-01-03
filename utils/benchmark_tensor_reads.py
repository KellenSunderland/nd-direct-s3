import boto3
import logging
import numpy as np
import sys
sys.path.append('/workspaces/nd-direct-s3/tensorizer/python')

import aws_clients
import tensor_constants
from tensorizer import Tensorizer

from functools import lru_cache

logging.basicConfig(level=logging.INFO)


def main():
    tensorizer = Tensorizer()
    for epoch_index in range(0, 1):
        for minibatch_index in range(0, 1):
            s3 = aws_clients.get_s3_client()
            tensor_key = tensor_constants.MODEL_NAME + "_" + str(epoch_index) + "_" + str(minibatch_index)
            tensorizer.queue_ndarray(tensor_constants.BUCKET_NAME ,tensor_key)

    logging.info("Hello World")


if __name__=="__main__":
    main()
