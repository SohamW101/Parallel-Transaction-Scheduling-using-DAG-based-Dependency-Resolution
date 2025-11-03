# blockchain/transaction.py
"""Transaction class for blockchain"""

from typing import Optional, Dict, Any
import time
from blockchain.crypto_utils import hash_data, generate_address

class Transaction:
    """
    Represents a blockchain transaction
    
    Two types:
    1. Monetary: Simple value transfer between accounts
    2. Smart Contract: Execute contract function with parameters
    """
    
    def __init__(self, tx_from: str, tx_to: str, value: float = 0.0,
                 contract_function: Optional[str] = None,
                 contract_params: Optional[Dict] = None,
                 gas_limit: int = 21000):
        """
        Initialize a transaction
        
        Args:
            tx_from: Sender address
            tx_to: Recipient address
            value: Amount transferred (in ether units)
            contract_function: Smart contract function name (if contract call)
            contract_params: Parameters for contract function
            gas_limit: Gas limit for transaction
        """
        self.from_address = tx_from
        self.to_address = tx_to
        self.value = value
        self.contract_function = contract_function
        self.contract_params = contract_params or {}
        self.gas_limit = gas_limit
        self.timestamp = time.time()
        self.nonce = 0
        
        # Compute transaction hash
        self.tx_hash = self._compute_hash()
    
    def _compute_hash(self) -> str:
        """Compute transaction hash"""
        tx_data = {
            'from': self.from_address,
            'to': self.to_address,
            'value': self.value,
            'function': self.contract_function,
            'params': self.contract_params,
            'timestamp': self.timestamp,
            'nonce': self.nonce
        }
        return hash_data(tx_data)
    
    @property
    def is_contract_transaction(self) -> bool:
        """Check if this is a smart contract transaction"""
        return self.contract_function is not None
    
    @property
    def accessed_accounts(self) -> set:
        """Get set of accounts accessed by this transaction"""
        accounts = {self.from_address, self.to_address}
        # For contract calls, also include any accounts in parameters
        if self.contract_params:
            for value in self.contract_params.values():
                if isinstance(value, str) and value.startswith('0x'):
                    accounts.add(value)
        return accounts
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert transaction to dictionary"""
        return {
            'tx_hash': self.tx_hash,
            'from': self.from_address,
            'to': self.to_address,
            'value': self.value,
            'function': self.contract_function,
            'params': self.contract_params,
            'gas_limit': self.gas_limit,
            'timestamp': self.timestamp,
            'is_contract': self.is_contract_transaction
        }
    
    def __repr__(self) -> str:
        tx_type = "Contract" if self.is_contract_transaction else "Transfer"
        return (f"Transaction({tx_type} {self.tx_hash[:8]}... "
                f"{self.from_address[:6]}→{self.to_address[:6]} "
                f"value={self.value})")
