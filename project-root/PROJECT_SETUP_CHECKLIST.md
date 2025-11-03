# PROJECT_SETUP_CHECKLIST.md - Complete Setup Instructions

## Phase 0: Pre-Installation Checklist

- [ ] Python 3.9+ installed on your system
- [ ] pip (Python package manager) available
- [ ] Internet connection for downloading packages
- [ ] 4GB+ disk space available
- [ ] Terminal/Command prompt access

**Check your setup:**
```bash
python --version          # Should show 3.9 or higher
pip --version            # Should show pip version
python -m pip list | wc -l  # Show number of installed packages
```

---

## Phase 1: Repository Setup (2 minutes)

### Option A: If you have all files already

```bash
# 1. Navigate to your project directory
cd /path/to/DiPETrans_Project

# 2. Verify all files are present
ls -la  # Should show: main.py, config.py, README.md, etc.

# 3. Create necessary directory structure (optional - will be created automatically)
mkdir -p data execution blockchain experiments results logs tests
```

### Option B: Create from scratch

```bash
# 1. Create project directory
mkdir DiPETrans_Project
cd DiPETrans_Project

# 2. Create subdirectories
mkdir -p data execution blockchain experiments results logs tests

# 3. Create __init__.py files (required for Python packages)
touch blockchain/__init__.py
touch execution/__init__.py
touch data/__init__.py
touch experiments/__init__.py
touch tests/__init__.py

# 4. Move all provided .py files to correct directories:
# - config.py → root
# - crypto_utils.py → root (or blockchain/)
# - transaction.py → root (or blockchain/)
# - dependency_analyzer.py → execution/
# - parallel_executor.py → execution/
# - smart_contracts.py → execution/
# - synthetic_data.py → data/
# - benchmark.py → experiments/
# - main.py → root
# - requirements.txt → root
# - README.md, QUICK_START.md → root
```

---

## Phase 2: Virtual Environment (Recommended)

### Why virtual environment?
- Isolates project dependencies
- Prevents version conflicts with other projects
- Easy to reproduce setup on different machines

### Create and Activate Virtual Environment

**On Linux/Mac:**
```bash
# Create virtual environment
python3 -m venv venv

# Activate it
source venv/bin/activate

# You should see (venv) in your terminal
```

**On Windows (PowerShell):**
```bash
# Create virtual environment
python -m venv venv

# Activate it
.\venv\Scripts\Activate.ps1

# You should see (venv) in your terminal
```

**On Windows (CMD):**
```bash
# Create virtual environment
python -m venv venv

# Activate it
venv\Scripts\activate.bat

# You should see (venv) in your terminal
```

---

## Phase 3: Install Dependencies (3 minutes)

### Single Command Installation

```bash
# Make sure you're in project root and venv is activated
pip install -r requirements.txt
```

**This installs:**
- NetworkX 2.6.3 - Graph algorithms (Weakly Connected Components)
- Pandas 1.3.5 - Data manipulation
- NumPy 1.21.6 - Numerical computing
- Matplotlib 3.5.2 - Visualization
- And 20+ more packages for blockchain, crypto, testing

### Verify Installation

```bash
# Check all packages installed
pip list

# Test key imports
python -c "import networkx; import pandas; import numpy; print('✓ All packages OK')"

# Test blockchain module import
python -c "from blockchain.transaction import Transaction; print('✓ Blockchain module OK')"

# Test execution module import
python -c "from execution.dependency_analyzer import DependencyAnalyzer; print('✓ Execution module OK')"
```

---

## Phase 4: Configuration Check

### Edit config.py (Optional)

```bash
# View current config
cat config.py

# Edit with your preferred editor
nano config.py      # Linux/Mac
# or
code config.py      # VS Code
# or
vim config.py       # Vim
```

**Key settings to check:**
- `MAX_TRANSACTIONS_PER_BLOCK`: 500 (good starting point)
- `NUM_FOLLOWERS`: 4 (adjust based on your CPU cores)
- `BENCHMARK_CONFIG`: Number of iterations and test configurations

### Typical configurations:

**For weak laptop (2 cores):**
```python
NUM_FOLLOWERS: 2
ITERATIONS: 2
BENCHMARK_CONFIG = {'ITERATIONS': 2, 'FOLLOWER_COUNTS': [1, 2]}
```

**For decent laptop (4 cores):**
```python
NUM_FOLLOWERS: 4
ITERATIONS: 5
BENCHMARK_CONFIG = {'ITERATIONS': 5, 'FOLLOWER_COUNTS': [1, 2, 3, 4]}
```

