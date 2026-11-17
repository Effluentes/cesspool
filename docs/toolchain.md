# Toolchain baseline and evidence

This records observations made in the development environment; it is not a claim that the
production toolchain is qualified.

| Tool | Observed version / pin | Evidence |
|---|---|---|
| ESP-IDF development environment | `v6.1-dev-7308-g0276131402f` | `idf.py --version` in the workspace |
| ESP-IDF component lock | `6.2.0` | `dependencies.lock`; this conflicts with the installed development version |
| ESP-IDF CI container | `espressif/idf` image digest `sha256:0e714292d8b4e6e75cb42e25461716221448858e125d96dbfea09732964c412d` | Immutable digest for official `release-v6.2`; tag metadata at <https://hub.docker.com/r/espressif/idf/tags> |
| Host GCC | `13.3.0` | `c++ --version` |
| Host CMake | `4.0.3` | `cmake --version` |
| QP/C++ | commit `435131fdb5ffe5cb383aa16b5cf596a1b462167a` | `FetchContent` pin in the root `CMakeLists.txt` |
| Catch2 | commit `2b60af89e23d28eefc081bc930831ee9d45ea58b` | `FetchContent` pin in `runtime/application/tests/CMakeLists.txt` |
| CMake Format | `0.6.13` | Installed tool observed locally; not yet part of CI |
| Clang format / clang-tidy | LLVM `18` tools in CI | `clang-format-18` and `clang-tidy-18` Ubuntu packages; scoped to the application core by pre-commit |
| libFuzzer | LLVM `18` plus host libstdc++ | Clang's C++23 mode and Ubuntu's GCC 13 standard library disagree on the `__cpp_concepts` gate used by libstdc++'s `<expected>`; the fuzzer target locally raises that feature macro only for that target to expose the C++23 `std::expected` declaration |

The application targets C++23. C++26 library facilities and contracts are deliberately not used:
the host compiler used for validation is GCC 13.3, and cross-toolchain support has not been
qualified consistently for host and ESP32.

Before release, choose one supported ESP-IDF baseline, regenerate and commit the component lock
with that release, and run CI using the same immutable image digest. The current lock/image
mismatch is an explicit release blocker. The exact `idf.py size` output is kept as a CI artifact;
a flash/RAM threshold must be set only after partition layout and the measured baseline are
reviewed.

The clang-tidy configuration excludes `modernize-use-trailing-return-type` to retain the project's
existing return-type style, and `bugprone-exception-escape` because the application API is
intentionally `noexcept` and command alternatives are non-throwing value types. It also excludes
`cppcoreguidelines-avoid-magic-numbers` because embedded protocol constants and fixed buffer sizes
are intentionally explicit, and two bounds checks that flag fixed-size indexing. These exclusions
should be reviewed when application behavior changes; they do not prove bounds safety.
