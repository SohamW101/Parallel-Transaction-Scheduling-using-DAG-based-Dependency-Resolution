# execution/dependency_analyzer.py
"""
Analyzes transaction dependencies and creates shards.
Core algorithm: Weakly Connected Components (WCC) on account-transaction graph
"""

import networkx as nx
from typing import Dict, List, Set, Tuple
from collections import defaultdict

class DependencyAnalyzer:
    """
    Analyzes transaction dependencies using graph theory
    - Vertices: Accounts (addresses)
    - Edges: Transactions (connect accounts they access)
    - Components: Shards (groups of dependent transactions)
    """
    
    def __init__(self):
        self.graph = nx.Graph()
        self.transaction_map = {}  # Map account pairs to transactions
    
    def build_dependency_graph(self, transactions: List) -> Tuple[nx.Graph, Dict]:
        """
        Build account-transaction dependency graph
        
        Args:
            transactions: List of Transaction objects
        
        Returns:
            (graph, transaction_map) where transaction_map[account] = [tx_hashes]
        """
        self.graph = nx.Graph()
        self.transaction_map = defaultdict(list)
        
        for tx in transactions:
            accounts = tx.accessed_accounts
            
            # Add nodes for all accounts
            for account in accounts:
                if account not in self.graph:
                    self.graph.add_node(account)
            
            # Connect accounts accessed by same transaction
            if len(accounts) > 1:
                accounts_list = list(accounts)
                for i in range(len(accounts_list)):
                    for j in range(i + 1, len(accounts_list)):
                        # Edge represents a transaction connecting two accounts
                        self.graph.add_edge(accounts_list[i], accounts_list[j],
                                          tx_hash=tx.tx_hash)
            
            # Map transaction to accounts
            for account in accounts:
                self.transaction_map[account].append(tx.tx_hash)
        
        return self.graph, dict(self.transaction_map)
    
    def find_shards(self, transactions: List) -> Dict[int, List]:
        """
        Find Weakly Connected Components (shards) from transactions
        
        Algorithm:
        1. Build dependency graph
        2. Find all WCC using NetworkX
        3. Group transactions by their component
        4. Return shards with transactions in original order
        
        Args:
            transactions: List of Transaction objects (in order)
        
        Returns:
            Dictionary {shard_id: [tx1, tx2, ...]} where transactions within
            a shard are in original order
        """
        self.build_dependency_graph(transactions)
        
        # Find weakly connected components
        wcc_list = list(nx.connected_components(self.graph))
        
        # Create transaction to shard mapping
        account_to_shard = {}
        for shard_id, component in enumerate(wcc_list):
            for account in component:
                account_to_shard[account] = shard_id
        
        # Group transactions into shards (preserving order)
        shards = defaultdict(list)
        for tx in transactions:
            # Find which shard this transaction belongs to
            accounts = tx.accessed_accounts
            if accounts:
                shard_id = account_to_shard[next(iter(accounts))]
                shards[shard_id].append(tx)
        
        return dict(shards)
    
    def get_shard_statistics(self, shards: Dict[int, List]) -> Dict:
        """Compute statistics about shards"""
        if not shards:
            return {}
        
        shard_sizes = [len(shard) for shard in shards.values()]
        
        return {
            'num_shards': len(shards),
            'min_shard_size': min(shard_sizes),
            'max_shard_size': max(shard_sizes),
            'avg_shard_size': sum(shard_sizes) / len(shard_sizes),
            'total_transactions': sum(shard_sizes),
        }

class LoadBalancer:
    """
    Distributes shards among followers for load balancing
    Uses bin packing algorithm for efficient distribution
    """
    
    @staticmethod
    def balance_shards(shards: Dict[int, List], num_followers: int) -> Dict[int, List]:
        """
        Distribute shards to followers with load balancing
        
        Algorithm: First Fit Decreasing (FFD) bin packing
        1. Sort shards by size (descending)
        2. Assign each shard to follower with least load
        
        Args:
            shards: Dictionary of shards {shard_id: [transactions]}
            num_followers: Number of follower workers
        
        Returns:
            Dictionary {follower_id: [transactions]}
        """
        if num_followers <= 0:
            raise ValueError("Number of followers must be positive")
        
        # Sort shards by size (descending)
        sorted_shards = sorted(shards.items(), 
                             key=lambda x: len(x[1]), 
                             reverse=True)
        
        # Initialize follower loads
        follower_loads = [[] for _ in range(num_followers)]
        follower_sizes = [0] * num_followers
        
        # Assign each shard to follower with minimum load
        for shard_id, shard_txs in sorted_shards:
            min_idx = follower_sizes.index(min(follower_sizes))
            follower_loads[min_idx].extend(shard_txs)
            follower_sizes[min_idx] += len(shard_txs)
        
        return {i: txs for i, txs in enumerate(follower_loads) if txs}
    
    @staticmethod
    def get_load_balance_score(follower_loads: Dict[int, List]) -> float:
        """
        Calculate load balance score (0-100, higher is better)
        Perfect balance = 100, completely unbalanced = 0
        """
        if not follower_loads:
            return 0.0
        
        loads = [len(txs) for txs in follower_loads.values()]
        if not loads or len(loads) == 0:
            return 100.0
        
        avg_load = sum(loads) / len(loads)
        if avg_load == 0:
            return 100.0
        
        # Calculate coefficient of variation
        variance = sum((x - avg_load) ** 2 for x in loads) / len(loads)
        std_dev = variance ** 0.5
        cv = std_dev / avg_load if avg_load > 0 else 0
        
        # Convert to score (0-100)
        score = max(0, 100 * (1 - cv))
        return min(100, score)