**For powerful workstation (8+ cores):**
```python
NUM_FOLLOWERS: 8
ITERATIONS: 10
BENCHMARK_CONFIG = {'ITERATIONS': 10, 'FOLLOWER_COUNTS': [1, 2, 3, 4, 5, 6, 8]}
```

---

## Phase 5: Run Quick Validation

### Test 1: Module Imports
```bash
python << 'EOF'
import sys
print("Testing imports...")
try:
    from blockchain.transaction import Transaction
    print("✓ Transaction class imported")
    
    from execution.dependency_analyzer import DependencyAnalyzer
    print("✓ DependencyAnalyzer class imported")
    
    from execution.parallel_executor import ParallelExecutor
    print("✓ ParallelExecutor class imported")
    
    from execution.smart_contracts import SmartContractExecutor
    print("✓ SmartContractExecutor class imported")
    
    from data.synthetic_data import generate_synthetic_transactions
    print("✓ Data generation function imported")
    
    print("\n✅ All imports successful!")
except ImportError as e:
    print(f"❌ Import failed: {e}")
    sys.exit(1)
EOF
```

### Test 2: Data Generation
```bash
# Generate small dataset
python data/synthetic_data.py --transactions 100 --output test_data.csv

# Verify CSV was created
ls -lh test_data.csv
head test_data.csv
```

### Test 3: Run Quick Benchmark
```bash
# This should complete in 2-5 minutes
python main.py --mode quick --transactions 200
```

**Expected output:**
```
============================================================
DIPETRANS BENCHMARK
============================================================
Transactions: 200
...
Serial Execution (baseline):
  Average: X.XXXXs ± X.XXXXs

Parallel Execution with 1 followers:
  Average: X.XXXXs ± X.XXXXs
  Speedup: X.XXx

...
✓ Benchmark complete!
✓ Results saved to results/quick_test_results.json
```

---

## Phase 6: Project Structure Verification

After setup, your directory should look like:

```
DiPETrans_Project/
├── __pycache__/                    # Auto-generated, ignore
├── blockchain/
│   ├── __pycache__/               # Auto-generated, ignore
│   ├── __init__.py                # ✓ Must exist
├── execution/
│   ├── __pycache__/               # Auto-generated, ignore
│   ├── __init__.py                # ✓ Must exist
│   ├── dependency_analyzer.py     # ✓ Core algorithm
│   ├── parallel_executor.py       # ✓ Execution engine
│   └── smart_contracts.py         # ✓ Contract simulation
├── data/
│   ├── __init__.py                # ✓ Must exist
│   ├── synthetic_data.py          # ✓ Data generation
│   └── *.csv                      # Generated files
├── experiments/
│   ├── __init__.py                # ✓ Must exist
│   └── benchmark.py               # ✓ Benchmarking
├── tests/
│   └── __init__.py                # ✓ Must exist
├── results/                       # Auto-created by programs
│   └── *.json                     # Benchmark results
├── logs/                          # Auto-created by programs
│   └── *.log                      # Log files
│
├── config.py                      # ✓ Configuration
├── crypto_utils.py                # ✓ Cryptography utils
├── transaction.py                 # ✓ Transaction class
├── main.py                        # ✓ Main entry point
│
├── requirements.txt               # ✓ Dependencies
├── README.md                      # ✓ Full documentation
├── QUICK_START.md                 # ✓ Quick guide
└── PROJECT_SETUP_CHECKLIST.md    # ✓ This file
```

---

## Phase 7: Running the Complete Project

### Option 1: Automated Quick Test (5 minutes)
```bash
python main.py --mode quick --transactions 500
```

### Option 2: Step-by-Step Workflow

**Step 1: Generate data**
```bash
python main.py --mode generate --transactions 1000 --contract-ratio 0.3
```

**Step 2: Analyze dependencies**
```bash
python main.py --mode analyze --data synthetic_data.csv --followers 4
```

**Step 3: Run benchmarks**
```bash
python main.py --mode benchmark --data synthetic_data.csv --followers 1 2 3 4 --iterations 5
```

**Step 4: View results**
```bash
cat results/benchmark_results.json
```

### Option 3: Custom Configuration

