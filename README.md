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

build with `Release` config for best compiler optimization.

```bash
cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=Release .
make
```

## Run

```bash
./facex --help
```

## Monitor

use `nc` (netcat) to monitor output

```bash
nc 127.0.0.1 6699
```

## Protocol

TBD
