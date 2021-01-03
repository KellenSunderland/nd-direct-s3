import boto3

from functools import lru_cache

@lru_cache(maxsize=None)
def get_s3_client():
    return boto3.client('s3', region_name='us-west-2')
