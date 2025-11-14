# Parallel Transaction Executor — DAG-Based Execution + Interactive GUI

This project implements a **parallel transaction executor** using a **Directed Acyclic Graph (DAG)**.  
Transactions that do not conflict (based on read/write sets) are executed **in parallel**, and the entire execution is visualized using a **D3.js GUI**.

Backend: **C++17**  
Frontend: **HTML + CSS + JavaScript (D3.js)**  
No backend frameworks required.

---

# 📁 Project Overview

```
project/
│
├── include/                    # Header files (.h)
├── *.cpp                       # Core C++ source files
│   (DAG.cpp, Executor.cpp, Transaction.cpp, etc.)
│
├── main.cpp                    # Entry point
│
└── gui/                        # Frontend visualization
    ├── index.html              # D3.js GUI
    ├── dag_output.json         # Generated DAG
    └── trace.json              # Execution trace
```

---

# 🛠️ Requirements

### ✔ C++17 Compiler  
- Linux/macOS: built-in `g++`  
- Windows: **MSYS2 MinGW64**

### ✔ Python 3 (to serve GUI)
You only need this command:

```bash
python -m http.server
```

---

# 🚀 1. Build the C++ Program

Run this in the **project root**.

### Linux / macOS / MSYS2 / Git Bash
```bash
g++ -std=c++17 -O2 -pthread -I include     DAG.cpp Executor.cpp State.cpp ThreadPool.cpp Transaction.cpp Utils.cpp     Metrics.cpp DAGExporter.cpp TraceWriter.cpp main.cpp     -o dipetrans_app
```

### Windows (MSYS2 MinGW64)
```bash
g++ -std=c++17 -O2 -pthread -I include ^
    DAG.cpp Executor.cpp State.cpp ThreadPool.cpp Transaction.cpp Utils.cpp ^
    Metrics.cpp DAGExporter.cpp TraceWriter.cpp main.cpp ^
    -o dipetrans_app.exe
```

---

# ▶️ 2. Run the Executor

### Linux/macOS
```bash
./dipetrans_app
```

### Windows
```bash
./dipetrans_app.exe
```

This generates:

- `dag_output.json`
- `trace.json`
- `dag_output.dot`

---

# 🌐 3. View the GUI

## Step A — Copy outputs into `gui/`

### Linux/macOS:
```bash
cp dag_output.json gui/
cp trace.json gui/
```

### Windows (MSYS2):
```bash
copy dag_output.json gui
copy trace.json gui
```

---

## Step B — Start local HTTP server

```bash
cd gui
python -m http.server 8000
```

---

## Step C — Open in browser

```
http://localhost:8000/index.html
```

---

# 🎨 GUI Features

- Interactive **DAG visualization** with arrowheads  
- Node lifecycle animation: **pending → ready → running → done**  
- Inspector showing:
  - Read/write sets  
  - Thread ID  
  - Delta updates  
- Full execution playback:
  - ▶ Play  
  - ⏸ Pause  
  - ⏭ Step  
  - 🔁 Reset  

---

# ❗ Troubleshooting

| Problem | Solution |
|--------|----------|
| GUI loads blank | You forgot to copy JSON files to `gui/` |
| Browser blocks JSON | You must run `python -m http.server`, not open the file directly |
| Missing tx events | DAG had a cycle or JSON incomplete |

---

# 🌍 Deploying the GUI Online

Upload the **contents of `gui/`** to:

- GitHub Pages  
- Netlify  
- Vercel  
- Cloudflare Pages  

Works instantly — **no backend required**.

---

# 📘 Summary

### Build:
```bash
g++ -std=c++17 -O2 -pthread -I include ... -o dipetrans_app
```

### Run:
```bash
./dipetrans_app
```

### Serve GUI:
```bash
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