```bash
# Generate different dataset
python main.py --mode generate \
  --transactions 2000 \
  --contract-ratio 0.5 \
  --accounts 200 \
  --output my_data.csv

# Run comprehensive benchmark
python main.py --mode benchmark \
  --data my_data.csv \
  --followers 1 2 3 4 5 6 \
  --iterations 10 \
  --output results/comprehensive
```

---

## Phase 8: Troubleshooting

### Issue: ModuleNotFoundError

**Error message:**
```
ModuleNotFoundError: No module named 'blockchain'
```

**Causes and fixes:**
```bash
# 1. Check if in correct directory
pwd  # Should show .../DiPETrans_Project

# 2. Check if __init__.py exists
ls blockchain/__init__.py  # Should exist

# 3. Reinstall with correct paths
pip install -e .

# 4. Verify imports
python -c "from blockchain.transaction import Transaction"
```

### Issue: Can't find data file

**Error message:**
```
Error: Data file 'synthetic_data.csv' not found
```

**Fixes:**
```bash
# Generate it first
python main.py --mode generate

# Or specify full path
python main.py --mode benchmark --data ./data/synthetic_data.csv

# Or check if file exists
ls -la synthetic_data.csv
```

### Issue: Out of Memory

**If program crashes with memory error:**
```bash
# Reduce transaction count
python main.py --mode generate --transactions 200

# Or reduce parallel workers
python main.py --mode benchmark --data synthetic_data.csv --followers 2 3
```

### Issue: Python version mismatch

```bash
# Check version
python --version

# If < 3.9, install Python 3.9+
# Then create new virtual environment
python3.9 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### Issue: Package dependency conflicts

```bash
# Clear pip cache
pip cache purge

# Reinstall all packages
pip install --upgrade --force-reinstall -r requirements.txt
```

---

## Phase 9: Performance Optimization

### Run on multiple cores

**Linux/Mac:**
```bash
# Check number of cores
nproc

# Use all cores
python main.py --mode benchmark --data synthetic_data.csv --followers 1 2 3 4 5 6 7 8
```

**Windows (PowerShell):**
```powershell
# Check number of cores
Get-WmiObject Win32_Processor | Select-Object NumberOfLogicalProcessors
```

### Profile code execution (Advanced)

```bash
# Profile execution time
python -m cProfile -s cumtime main.py --mode quick --transactions 200

# Or use memory_profiler
pip install memory-profiler
python -m memory_profiler main.py --mode quick --transactions 200
```

---

## Phase 10: Preparing for Submission

### Final Checklist

- [ ] All code files present and working
- [ ] requirements.txt complete and tested
- [ ] README.md comprehensive and clear
- [ ] Main.py runs without errors
- [ ] Benchmark results generated and saved
- [ ] Documentation includes examples
- [ ] Code well-commented
- [ ] Project follows academic standards
- [ ] Tested on fresh virtual environment
- [ ] No hardcoded paths (all relative)

### Create clean package

```bash
# Remove temporary files
rm -rf __pycache__ */__pycache__ *.pyc

# Keep only essential results
rm results/*.json  # Keep one sample result

# Create archive
tar -czf DiPETrans_Project.tar.gz DiPETrans_Project/

# Or zip
zip -r DiPETrans_Project.zip DiPETrans_Project/
```

### Final test from fresh environment

```bash
# Create new virtual environment
python -m venv test_env
source test_env/bin/activate  # Linux/Mac
# or
test_env\Scripts\activate     # Windows

# Install fresh
pip install -r requirements.txt

# Run tests
python main.py --mode quick
```

---

## Success Indicators

✅ **Project is ready if:**

1. ✓ All modules import without errors
2. ✓ Synthetic data generates successfully
3. ✓ Quick benchmark runs in < 5 minutes
4. ✓ Speedup > 1.5x with 4 followers
5. ✓ Load balance score > 90%
6. ✓ Results saved to JSON properly
7. ✓ No import errors when running from different directory
8. ✓ Code follows Python best practices
9. ✓ Documentation is comprehensive
10. ✓ Project can run on fresh clone

---

## Next Steps for Presentation

1. **Run full benchmark** with 1000+ transactions
2. **Analyze results** and create performance graphs
3. **Write report** discussing findings
4. **Prepare demo** showing:
   - Data generation
   - Dependency analysis
   - Speedup comparison
   - Load balancing results
5. **Practice explanation** of core algorithm (WCC)

---

**Congratulations! Your DiPETrans project is ready! 🚀**

For questions: Refer to README.md and QUICK_START.md
For troubleshooting: Check this file's Phase 8

Good luck with your project presentation!
