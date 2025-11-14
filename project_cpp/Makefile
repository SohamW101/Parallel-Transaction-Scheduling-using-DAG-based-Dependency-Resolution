CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -I ./include

SRC = DAG.cpp Executor.cpp State.cpp ThreadPool.cpp Transaction.cpp Utils.cpp Metrics.cpp DAGExporter.cpp TraceWriter.cpp main.cpp
TARGET = dipetrans_app.exe

all: $(TARGET)

$(TARGET):
  $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
  ./$(TARGET)

gui: $(TARGET)
  ./$(TARGET)
  cp dag_output.json gui/
  cp trace.json gui/
  cd gui && python -m http.server 8000

serve:
  cd gui && python -m http.server 8000

clean:
  rm -f $(TARGET)