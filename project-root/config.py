# config.py - Configuration settings for DiPETrans

import os
from pathlib import Path

# Project paths
PROJECT_ROOT = Path(__file__).parent
DATA_DIR = PROJECT_ROOT / "data"
RESULTS_DIR = PROJECT_ROOT / "results"
PLOTS_DIR = RESULTS_DIR / "plots"
LOGS_DIR = PROJECT_ROOT / "logs"

# Create directories if they don't exist
for directory in [DATA_DIR, RESULTS_DIR, PLOTS_DIR, LOGS_DIR]:
    directory.mkdir(parents=True, exist_ok=True)

# Blockchain Configuration
BLOCK_CONFIG = {
    'MAX_TRANSACTIONS_PER_BLOCK': 500,
    'MIN_TRANSACTIONS_PER_BLOCK': 10,
    'DIFFICULTY': 4,  # Number of leading zeros in hash (simplified PoW)
    'GAS_LIMIT': 1_000_000,
    'BLOCK_REWARD': 1.0,
}

# Execution Configuration
EXECUTION_CONFIG = {
    'NUM_FOLLOWERS': 4,
    'TIMEOUT_SECONDS': 30,
    'LOAD_BALANCE_METHOD': 'bin_packing',  # or 'greedy'
    'USE_MULTIPROCESSING': True,
}

# Dataset Configuration
DATASET_CONFIG = {
    'SYNTHETIC_TRANSACTIONS': 1000,
    'CONTRACT_TRANSACTION_RATIO': 0.4,
    'USE_REAL_DATA': False,
    'BIGQUERY_PROJECT': os.getenv('GCP_PROJECT_ID', ''),
}

# Benchmark Configuration
BENCHMARK_CONFIG = {
    'ITERATIONS': 10,
    'FOLLOWER_COUNTS': [1, 2, 3, 4, 5],
    'MEASURE_MEMORY': True,
    'PLOT_RESULTS': True,
}

# Logging Configuration
LOGGING_CONFIG = {
    'LEVEL': 'INFO',
    'FORMAT': '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    'LOG_FILE': LOGS_DIR / 'dipetrans.log',
}

# Smart Contracts Configuration (Ethereum most-used functions)
SMART_CONTRACTS = {
    'transfer': {'hash': '0xa9059cbb', 'gas_cost': 20000},
    'approve': {'hash': '0x095ea7b3', 'gas_cost': 20000},
    'vote': {'hash': '0x0121b93f', 'gas_cost': 30000},
    'submitTransaction': {'hash': '0xc6427474', 'gas_cost': 40000},
    'issue': {'hash': '0x867904b4', 'gas_cost': 25000},
    'callback': {'hash': '0x38bbfa50', 'gas_cost': 35000},
    'playerRollDice': {'hash': '0xdc6dd152', 'gas_cost': 50000},
    'multisend': {'hash': '0xad8733ca', 'gas_cost': 45000},
    'SmartAirdrop': {'hash': '0xa8faf6f0', 'gas_cost': 60000},
    'PublicMine': {'hash': '0x87ccccb3', 'gas_cost': 55000},
    'setGenesisAddress': {'hash': '0x0d571742', 'gas_cost': 30000},
}
