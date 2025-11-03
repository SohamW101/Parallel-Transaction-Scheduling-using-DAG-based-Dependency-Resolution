# experiments/benchmark.py
"""
Benchmark DiPETrans framework - compare serial vs parallel execution
"""

import csv
import json
import time
import statistics
from typing import Dict, List, Tuple
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).parent.parent))

from blockchain.transaction import Transaction
from blockchain.crypto_utils import generate_address
from execution.dependency_analyzer import DependencyAnalyzer, LoadBalancer
from execution.parallel_executor import ParallelExecutor

class Benchmark:
    """Benchmarking framework for DiPETrans"""
    
    def __init__(self, output_dir: str = 'results'):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.results = []
    
    def load_transactions_from_csv(self, csv_file: str) -> List[Transaction]:
        """Load transactions from CSV file"""
        transactions = []
        
        with open(csv_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                tx = Transaction(
                    tx_from=row['from_address'],
                    tx_to=row['to_address'],
                    value=float(row['value']),
                    contract_function=row.get('contract_function') if row.get('is_contract') == 'True' else None,
                    contract_params={} if row.get('is_contract') == 'True' else None,
                )
                transactions.append(tx)
        
        return transactions
    
    def run_benchmark(self, transactions: List[Transaction], 
                     num_followers_list: List[int] = [1, 2, 3, 4, 5],
                     iterations: int = 3) -> Dict:
        """
        Run benchmark comparing serial and parallel execution
        
        Args:
            transactions: List of Transaction objects
            num_followers_list: Different follower counts to test
            iterations: Number of iterations per configuration
        
        Returns:
            Dictionary of benchmark results
        """
        
        results = {
            'transactions': len(transactions),
            'contract_ratio': sum(1 for tx in transactions if tx.is_contract_transaction) / len(transactions),
            'runs': []
        }
        
        # Initialize account state
        initial_state = {}
        for tx in transactions:
            initial_state[tx.from_address] = 1000.0
            initial_state[tx.to_address] = 1000.0
        
        print("\n" + "="*60)
        print("DIPETRANS BENCHMARK")
        print("="*60)
        print(f"Transactions: {len(transactions)}")
        print(f"Contract transactions: {sum(1 for tx in transactions if tx.is_contract_transaction)}")
        print(f"Monetary transactions: {sum(1 for tx in transactions if not tx.is_contract_transaction)}")
        
        # Run serial execution baseline
        print(f"\nSerial Execution (baseline):")
        serial_times = []
        for i in range(iterations):
            _, exec_time = ParallelExecutor.execute_serial(transactions, initial_state)
            serial_times.append(exec_time)
            print(f"  Iteration {i+1}: {exec_time:.4f}s")
        
        serial_avg = statistics.mean(serial_times)
        serial_std = statistics.stdev(serial_times) if len(serial_times) > 1 else 0
        
        results['serial'] = {
            'avg_time': serial_avg,
            'std_dev': serial_std,
            'min_time': min(serial_times),
            'max_time': max(serial_times),
        }
        
        print(f"  Average: {serial_avg:.4f}s ± {serial_std:.4f}s")
        
        # Run parallel executions
        analyzer = DependencyAnalyzer()
        shards = analyzer.find_shards(transactions)
        shard_stats = analyzer.get_shard_statistics(shards)
        
        print(f"\nShard Analysis:")
        print(f"  Total shards: {shard_stats['num_shards']}")
        print(f"  Avg shard size: {shard_stats['avg_shard_size']:.2f} transactions")
        print(f"  Max shard size: {shard_stats['max_shard_size']} transactions")
        
        results['shards'] = shard_stats
        
        for num_followers in num_followers_list:
            print(f"\nParallel Execution with {num_followers} followers:")
            
            parallel_times = []
            for i in range(iterations):
                # Load balance shards
                follower_loads = LoadBalancer.balance_shards(shards, num_followers)
                
                # Execute in parallel
                _, exec_time = ParallelExecutor.execute_parallel(
                    follower_loads,
                    initial_state,
                    num_processes=num_followers
                )
                parallel_times.append(exec_time)
                print(f"  Iteration {i+1}: {exec_time:.4f}s")
            
            parallel_avg = statistics.mean(parallel_times)
            parallel_std = statistics.stdev(parallel_times) if len(parallel_times) > 1 else 0
            speedup = serial_avg / parallel_avg if parallel_avg > 0 else 0
            load_balance_score = LoadBalancer.get_load_balance_score(
                LoadBalancer.balance_shards(shards, num_followers)
            )
            
            print(f"  Average: {parallel_avg:.4f}s ± {parallel_std:.4f}s")
            print(f"  Speedup: {speedup:.2f}x")
            print(f"  Load balance score: {load_balance_score:.1f}%")
            
            results['runs'].append({
                'num_followers': num_followers,
                'avg_time': parallel_avg,
                'std_dev': parallel_std,
                'min_time': min(parallel_times),
                'max_time': max(parallel_times),
                'speedup': speedup,
                'throughput_improvement': (serial_avg - parallel_avg) / serial_avg * 100,
                'load_balance_score': load_balance_score,
            })
        
        return results
    
    def save_results(self, results: Dict, filename: str = 'benchmark_results.json'):
        """Save benchmark results to JSON file"""
        output_file = self.output_dir / filename
        
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Results saved to {output_file}")
        return output_file

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Run DiPETrans benchmark')
    parser.add_argument('--data', type=str, required=True, help='CSV file with transactions')
    parser.add_argument('--followers', type=int, nargs='+', default=[1,2,3,4,5],
                       help='Number of followers to test')
    parser.add_argument('--iterations', type=int, default=3, help='Iterations per configuration')
    parser.add_argument('--output', type=str, default='results', help='Output directory')
    
    args = parser.parse_args()
    
    # Run benchmark
    benchmark = Benchmark(output_dir=args.output)
    transactions = benchmark.load_transactions_from_csv(args.data)
    
    results = benchmark.run_benchmark(
        transactions,
        num_followers_list=args.followers,
        iterations=args.iterations
    )
    
    benchmark.save_results(results)
    
    # Print summary
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    for run in results['runs']:
        print(f"Followers: {run['num_followers']}, "
              f"Speedup: {run['speedup']:.2f}x, "
              f"Load balance: {run['load_balance_score']:.1f}%")

if __name__ == '__main__':
    main()
