# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
Major changes to `extinet` unit, in preparation for a new server handler.

## Added
- `extinet` connect_auto() for connecting to ambiguous AF_ family types.
- `extinet` get_hostipv6() for IPv6 socket operations.

## Changed
- `extinet` gethostip() to get_hostipv4().
- `extinet` sock_connect/send/recv() to *_timed() named function variants to better reflect their use over standard socket operations.
- `extinet` sock_close() to closesocket(), used historically.
- `extinet` sock_startup/cleanup() to wsa_startup(major, minor) and wsa_cleanup().

## Removed
- `extinet` get_sock_ip() in favor of `struct sockaddr` and associated functions.
- `extinet` sock_state() and Sockinuse restrictions.
- `extinet` sock_connect_addr() to be handled by application.

## [1.2.0] - 2022-05-18
Function changes/additions to `extinet` and `extthread` units.
Minor changes to comments and documentation, for clarity.
Updated to build-c-1.2.1 for makefile improvements.

## Added
- `extinet` gethostip() for obtaining host system IPv4 address
- `extinet` sock_state() as replacement for global Sockinuse variable
- `extthread` thread_join_list() as replacement for thread_multijoin()
- `extthread` thread_terminate() for sending terminate signals to threads
- `extthread` thread_terminate_list() for sending many terminate signals

## Changed
- `build-c` tools updated to v1.2.1 for makefile improvements
- Some documentation and comment adjustments, for clarity

## Removed
- `extinet` phostinfo() - see gethostip() notes
- `extthread` thread_multijoin() - see thread_join_list() notes

## [1.1.2] - 2022-04-15
Updated to build-c-1.1.3 for CUDA (revert) changes.

## [1.1.1] - 2022-04-06
Updated to build-c-1.1.2 for CUDA build fixes and docs config adjustments.

## [1.1.0] - 2022-04-06
Updated `build-c` utilities to `build-c-1.1.1`.

## [1.0.0] - 2022-01-26
Initial repository release.

[Unreleased]: https://github.com/adequatesystems/extended-c/compare/v1.1.2...HEAD
[1.1.2]: https://github.com/adequatesystems/extended-c/compare/v1.1.1...v1.1.2
[1.1.1]: https://github.com/adequatesystems/extended-c/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/adequatesystems/extended-c/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/adequatesystems/extended-c/releases/tag/v1.0.0
