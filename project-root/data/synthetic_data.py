# data/synthetic_data.py
"""
Generate synthetic blockchain transaction data for testing
"""

import csv
import random
import argparse
from pathlib import Path
from typing import List, Dict
import sys

def generate_addresses(count: int) -> List[str]:
    """Generate random Ethereum-style addresses"""
    addresses = []
    for i in range(count):
        addr = '0x' + ''.join(random.choices('0123456789abcdef', k=40))
        addresses.append(addr)
    return addresses

def generate_synthetic_transactions(num_transactions: int,
                                   contract_ratio: float = 0.4,
                                   num_accounts: int = 100) -> List[Dict]:
    """
    Generate synthetic blockchain transactions
    
    Args:
        num_transactions: Number of transactions to generate
        contract_ratio: Ratio of smart contract transactions (0-1)
        num_accounts: Number of unique accounts
    
    Returns:
        List of transaction dictionaries
    """
    
    addresses = generate_addresses(num_accounts)
    contract_functions = [
        'transfer', 'approve', 'vote', 'submitTransaction', 'issue',
        'callback', 'playerRollDice', 'multisend', 'SmartAirdrop',
        'PublicMine', 'setGenesisAddress'
    ]
    
    transactions = []
    
    for i in range(num_transactions):
        tx_from = random.choice(addresses)
        tx_to = random.choice(addresses)
        
        # Ensure from != to for most transactions
        while tx_to == tx_from and random.random() > 0.1:
            tx_to = random.choice(addresses)
        
        is_contract = random.random() < contract_ratio
        
        if is_contract:
            tx = {
                'tx_id': f'0x{i:064x}',
                'from_address': tx_from,
                'to_address': tx_to,
                'value': random.uniform(0, 10),
                'contract_function': random.choice(contract_functions),
                'gas_used': random.randint(20000, 100000),
                'block_number': i // 100 + 1,
                'timestamp': 1600000000 + i,
                'is_contract': True,
            }
        else:
            tx = {
                'tx_id': f'0x{i:064x}',
                'from_address': tx_from,
                'to_address': tx_to,
                'value': random.uniform(0.01, 100),
                'contract_function': None,
                'gas_used': 21000,
                'block_number': i // 100 + 1,
                'timestamp': 1600000000 + i,
                'is_contract': False,
            }
        
        transactions.append(tx)
    
    return transactions

def save_to_csv(transactions: List[Dict], output_file: str):
    """Save transactions to CSV file"""
    if not transactions:
        print("No transactions to save")
        return
    
    fields = transactions[0].keys()
    
    with open(output_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader()
        writer.writerows(transactions)
    
    print(f"✓ Saved {len(transactions)} transactions to {output_file}")

def main():
    parser = argparse.ArgumentParser(description='Generate synthetic blockchain data')
    parser.add_argument('--transactions', type=int, default=1000,
                       help='Number of transactions to generate')
    parser.add_argument('--contract-ratio', type=float, default=0.4,
                       help='Ratio of smart contract transactions')
    parser.add_argument('--accounts', type=int, default=100,
                       help='Number of unique accounts')
    parser.add_argument('--output', type=str, default='synthetic_data.csv',
                       help='Output CSV file')
    
    args = parser.parse_args()
    
    print(f"Generating {args.transactions} synthetic transactions...")
    transactions = generate_synthetic_transactions(
        args.transactions,
        args.contract_ratio,
        args.accounts
    )
    
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    save_to_csv(transactions, str(output_path))
    
    # Print summary
    contract_count = sum(1 for tx in transactions if tx['is_contract'])
    print(f"\nSummary:")
    print(f"  Total transactions: {len(transactions)}")
    print(f"  Smart contract calls: {contract_count} ({100*contract_count/len(transactions):.1f}%)")
    print(f"  Monetary transfers: {len(transactions) - contract_count} ({100*(len(transactions)-contract_count)/len(transactions):.1f}%)")
    print(f"  Unique accounts: {args.accounts}")

if __name__ == '__main__':
    main()
