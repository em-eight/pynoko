# Pynoko
Python bindings for Kinoko

# Requirements
- cmake
- Recent clang version
- Python 3.8+
- python library for said version (e.g. libpython3-dev for ubuntu/debian). Usually just comes with python itself

When cloning for the first time, grab all submodules first
```
git submodule update --init --recursive
```

# Building
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

# Usage
Make sure the generated cpython library is in your python path and `import pynoko`
