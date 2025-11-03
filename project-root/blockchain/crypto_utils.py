# blockchain/crypto_utils.py
"""Cryptographic utilities for blockchain operations"""

import hashlib
import json
from typing import Any

def hash_data(data: Any) -> str:
    """
    Hash any data using SHA-256
    
    Args:
        data: Data to hash (will be JSON serialized if dict/list)
    
    Returns:
        Hex string of SHA-256 hash
    """
    if isinstance(data, (dict, list)):
        data_str = json.dumps(data, sort_keys=True)
    else:
        data_str = str(data)
    
    return hashlib.sha256(data_str.encode()).hexdigest()

def verify_hash(data: Any, hash_value: str) -> bool:
    """Verify if computed hash matches expected hash"""
    return hash_data(data) == hash_value

def generate_address(seed: str) -> str:
    """Generate a blockchain address from a seed string"""
    hash_obj = hashlib.sha256(seed.encode())
    return '0x' + hash_obj.hexdigest()[:40]

def mine_hash(block_data: dict, difficulty: int) -> tuple:
    """
    Simple Proof-of-Work: find nonce such that hash has 'difficulty' leading zeros
    
    Args:
        block_data: Block data to mine
        difficulty: Number of leading zeros required
    
    Returns:
        (nonce, hash_value) that satisfies PoW requirement
    """
    target = '0' * difficulty
    nonce = 0
    
    while True:
        block_data['nonce'] = nonce
        hash_value = hash_data(block_data)
        
        if hash_value.startswith(target):
            return nonce, hash_value
        
        nonce += 1
        
        # For testing: limit iterations to prevent infinite loops
        if nonce > 10_000_000:
            # Return best effort
            return nonce, hash_value
