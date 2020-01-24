# Open-Sourcing / Upstreaming Plan for `one-api/level_zero_tests`

## Schedule

- EoQ1 2020 - [VLCLJ-774](https://jira.devtools.intel.com/browse/VLCLJ-774)
  - Development and CI build switched to tracking Level-Zero spec (loader)
    instead of GPU driver implementation.
    [VLCLJ-776](https://jira.devtools.intel.com/browse/VLCLJ-776)
  - **Upstream & Delete Internal** files open-sourced to upstream repository
    - Open-sourcing / upstreaming branch created in internal `level_zero_tests`
      GitLab repository
    - **Keep Internal** and **Delete** files removed from upstreaming branch
    - Upstreaming branch locked in internal `level_zero_tests` GitLab
      repository, checked-out, origin changed to upstream repository, pushed to
      upstream repository's `master` branch.
  - New contribution documentation authored in open-source repository.
    [VLCLJ-775](https://jira.devtools.intel.com/browse/VLCLJ-775)
  - All future contribution happens in open-source repository
  - **Delete** files removed from internal `level_zero_tests:master` branch
  - Internal `level_zero_tests` GitLab repository kept for internal CI, contains
    CI scripts / internal docker images, *possibly renamed for clarity*.
    [VLCLJ-777](https://jira.devtools.intel.com/browse/VLCLJ-777)
    [VLCLJ-778](https://jira.devtools.intel.com/browse/VLCLJ-778)
- Q2-Q4 2020
  - Public CI enabled, adapted and migrated from internal CI.
    - Build
    - Publish release artifacts (GitHub releases)
    - Regression test
  - Internal CI and internal `level_zero_tests` GitLab repository archived

## Developer Process

*Do we have any special obligations ouside of standard Open Source PDT process
that we need to fulfill in order to enable us to perform development directly in
upstream repositories?*

Yes, in theory all code that we submit (i.e. pull requests) needs to have
scanning done on it. Need to work out how that scanning will take place and
implications (if any) for pre-merge CI.

## Manifest

Here is a complete manifest of the repository with dispostions partitioned
between **Delete**, **Keep Internal**, and **Upstream & Delete Internal**.

- `ci/*` - **Keep Internal**
- `cmake/*` - **Upstream & Delete Internal**
- `conformance_tests/*` - **Upstream & Delete Internal**
- `doc/*` - **Delete**
- `docker/`
  - `docker/build-ubuntu1804.Dockerfile` - **Delete**
  - `docker/build-windows.Dockerfile` - **Delete**
  - `docker/fulsim.Dockerfile` - **Keep Internal**
  - `docker/pylzt.Dockerfile` - **Keep Internal**
  - `docker/runtime-ubuntu1804.Dockerfile` - **Delete**
- `mediadata/*` - **Upstream & Delete Internal**
- `perf_tests/*` - **Upstream & Delete Internal**
- `utils/*` - **Upstream & Delete Internal**
- `.clang-format` - **Upstream & Delete Internal**
- `.clang-tidy` - **Upstream & Delete Internal**
- `.gitignore` - **Keep Internal**
- `.gitlab-ci.yml` - **Keep Internal**
- `BUILD.md` - **Upstream & Delete Internal**
- `CMakeLists.txt` - **Upstream & Delete Internal**
- `CODEOWNERS` - **Keep Internal**
- `CONTRIBUTING.md` - **Delete**
- `README.md` - **Keep Internal**
- `LICENSE` - **Upstream & Delete Internal**
- `clang-format-patch.sh` - **Upstream & Delete Internal**
- `dependencies.yml` - **Keep Internal**
- `upstreaming_plan.md` - **Keep Internal**
- `*.spv` - **Delete**

## Versioning

[VLCLJ-779](https://jira.devtools.intel.com/browse/VLCLJ-779)

The tests need to be versioned in a way that supports these use-cases:

- Vendors (like us) want to pull old versions of the tests to make sure their
  implementation didn't break anything for backwards-compatibility.
- Vendors want to pull old versions of the tests that correspond to the version
  of the spec they are writing their implementation against to validate that
  their implementation fully supports that version of the spec.
- Vendors found a bug in an old version of the tests, and they want to
  contribute a fix for it so that it can be used to validate their
  implementation properly.

The tests will be written and released against versions of the spec. If
possible, the tests will also be sanity-checked against any available
implementations that are supposed to conform to that version of the spec.

Each test must specify which versions of the spec for which it is valid. A
release of the tests against a version of the spec means that the tests provide
coverage for up to and including that version. Tests need to query the version
of the spec from the loader and decide whether or not they are valid to run.
Registration of the tests with gtest will be dynamic based on this version check
(https://github.com/google/googletest/blob/master/googletest/docs/advanced.md#registering-tests-programmatically).

**Needs additional research and scoping**

## Automation

Several public Dockerfiles will exist that define *generic* suitable build and
execution environments for supported platforms. These will be based off of their
currently existing counterparts. Both the internal and public CI pipelines will
use these directly.

- `build-*.Dockerfile`
  - Linux and Windows container images suitable for building the tests.
  - Visual Studio 2019 product key is an argument to the windows Dockerfile, so
    internal CI pipeline will continue to use Intel internal product key. Public
    CI pipeline can use either Community Edition (free for open-source projects)
    or Intel internal product key (as long as key is stored privately).
  - **Estimated work: 2 days**
- `runtime-*.Dockerfile`
  - Linux (and possibly Windows) container images suitable for running the
    tests. No third-party product license terms apply.
  - **Current scope of this is evolving.**
  - **Estimated work: ?**

*Need to outline the process that we as upstream developers/maintainers will go
through for submitting changes and getting pre-merge CI results
(build/lint/test).*

### Internal CI (EoQ1 2020)

[VLCLJ-777](https://jira.devtools.intel.com/browse/VLCLJ-777)
[VLCLJ-778](https://jira.devtools.intel.com/browse/VLCLJ-778)

The internal GitLab repository will retain only the existing CI automation
toolchain. A small shim layer will be authored by Lisanna in NEO Jenkins which
will trigger the internal `level_zero_tests` GitLab repository's retained CI
pipelines. This should be in-place before the open-sourcing happens so
contributions can continue uninterrupted.

CI results for `master` and pull requests will only be available *internally*
through the GitLab project. Maintainers (us) reference this as we're authoring
our pull requests and reviewing any external contributions. Results from these
internal CI pipelines are **not pushed** to the open-source repository to guard
against inadvertent internal data/IP exposure.

We can't keep using the GitLab MR process for doing regression reporting because
we will no longer be opening GitLab MRs. Need to write a generic component that
can generate a regression report.

**Estimated work: 1-2 weeks.**

### Public CI (Q2-Q4 2020)

Lisanna will implement and enable a public CI at some point during Q2-Q4 2020.
This will be very similar to the existing build/test/deploy CI run internally,
but will be fully visible in the upstream repository. This must use only
external resources.

The internal https://gitlab.devtools.intel.com/ledettwy/pylzt repository
contains code that supports the regression execution of the tests. This will
also need to be open-sourced in order to enable regression testing as part of
any public CI (GPU driver and/or tests). All of the code in this repository is
completely original (Intel copyright/IP) except for `setup.py`, which is a
trivial derivative from common example Python code
(https://packaging.python.org/tutorials/packaging-projects/#creating-setup-py).
The code in this repository will need scanning / copyright / licensing
disposition.

There are plenty of free infrastructure offerings that we will be eligible for
because the tests will be open-source:

- Azure DevOps Pipelines
  - Good GitHub integration
  - Unlimited minutes for open-source projects
  - 10 parallel pipelines for open-source projects
  - Solid Linux + Windows container build and execution support
- GitHub Actions
  - Built-in to GitHub
  - Unlimited minutes for public projects
  - Windows container support limited, custom solution **might** need to be
    engineered.
- Travis CI
  - Currently used by `intel/compute-runtime`
  - Needs additional research
- Semaphore CI
  - Currently used by `intel/compute-runtime`
  - Needs additional research
- Shippable
  - Currently used by `intel/compute-runtime`
  - Needs additional research
- Others worth considering?

Lisanna's current tentative choice is **Azure DevOps Pipelines**.

**This effort needs additional scoping**, and will be enabled at some point
after EoQ1 2020.

**Estimated work: ?**

## Third-Party Components

Google Test will become a submodule of the upstream repository pointing to
https://github.com/google/googletest.git. Currently this is just pulled with
irepo/devtool. Trivial change.

The tests dynamically link against libpng and zlib and statically link against
boost. `ze_nano` dynamically links against libpapi.

No binary executables are checked-in to the repository.

### Level-Zero Dependence

[VLCLJ-776](https://jira.devtools.intel.com/browse/VLCLJ-776)

The tests are currently built, developed, and tested against the Level-Zero GPU
driver implementation and loader. This will need to be changed to developed and
built against the Level-Zero spec loader, and tested against the GPU driver
implementation and loader.

**Estimated work: 1 week**

## Licensing

[VLCLJ-780](https://jira.devtools.intel.com/browse/VLCLJ-780)

MIT to match NEO and Level-Zero spec. See included [LICENSE](./LICENSE) file.

### Source File Header

Source files (including CMake files!) should all contain following header text
(formatted appropriately as a block comment and with the correct years)
indicating licensing under MIT.

```
Copyright (C) 2018-2020 Intel Corporation

SPDX-License-Identifier: MIT
```

C/C++ example:
```cpp
/*
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
```

CMake example:
```cmake
# Copyright (C) 2018-2020 Intel Corporation
# SPDX-License-Identifier: MIT
```

- [ ] All `.c`, `.cpp`, `.cl`, and `.h` files contain correct version of MIT
  license header.
- [ ] All `.cmake` and `CMakeLists.txt` files contain correct version of MIT
  license header.

### clpeak and `xe_peak` / `ze_peak`

clpeak and derivatives can be re-licensed at our discretion
(https://github.com/krrishnarraj/clpeak/blob/master/LICENSE). `xe_peak` (should
be changed to `ze_peak`) is a derivative of clpeak, and will be re-licensed
under MIT.
- [ ] Get formal approval from Intel Legal to license `ze_peak` as MIT.

### clPingPong and `xe_pingpong` / `ze_pingpong`

clPingPong is licensed under MIT license
(https://github.com/krrishnarraj/clpeak/blob/master/LICENSE). `xe_pingpong`
(should be changed to `ze_pingpong`) is a derivative of clpeak, and will be
re-licensed under MIT.
- [ ] Get formal approval from Intel Legal to license `ze_pingpong` as MIT.

## Naming and Branding

[VLCLJ-781](https://jira.devtools.intel.com/browse/VLCLJ-781)

"`ZE`" is the appropriate prefix to use for name prefixes for things that
require them (e.g., `ZeSomething`, `ze_peak`). Where possible, don't use a
prefix at all.

- [ ] Perf test renaming:
  - [ ] Rename `xe_bandwidth` to `ze_bandwidth`
  - [ ] Rename `xe_image_copy` to `ze_image_copy`
  - [ ] Rename `xe_nano` to `ze_nano`
  - [ ] Rename `xe_peak` to `ze_peak`
  - [ ] Rename `xe_peer` to `ze_peer`
  - [ ] Rename `xe_pingpong` to `ze_pingpong`
- [ ] Conformance test gtest name renaming (some test suite and parameterization
  names start with `xe`):
  - [ ] Rename `xeCLInteropTests` to `zeCLInteropTests`
  - [ ] Rename `xeDevicGetP2PPropertiesTests` to `zeDevicGetP2PPropertiesTests`
  - [ ] Rename `xeSetCacheConfigTests` to `zeSetCacheConfigTests`
  - [ ] Rename `xeHostEventSyncPermuteTimeoutTests` to `zeHostEventSyncPermuteTimeoutTests`
  - [ ] Rename `xeSharedMemGetPropertiesTests` to `zeSharedMemGetPropertiesTests`
  - [ ] Rename `xeSharedMemGetAddressRangeTests` to `zeSharedMemGetAddressRangeTests`
  - [ ] Rename `xeHostMemGetAddressRangeParameterizedTests` to `zeHostMemGetAddressRangeParameterizedTests`
  - [ ] Rename `xeHostSystemMemoryDeviceTests` to `zeHostSystemMemoryDeviceTests`
  - [ ] Rename `xeMemAccessTests` to `zeMemAccessTests`
  - [ ] Rename `xeMemAccessCommandListTests` to `zeMemAccessCommandListTests`
  - [ ] Rename `xeMemAccessCommandListTests_1` to `zeMemAccessCommandListTests_1`
  - [ ] Rename `xeMemAccessDeviceTests` to `zeMemAccessDeviceTests`
  - [ ] Rename `xeP2PTests` to `zeP2PTests`
  - [ ] Rename `xeP2PKernelTestsAtomicAccess` to `zeP2PKernelTestsAtomicAccess`
  - [ ] Rename `xeP2PKernelTestsConcurrentAccess` to `zeP2PKernelTestsConcurrentAccess`

## SPIR-V Kernels

[VLCLJ-384](https://jira.devtools.intel.com/browse/VLCLJ-384)

Automate the SPV building, so they don't need to be checked-in to the repository
in binary format. We shouldn't have to have these binaries in the open-source
repository.

The upstream repository should contain instructions on how to compile the `.cl`
files to `.spv` files using clang. Internal CI should do this as part of its
build according to the linked JIRA task.

- [ ] No `.spv` files checked-in to upstream, instructions provided for
  compiling them (perhaps include Dockerfiles).

**Estimated work: 1 week**
