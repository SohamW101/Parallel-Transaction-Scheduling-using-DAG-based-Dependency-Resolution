# execution/parallel_executor.py
"""
Parallel execution engine for transaction shards using multiprocessing
"""

import multiprocessing as mp
import time
from typing import Dict, List, Tuple
from execution.smart_contracts import SmartContractExecutor

class TransactionExecutor:
    """Executes transactions on simulated account state"""
    
    def __init__(self):
        self.state = {}  # account_address -> balance
    
    def execute_transaction(self, tx, state: Dict) -> Tuple[bool, Dict]:
        """
        Execute a single transaction
        
        Args:
            tx: Transaction object
            state: Current account state
        
        Returns:
            (success: bool, updated_state: Dict)
        """
        updated_state = state.copy()
        
        try:
            # Initialize accounts if not exist
            if tx.from_address not in updated_state:
                updated_state[tx.from_address] = 1000.0
            if tx.to_address not in updated_state:
                updated_state[tx.to_address] = 0.0
            
            if tx.is_contract_transaction:
                # Execute smart contract
                executor = SmartContractExecutor()
                result = executor.execute(
                    tx.contract_function,
                    tx.contract_params,
                    updated_state
                )
                if not result:
                    return False, updated_state
            else:
                # Execute monetary transaction
                if updated_state[tx.from_address] < tx.value:
                    return False, updated_state
                
                updated_state[tx.from_address] -= tx.value
                updated_state[tx.to_address] += tx.value
            
            return True, updated_state
        
        except Exception as e:
            print(f"Transaction execution failed: {e}")
            return False, updated_state
    
    def execute_shard(self, shard: List, initial_state: Dict) -> Dict:
        """
        Execute all transactions in a shard sequentially
        
        Args:
            shard: List of transactions in order
            initial_state: Initial account state
        
        Returns:
            Updated state after all transactions
        """
        current_state = initial_state.copy()
        
        for tx in shard:
            success, current_state = self.execute_transaction(tx, current_state)
            if not success:
                print(f"Warning: Transaction {tx.tx_hash[:8]} failed")
        
        return current_state

class ParallelExecutor:
    """Coordinates parallel execution using multiprocessing"""
    
    @staticmethod
    def execute_parallel(follower_loads: Dict[int, List], 
                        initial_state: Dict,
                        num_processes: int = 4) -> Tuple[Dict, float]:
        """
        Execute follower loads in parallel
        
        Args:
            follower_loads: Dictionary {follower_id: [transactions]}
            initial_state: Initial account state
            num_processes: Number of parallel processes
        
        Returns:
            (final_state: Dict, execution_time: float)
        """
        start_time = time.time()
        
        # Use process pool
        with mp.Pool(processes=num_processes) as pool:
            executor = TransactionExecutor()
            
            # Execute shards in parallel
            results = []
            for follower_id, shard in follower_loads.items():
                result = pool.apply_async(
                    executor.execute_shard,
                    (shard, initial_state)
                )
                results.append(result)
            
            # Collect results
            follower_states = []
            for result in results:
                state = result.get()
                follower_states.append(state)
        
        execution_time = time.time() - start_time
        
        # Merge states from all followers
        final_state = initial_state.copy()
        for state in follower_states:
            final_state.update(state)
        
        return final_state, execution_time
    
    @staticmethod
    def execute_serial(transactions: List, 
                      initial_state: Dict) -> Tuple[Dict, float]:
        """
        Execute transactions serially (baseline for comparison)
        
        Args:
            transactions: List of transactions in order
            initial_state: Initial account state
        
        Returns:
            (final_state: Dict, execution_time: float)
        """
        start_time = time.time()
        
        executor = TransactionExecutor()
        final_state = executor.execute_shard(transactions, initial_state)
        
        execution_time = time.time() - start_time
        
        return final_state, execution_time
