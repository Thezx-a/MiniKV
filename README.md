<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/Tests-22%20passing-brightgreen?style=for-the-badge" alt="Tests"/>
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="MIT"/>
</p>

<h1 align="center">🗄️ MiniKV</h1>

<p align="center">
  <b>Embedded LSM-Tree Key-Value Storage Engine</b><br/>
  <i>WAL · Bloom Filter · SSTable · Compaction — built from scratch in C++17</i>
</p>

---

## Overview

MiniKV is a **from-scratch implementation** of the core architecture behind LevelDB and RocksDB — the LSM-Tree (Log-Structured Merge-Tree). It's designed as a drop-in storage engine for applications needing fast writes with ordered key lookups.

```
 Write Path                          Read Path
 ┌──────────┐                       ┌──────────┐
 │  Client   │                       │  Client   │
 └────┬─────┘                       └────┬─────┘
      │                                   │
      ▼                                   │
 ┌──────────┐    ┌──────────┐            │
 │   WAL    │───▶│ MemTable │            │
 │ (durable)│    │ (sorted) │            │
 └──────────┘    └────┬─────┘            │
                      │ flush             │
                      ▼                   │
                 ┌──────────┐             │
                 │  L0 SST  │─────────────┤
                 │ (sorted) │   binary    │
                 └────┬─────┘   search    │
                      │ merge             │
                      ▼                   │
                 ┌──────────┐             │
                 │  L1 SST  │─────────────┘
                 │ (sorted) │
                 └──────────┘
```

---

## Core Concepts

| Component | What It Does |
|-----------|-------------|
| **MemTable** | In-memory sorted buffer (SkipList). Writes go here first. |
| **WAL** | Write-Ahead Log. Guarantees durability on crash. |
| **SSTable** | Sorted String Table. Immutable on-disk sorted files. |
| **Bloom Filter** | Probabilistic structure: "definitely not in this SSTable" |
| **Compaction** | Merges SSTables to reclaim space and reduce read levels. |
| **Skip List** | Probabilistic sorted structure used as MemTable backend. |

---

## Quick Start

```bash
# Clone
git clone https://github.com/Thezx-a/MiniKV.git
cd MiniKV

# Build & Test
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_TESTS=ON -DCMAKE_CXX_COMPILER=g++-12
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

### Use as a Library

```cpp
#include <minikv/db.h>

minikv::Options opts;
opts.create_if_missing = true;
minikv::DB* db;
minikv::DB::Open(opts, "/tmp/mydb", &db);

db->Put("key", "value");
std::string val;
db->Get("key", &val);  // val == "value"

delete db;
```

### Link in CMake

```cmake
add_subdirectory(path/to/MiniKV)
target_link_libraries(your_app minikv)
```

---

## Project Structure

```
MiniKV/
├── include/minikv/          Public API headers
│   ├── db.h                 DB open/put/get/delete
│   ├── options.h            Configuration
│   ├── comparator.h         Key comparison
│   └── write_batch.h        Atomic batch writes
├── src/
│   ├── db/                  DB implementation
│   ├── core/                LSM-Tree internals
│   │   ├── skiplist.h       MemTable backend
│   │   ├── wal.h            Write-Ahead Log
│   │   ├── sstable.h        SSTable reader/writer
│   │   ├── compaction.h     Merge logic
│   │   └── bloom.h          Bloom filter
│   ├── table/               Table operations
│   └── utils/               Utilities
│       ├── coding.h         Varint encoding
│       ├── crc32.h          Checksums
│       ├── hash.h           Hash functions
│       ├── lru_cache.h      LRU block cache
│       └── thread_pool.h    Async compaction
├── tests/                   22 unit tests
├── benches/                 Benchmarks
├── client/                  Python asyncio client
├── docker/                  Docker deployment
└── docs/                    Design documentation
```

---

## Architecture

MiniKV implements a **multi-level LSM-Tree**:

```
Level 0:   [SSTable] [SSTable] [SSTable]     ← from MemTable flush
            ↓ compact
Level 1:   [    Sorted SSTable    ]            ← merged, non-overlapping
            ↓ compact
Level 2:   [        Sorted SSTable            ] ← larger, fewer files
```

**Write amplification** is managed by leveled compaction — each level is at most 10× the previous. **Read amplification** is reduced by Bloom filters that skip entire SSTables without touching disk.

---

## Tests

22 unit tests covering:

| Module | Tests | What's Verified |
|--------|-------|-----------------|
| SkipList | 3 | Insert, iterator, memory accounting |
| BloomFilter | 3 | FP rate, serialization, integration |
| WAL | 3 | Append, recovery, truncate |
| SSTable | 2 | Build, lookup |
| Compaction | 2 | L0→L1 merge, tombstone removal |
| DB | 5 | Put/Get, delete, batch, reopen, crash recovery |
| ThreadPool | 2 | Concurrent tasks, shutdown |
| Coding | 2 | Varint, fixed32 |

---

## Tech Stack

`C++17` `CMake` `Ninja` `Epoll` `Google Test` `Python asyncio`

---

## License

[MIT](LICENSE)
