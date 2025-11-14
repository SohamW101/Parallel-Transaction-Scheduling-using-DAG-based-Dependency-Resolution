# Parallel Transaction Executor — DAG-Based Execution + Interactive GUI

This project implements a **parallel transaction executor** using a **Directed Acyclic Graph (DAG)**.  
Transactions that do not conflict (based on read/write sets) are executed **in parallel**, and the entire execution is visualized using a **D3.js GUI**.

The system is built in **C++ (backend)** + **HTML/CSS/JavaScript (frontend GUI)** and requires **no frameworks** for backend.

---

# 📁 Project Overview

```
project/
│
├── include/                    # Header files (.h)
├── *.cpp                       # Core C++ source files
│   DAG.cpp, Executor.cpp, Transaction.cpp, etc.
│
├── main.cpp                    # Program entry point
│
└── gui/                        # Frontend visualization
    ├── index.html              # Interactive D3.js GUI
    ├── dag_output.json         # Generated DAG structure
    └── trace.json              # Generated execution trace
```

---

# 🛠️ Requirements

### ✔ C++:
- `g++` with **C++17** support  
  - Windows: MSYS2 + MinGW64  
  - Linux/macOS: built-in g++

### ✔ Python 3:
```
python -m http.server
```

---

# 🚀 1. Build the C++ Program

Run this from the **project root**:

### Linux / macOS / MSYS2 / Git Bash:
```
g++ -std=c++17 -O2 -pthread -I include     DAG.cpp Executor.cpp State.cpp ThreadPool.cpp Transaction.cpp Utils.cpp     Metrics.cpp DAGExporter.cpp TraceWriter.cpp main.cpp     -o dipetrans_app
```

### Windows (MSYS2 MinGW):
```
g++ -std=c++17 -O2 -pthread -I include ^
    DAG.cpp Executor.cpp State.cpp ThreadPool.cpp Transaction.cpp Utils.cpp ^
    Metrics.cpp DAGExporter.cpp TraceWriter.cpp main.cpp ^
    -o dipetrans_app.exe
```

---

# ▶️ 2. Run the Executor

### Linux/macOS:
```
./dipetrans_app
```

### Windows:
```
./dipetrans_app.exe
```

This generates:
- `dag_output.json`
- `trace.json`
- `dag_output.dot`

---

# 🌐 3. View the GUI

### Step A — Copy JSON outputs into `gui/`

Linux/macOS:
```
cp dag_output.json gui/
cp trace.json gui/
```

Windows:
```
copy dag_output.json guicopy trace.json gui```

---

### Step B — Start the local server:

```
cd gui
python -m http.server 8000
```

### Step C — Open in browser:

```
http://localhost:8000/index.html
```

---

# 🎨 GUI Features

- Directed DAG with arrowheads  
- Node state transitions: pending → ready → running → done  
- Inspector: read/write sets, thread ID, merged deltas  
- Event timeline with Play / Pause / Step / Reset  
- Full animation of execution trace  

---

# ❗ Troubleshooting

- **Blank GUI** → Forgot to copy JSON files  
- **Browser blocks JSON** → Must use `python -m http.server`  
- **Missing tx evals** → DAG had a cycle  

---

# 🌍 Deploying the GUI Online

Upload everything inside `gui/` to:
- GitHub Pages  
- Netlify  
- Vercel  
- Cloudflare Pages  

It works instantly — no backend required.

---

# 📘 Summary

### Build:
```
g++ -std=c++17 -O2 -pthread -I include ... -o dipetrans_app
```

### Run:
```
./dipetrans_app
```

### Serve GUI:
```
cd gui
python -m http.server 8000
```

### Open:
```
http://localhost:8000/index.html
```

---

# 📜 License
MIT License
