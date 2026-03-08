# Pynoko
Python bindings for Kinoko

# Requirements
- cmake
- Python 3.8+
- python library for said version (e.g. libpython3-dev for ubuntu/debian). If you python, you most likely already have this as well

When cloning for the first time, grab all submodules first
```
git submodule update --init --recursive
```

# Building
Note: if you want to build the wheels or Release builds, only clang is supported. If you want to use gcc, specify `-DCMAKE_BUILD_TYPE=Debug`
## Local build
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

```

## Build and install wheel
Physics engine only. Wheel builds currently do not support GUI visualization.
```
pip install .
```

# Usage
Make sure the generated cpython library is in your python path and `import pynoko`. See `example.py` for a race replay example
