# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Planned
- Parallel scatter and per-bucket sort (OpenMP).
- Adaptive bucket count based on first-pass bucket-size skew.

## [0.1.0] - 2026-11-03

### Added
- Initial `sgs::sort` header-only implementation.
- `mode::uniform` (O(1) bucket lookup for arithmetic types).
- `mode::quantile` (O(log k) bucket lookup, any totally-ordered type).
- `sgs::config` tuning knobs.
- Unit tests: correctness, edge cases.
- Benchmark executable.
- Pure-Python reference implementation under `python/`.
- Documentation: `README.md`, `README_zh.md`, `docs/ALGORITHM.md`.
