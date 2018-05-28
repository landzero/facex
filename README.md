# facex

facex

## Dependencies

install `dlib`, `opencv` from source, enabling AVX or SSE4 instructions.

see https://github.com/Homebrew/homebrew-core/pull/28347

```bash
brew install -s opencv dlib
```

install `cmake`

```bash
brew install cmake
```

## Build

build with cmake

```bash
cmake .
make
```
