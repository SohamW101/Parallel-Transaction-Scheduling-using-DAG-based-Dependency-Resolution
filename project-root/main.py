# main.py
"""
Main entry point for DiPETrans project
Demonstrates complete workflow: data → sharding → parallel execution → benchmarking
"""

import sys
import argparse
from pathlib import Path

# Add project to path
sys.path.insert(0, str(Path(__file__).parent))

from data.synthetic_data import generate_synthetic_transactions, save_to_csv
from experiments.benchmark import Benchmark
from blockchain.transaction import Transaction
from execution.dependency_analyzer import DependencyAnalyzer, LoadBalancer

def main():
    parser = argparse.ArgumentParser(
        description='DiPETrans: Distributed Parallel Execution of Blockchain Transactions',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Generate synthetic data
  python main.py --mode generate --transactions 1000 --contract-ratio 0.4
  
  # Run benchmark
  python main.py --mode benchmark --data synthetic_data.csv --followers 4
  
  # Quick test
  python main.py --mode quick --transactions 500
        """
    )
    
    parser.add_argument('--mode', type=str, choices=['generate', 'benchmark', 'analyze', 'quick'],
                       default='quick',
                       help='Operation mode')
    
    # Generate mode
    parser.add_argument('--transactions', type=int, default=1000,
                       help='Number of transactions to generate')
    parser.add_argument('--contract-ratio', type=float, default=0.4,
                       help='Ratio of smart contract transactions (0-1)')
    parser.add_argument('--accounts', type=int, default=100,
                       help='Number of unique accounts')
    
    # Benchmark mode
    parser.add_argument('--data', type=str, help='CSV file with transaction data')
    parser.add_argument('--followers', type=int, nargs='+', default=[1,2,3,4,5],
                       help='Number of followers to test')
    parser.add_argument('--iterations', type=int, default=3,
                       help='Iterations per benchmark configuration')
    parser.add_argument('--output', type=str, default='results',
                       help='Output directory for results')
    
    args = parser.parse_args()
    
    # Generate mode: Create synthetic data
    if args.mode == 'generate':
        print("="*70)
        print("MODE: GENERATE SYNTHETIC DATA")
        print("="*70)
        
        print(f"\nGenerating {args.transactions} synthetic transactions...")
        print(f"  Contract ratio: {args.contract_ratio}")
        print(f"  Unique accounts: {args.accounts}")
        
        transactions = generate_synthetic_transactions(
            args.transactions,
            args.contract_ratio,
            args.accounts
        )
        
        output_file = 'synthetic_data.csv'
        save_to_csv(transactions, output_file)
        
        print(f"\n✓ Successfully generated and saved to {output_file}")
        print(f"✓ Next step: python main.py --mode benchmark --data {output_file}")
    
    # Benchmark mode: Compare serial vs parallel
    elif args.mode == 'benchmark':
        if not args.data:
            print("Error: --data argument required for benchmark mode")
            sys.exit(1)
        
        print("="*70)
        print("MODE: BENCHMARK (Serial vs Parallel Execution)")
        print("="*70)
        
        # Check if file exists
        if not Path(args.data).exists():
            print(f"Error: Data file '{args.data}' not found")
            print(f"Generate data first: python main.py --mode generate")
            sys.exit(1)
        
        benchmark = Benchmark(output_dir=args.output)
        print(f"\nLoading transactions from {args.data}...")
        transactions = benchmark.load_transactions_from_csv(args.data)
        
        print(f"Loaded {len(transactions)} transactions")
        
        # Run benchmark
        results = benchmark.run_benchmark(
            transactions,
            num_followers_list=args.followers,
            iterations=args.iterations
        )
        
        # Save results
        results_file = benchmark.save_results(results)
        
        print(f"\n✓ Benchmark complete!")
        print(f"✓ Results saved to {results_file}")
        print(f"✓ View results: cat {results_file}")
    
    # Analyze mode: Analyze dependencies and shards
    elif args.mode == 'analyze':
        if not args.data:
            print("Error: --data argument required for analyze mode")
            sys.exit(1)
        
        print("="*70)
        print("MODE: ANALYZE TRANSACTION DEPENDENCIES")
        print("="*70)
        
        if not Path(args.data).exists():
            print(f"Error: Data file '{args.data}' not found")
            sys.exit(1)
        
        benchmark = Benchmark()
        transactions = benchmark.load_transactions_from_csv(args.data)
        
        print(f"\nAnalyzing {len(transactions)} transactions...")
        
        # Analyze dependencies
        analyzer = DependencyAnalyzer()
        shards = analyzer.find_shards(transactions)
        stats = analyzer.get_shard_statistics(shards)
        
        print(f"\nDependency Analysis Results:")
        print(f"  Total shards: {stats['num_shards']}")
        print(f"  Min shard size: {stats['min_shard_size']}")
        print(f"  Max shard size: {stats['max_shard_size']}")
        print(f"  Avg shard size: {stats['avg_shard_size']:.2f}")
        
        # Load balance analysis
        num_followers = args.followers[0] if args.followers else 4
        balanced_loads = LoadBalancer.balance_shards(shards, num_followers)
        balance_score = LoadBalancer.get_load_balance_score(balanced_loads)
        
        print(f"\nLoad Balancing for {num_followers} followers:")
        print(f"  Balance score: {balance_score:.1f}%")
        for follower_id, txs in balanced_loads.items():
            print(f"  Follower {follower_id}: {len(txs)} transactions")
    
    # Quick mode: Generate data and run quick benchmark
    else:  # quick
        print("="*70)
        print("MODE: QUICK TEST (Generate data + Benchmark)")
        print("="*70)
        
        # Generate data
        print(f"\n[1/2] Generating {args.transactions} synthetic transactions...")
        transactions_list = generate_synthetic_transactions(
            args.transactions,
            args.contract_ratio,
            args.accounts
        )
        save_to_csv(transactions_list, 'quick_test_data.csv')
        
        # Run quick benchmark
        print(f"\n[2/2] Running quick benchmark...")
        benchmark = Benchmark(output_dir='results')
        transactions = benchmark.load_transactions_from_csv('quick_test_data.csv')
        
        results = benchmark.run_benchmark(
            transactions,
            num_followers_list=[1, 2, 4],  # Quick test with fewer configs
            iterations=2  # Fewer iterations for speed
        )
        
        benchmark.save_results(results, 'quick_test_results.json')
        
        print("\n✓ Quick test complete!")
        print(f"✓ Data: quick_test_data.csv")
        print(f"✓ Results: results/quick_test_results.json")

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(0)
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
