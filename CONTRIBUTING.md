# Level-Zero Tests Contribution Guidelines

## C++ Coding standards

* C++11 maximum support
* Avoid C Arrays, replace with `std::array<>` / `std::vector<>`
* Avoid "magic numbers"
* Avoid C-style memory allocations in favor of C++
* Use `nullptr` instead of `NULL`
* Don’t add `void` to empty argument lists
* Use `std::unique_ptr` in place of `std::auto_ptr`

In addition, these naming conventions should be followed:

* Class - UpperCamelCase - `class MyClass`
* Class data member - snake_case_with_suffix - `MyClass::my_class_data_member_`
* Struct - UpperCamelCase - `struct MyStruct`
* Struct data member - snake_case - `MyStruct::my_struct_data_member`
* Function - snake_case - `void my_function()`
* Variable - snake_case - `int my_variable`
* Constant - snake_case - `const int my_constant`
* Enum - snake_case - `enum class my_enum`
* Enum member - snake_case - `my_enum::my_enum_member`
* Namespace - snake_case - `namespace my_namespace`
* Macro - CAPITALIZED_WITH_UNDERSCORES - `#define MY_MACRO`
* Module - snake_case - `my_module`
* GTEST Test cases will follow the [Given/When/Then naming convention][given_when_then]

A [`.clang-tidy`](./.clang-tidy) file is included in this repository to help
catch issues in your code related to these. To include these checks in your
build, set `-DCLANG_TIDY_CHECK=ON` when configuring the cmake project.

**This is still experimental, and a lot of the code currently checked-in is not
compliant, so expect to see a lot of messages if you run this.**

[given_when_then]: https://martinfowler.com/bliki/GivenWhenThen.html

## Code Formatting

Follow the [LLVM code formatting guidelines][llvm_code_formatting].

A [`.clang-format`](./.clang-format) file is included in this repository, and
the GitLab build automation will fail the build if your merge request has
improperly formatted code.

Build targets are provided in the cmake for convenience. `clang-format` will
check and fix any formatting issues with your code, and `clang-format-check`
will check for issues and print a diff of the corrections required.

Examples:

```
cmake --build . --target clang-format-check
cmake --build . --target clang-format
```

[llvm_code_formatting]: https://llvm.org/docs/CodingStandards.html#source-code-formatting

## Kernels (SPV files)

OpenCL C kernel source code (`.cl`) and binaries (`.spv`) should be placed in a
`kernels/` subdirectory of your conformance or performance test. The
`add_perf_test` and `add_conformance_test` CMake functions will search for
kernels in that directory.

Whenever you add or update any kernels, you must re-generate the SPIR-V binaries
and commit them with your changes. A specialized build of clang is required to
do this.

A [Dockerfile][clang-spv] is available in the one-api/devops GitLab repository
which specifies an image containing this specialized build of clang. You can
use it to generate an SPV file from an OpenCL C file (`.cl`).

Building the Docker image:

```
docker build -t clang-spv ./devops/docker/clang-spv
```

Using it to compile a kernel file called `xe_nano_benchmarks.cl` in the current
directory:

```
docker run
  --rm
  -v $PWD:$PWD
  -w $PWD
  clang-spv
  -o xe_nano_benchmarks.spv
  xe_nano_benchmarks.cl
```

This will soon be automated as part of the GitLab build automation, removing the
requirement to manually build them and check them in to the repository. For now,
please continue to manually build and check-in SPV files.

clang-spv: https://gitlab.devtools.intel.com/one-api/devops/blob/master/docker/clang-spv/Dockerfile

## Synchronizing Third Party Assets

A fresh git repo checkout will require that the required third party assets are
synchronized with a tool called [irepo](https://github.intel.com/GSDI/irepo).
Ensure you install and use irepo from a unix account with the username matching
your Intel IDSID so authentication works correctly.

> **_NOTE:_** Even though devtool (successor to irepo) is used in the build
> automation, irepo is still the recommended tool for developer checkouts. The
> current version of devtool does potentially unwanted things to your workspace,
> such as removing all your existing git hooks and replacing them with its own.

Once irepo is installed, synchronize the third-party assets:

```
irepo select dependencies.yml
irepo sync -c googletest
irepo sync -c yuv_samples
```

Additionally, synchronize the `level_zero_linux` component for building on Linux
without any prior versions of level-zero installed, and the `level_zero_windows`
component for building on Windows. If you do this, also specify the `L0_ROOT`
cmake argument to point to whichever level-zero directory gets created in
`third_party`.

You should synchronize these components whenever their entries are updated in
the [`dependencies.yml`](./dependencies.yml) file.

If you opt to use devtool (`dt`) insetad of irepo, the commands are under the
`workspace` subcommand, e.g., `dt workspace select dependencies.yml`.

## Updating Third Party Assets

Third party assets are specified in the [`dependencies.yml`](./dependencies.yml)
file, and the versions of each can be updated there.

The level-zero build/runtime dependency is specified by a version string, like
`0.2.2-2189`. The first component is the level-zero version, and the second
component is the depth of the commit that the artifact was created from (to
allow artifacts for commits between official "releases" to exist). Simply update
this version string in the [`dependencies.yml`](./dependencies.yml) file to
change the version of level-zero used. In general, the level_zero_linux and
level_zero_windows components should have the same version string.

To determine the version string to use for a given commit on the `integration`
branch of `level_zero_gpu_driver`, take the level-zero version of that commit
(e.g., `0.2.2`, check the top of `level_zero/CMakeLists.txt`) and append the
commit depth to it, which can be obtained by running
`git rev-list --count <COMMIT HASH>` on the commit you want. Alternately, check
the log of the deploy-artifactory job for the GitLab pipeline corresponding to
that commit, which should be visible on the commit's GitLab page (example: [commit page][gitlab_commit_page_example],
[job log][gitlab_commit_page_job_log]).

[gitlab_commit_page_example]: https://gitlab.devtools.intel.com/one-api/level_zero_gpu_driver/commit/2b168dd54dccaa147a7d80e8f97477caffa4917c
[gitlab_commit_page_job_log]: https://gitlab.devtools.intel.com/one-api/level_zero_gpu_driver/-/jobs/1846290

## Submitting Changes

[Merge requests][mrdoc] are used for proposing changes and doing code review.

Merge requests should be created from your personal branches that you've pushed
to the repository. Please do not create merge requests from forks of the
repository, as the CI will not be run.

If you have recently pushed to a branch in the project, you will see a handy
blue button on the [merge requests page][mrs] to create a new merge request from
that branch.

![Create a merge request from recently pushed branch][img:create_mr_from_branch]

The preferred naming convention for branches you push is `<idsid>/<identifier>`.

Example:

```
$ git checkout master
Switched to branch 'master'
Your branch is up to date with 'origin/master'.

$ git checkout -b ledettwy/contributing_file
Switched to a new branch 'ledettwy/contributing_file'

$ git add CONTRIBUTING.md

$ git commit -s -m "Add CONTRIBUTING.md"
[ledettwy/contributing_file 6092b15] Add CONTRIBUTING.md
 1 file changed, 18 insertions(+)
 create mode 100644 CONTRIBUTING.md

$ git push origin ledettwy/contributing_file
Warning: Permanently added '[gitlab.devtools.intel.com]:29418,[10.9.194.20]:29418' (ECDSA) to the list of known hosts.
Welcome to Intel Hardened OS
Enumerating objects: 4, done.
Counting objects: 100% (4/4), done.
Delta compression using up to 72 threads
Compressing objects: 100% (3/3), done.
Writing objects: 100% (3/3), 706 bytes | 706.00 KiB/s, done.
Total 3 (delta 1), reused 0 (delta 0)
remote:
remote: To create a merge request for ledettwy/contributing_file, visit:
remote:   https://gitlab.devtools.intel.com/one-api/level_zero_tests/merge_requests/new?merge_request%5Bsource_branch%5D=ledettwy%2Fcontributing_file
remote:
To ssh://gitlab.devtools.intel.com:29418/one-api/level_zero_tests.git
 * [new branch]      ledettwy/contributing_file -> ledettwy/contributing_file
```

You can optionally specify that the remote source branch used for the merge
request get deleted after the request is accepted (I almost always do this).

Please mark the option to squash all your commits when the request is
accepted.

![Delete source branch and squash commits][img:delete_and_squash]

[mrdoc]: https://docs.gitlab.com/ee/user/project/merge_requests/
[mrs]: https://gitlab.devtools.intel.com/one-api/level_zero_tests/merge_requests
[img:create_mr_from_branch]: doc/create_mr_from_branch.png
[img:delete_and_squash]: doc/create_mr_squash_commits.png

## Updating Changes

A change that you've submitted through a merge request can be updated by adding
additional commits to the branch the merge request was created from. These
commits do not need to be detailed, as the commit message from the first commit
will be used when they are squashed. CI automation will be executed every time
you push new commits to the branch, and the results of the CI jobs will be
visible in the merge request.

![Multiple commits in a merge request][img:many_changes]

Example:

```
$ git add CONTRIBUTING.md

$ git commit -s -m "update contributing.md"
[ledettwy/contributing_file 3114b43] update contributing.md
 1 file changed, 61 insertions(+), 2 deletions(-)

$ git push origin ledettwy/contributing_file
Warning: Permanently added '[gitlab.devtools.intel.com]:29418,[10.9.194.20]:29418' (ECDSA) to the list of known hosts.
Welcome to Intel Hardened OS
Enumerating objects: 5, done.
Counting objects: 100% (5/5), done.
Delta compression using up to 72 threads
Compressing objects: 100% (3/3), done.
Writing objects: 100% (3/3), 1.51 KiB | 1.51 MiB/s, done.
Total 3 (delta 1), reused 0 (delta 0)
remote: 
remote: View merge request for ledettwy/contributing_file:
remote:   https://gitlab.devtools.intel.com/one-api/level_zero_tests/merge_requests/13
remote: 
To ssh://gitlab.devtools.intel.com:29418/one-api/level_zero_tests.git
   6092b15..3114b43  ledettwy/contributing_file -> ledettwy/contributing_file
```

[img:many_changes]: doc/create_mr_multiple_commits.png

## Reviewing Changes

Comments and suggested changes can be submitted as code reviews from the Changes
tab in the merge request. Hover over a line number to insert a comment on a
line or set of lines.

![Add a suggested change][img:mr_suggestion]

Suggested changes can also [span multiple lines][mr_suggestion_multiline].

After you have finished adding all of the review comments and suggestions, you
can finish the review to submit them.

![Finishing a review][img:mr_finish_review]

The author of the merge request will be able to respond to your comments and
suggestions, and also optionally apply the suggestions directly as commits from
the web interface.

![Applying a suggestion][img:mr_apply_suggestion]

[img:mr_suggestion]: doc/mr_suggestion.png
[mr_suggestion_multiline]: https://docs.gitlab.com/ee/user/discussions/#multi-line-suggestions
[img:mr_finish_review]: doc/mr_finish_review.png
[img:mr_apply_suggestion]: doc/mr_apply_suggestion.png

## Verifying Changes

Changes should be verified before being merged into master. The minimum level of
verification is currently for your changes to successfully build on Ubuntu 18.04
and Windows and execute on Ubuntu 18.04 with no unintended regressions.

There are two ways to verify your changes: automated GitLab pipelines, and
manually using the provided Dockerfiles. For all the manual examples, your
current working directory should be the root of this repository checkout.

### Automated GitLab Build and Run Pipelines

The automated GitLab pipelines will automatically build changes for which you
open a merge request on all of the target platforms. Even if you aren't ready to
merge your changes yet, you can create a WIP merge request by prepending `WIP: `
to the title, which will prevent it from being accidentally merged.

You can also manually trigger a GitLab pipeline to run on your branch from the
[CI/CD][ci_cd_page] page by clicking the **Run Pipeline** button.

After building your changes, all the tests will be executed against fulsim-gen9
on Ubuntu 18.04. With merge requests to master, a result report will be
displayed as a panel on the merge request page. The report shows newly failing
(comparerd to the target branch, i.e. `master`) cases highlighted at the top,
cases that continue to fail following, and newly passing cases listed at the
very bottom. You should review this report and make sure your changes are not
introducing any unintended regressions.

![Test reports in merge requests][img:mr_test_report]

[img:mr_test_report]: doc/mr_test_report.png

### Manual Builds with Docker

You can build your changes locally using the provided Dockerfiles and CI build
scripts. You will need these prerequisites:

- Docker installed on your development machine. The community edition is
  supported on the following platforms:
  - [Ubuntu][docker_ubuntu]
  - [Fedora][docker_fedora]
  - [Debian][docker_debian]
  - [CentOS][docker_centos]
  - [Windows 10][docker_win10]
- Configure Docker to [use the Intel proxies][docker_proxy]. Use
  `http://proxy-chain.intel.com:911` for the HTTP and HTTPS proxy servers.

Once you have these prerequisites, the next step is to create the build image,
which will be the Docker image that your changes will be built in. This is the
same builder image that is used by the automated pipelines. From the root of
your checkout of this repository, run the following:

```
docker build
  -t level_zero_tests-build
  ./docker/build-<platform>
```

This will create the image locally and tag it as `level_zero_tests-build`.

Now that the build image has been created, you can perform a build of your
changes, using the following:

Linux example:

```bash
docker run
  --rm -ti
  -v ${PWD}:${PWD}
  -w ${PWD}
  -v ${PWD}/ccache:/ccache
  -e CCACHE_DIR=/ccache
  -e CCACHE_BASEDIR=/root/project
  level_zero_tests-build
  ./ci-build-linux.sh
```

Windows example:

```cmd
docker run
  --rm -ti
  -v %cd%:C:\project
  -w C:\project
  -v C:\clcache:C:\clcache
  -e CLCACHE_DIR=C:\clcache
  level_zero_tests-build
  .\ci-build-windows.bat
```

The above command will map your current working directory (the root of this
repo) into the build container and perform a build in a `build` directory
(ensure it doesn't already exist). ccache and clcache are also used to speed-up
subsequent compilations.

The build image is also general-purpose enough to be usable as an interactive
development environment, as it has all of the build tools and a safe version of
devtool (patched to remove the unwanted behavior described in the
[Synchronizing Third Party Assets](#synchronizing-third-party-assets) section)
installed and ready to go in it.

Linux example:

```bash
docker run
  --rm -ti
  -v ${PWD}:${PWD}
  -w ${PWD}
  -v ${PWD}/ccache:/ccache
  -e CCACHE_DIR=/ccache
  -e CCACHE_BASEDIR=/root/project
  level_zero_tests:latest
```

Windows example:

```cmd
docker run
  --rm -ti
  -v %cd%:C:\project
  -w C:\project
  -v C:\clcache:C:\clcache
  -e CLCACHE_DIR=C:\clcache
  level_zero_tests:latest
```

This will launch a container with an interactive shell in the root of this
repository, where you can use devtool (`dt`), cmake, and git for manually
building.

[ci_cd_page]: https://gitlab.devtools.intel.com/one-api/level_zero_tests/pipelines
[docker_ubuntu]: https://docs.docker.com/install/linux/docker-ce/ubuntu/
[docker_fedora]: https://docs.docker.com/install/linux/docker-ce/fedora/
[docker_debian]: https://docs.docker.com/install/linux/docker-ce/debian/
[docker_centos]: https://docs.docker.com/install/linux/docker-ce/centos/
[docker_win10]: https://docs.docker.com/docker-for-windows/install/
[docker_proxy]: https://docs.docker.com/network/proxy/
[artifactory_web_login]: https://gfx-assets.fm.intel.com/artifactory/webapp/#/login

### Manual Execution with Docker

You can execute the tests locally using the provide docker runtime images.
Execution in this manner is only supported on Linux.

First, build the runtime image, which will be used for the container that the
test binary itself executes in. This image does not include any of the NEO
runtime libraries, so you will need to supply those yourself. If you are using
the `level_zero_linux` irepo/devtool dependency for runtime, the NEO libraries
needed are included with that.

```bash
docker build
  --build-arg QB_USER
  --build-arg QB_PASSWORD
  -t level_zero_tests-runtime
  ./docker/runtime-<platform>
```

Next, build the fulsim image. This requires a user account and password capable
of authenticating with artifactory to download the fulsim binaries. The AGS
entitlement for this is
`Gfx Artifactory - gfx-compute-fulsim-assets-igk - readers`.

Assuming the artifactory username and password are stored in the
`ARTIFACTORY_USER` and `ARTIFACTORY_PASSWORD` environment variables:

```bash
docker build
  --build-arg ARTIFACTORY_USER
  --build-arg ARTIFACTORY_PASSWORD
  --build-arg FULSIM_NAME=gen9
  --build-arg FULSIM_VERSION=r36707
  -t level_zero_tests-fulsim:gen9-r36707
  ./docker/fulsim
```

Launch the fulsim container with:

```bash
docker run
  --rm -ti
  --name fulsim
  level_zero_tests-fulsim:gen9-r36707
  AubLoad -device skl.2.a0 -socket tcp
```

After fulsim has finished launching, you can execute a test binary in a runtime
container (append any additional arguments to pass the the binary as you
normally would). In this example, the `level_zero_linux` irepo/devtool
dependency is used to supply the level-zero and NEO runtime libraries.

```bash
docker run
  --rm -ti
  -v ${PWD}:${PWD}
  -w ${PWD}/out/ubuntu1804/conformance_tests
  --net=container:fulsim
  -e SetCommandStreamReceiver=2
  -e CreateMultipleRootDevices=2
  -e LD_LIBRARY_PATH=${PWD}/third_party/level_zero_linux/lib
  level_zero_tests-runtime-ubuntu1804
  ./test_xe_p2p
```

## Approvals

Approvers for your changes are automatically determined based on entries in the [`CODEOWNERS`](./CODEOWNERS)
file. You will need at least two approvals total, and one approval from each
section identified in the [`CODEOWNERS`](./CODEOWNERS) file that your change
updates.

## Generating a Source Distribution

Ensure the project directory is free of any extra files that aren't normally
part of the repository and aren't generated. An easy way to do this is to clone
a new copy of the repository independent from your regular development checkout.

Ensure that you have synchronized the third party assets so that they get
included in the source package. Do not include the level_zero components, as
those are for CI and local development purposes.

```
mkdir build
cd build
cmake ..
cmake --build . --config Release --target package_source
```

This will generate `.zip` and `.tar.gz` files of the source code suitable for
distributing to other users to build themselves.

## Versioning and Releases

Continuous delivery is used for releases, wherein every patch merged to master
will be pushed to artifactory with an appropriate version string. The version
string is based on the cmake `project()` version specified in the top-level [CMakeLists.txt](./CMakeLists.txt)
suffixed by the commit depth as computed by `git rev-list --count <COMMIT>`
(e.g., `0.2.2-110`). The cmake version should match the version of level-zero
with which the tests are compatible.

These releases are available at https://gfx-assets.fm.intel.com/artifactory
under `gfx-sandbox-fm/level-zero-{perf,conformance}/<PLATFORM>/<VERSION>` (e.g.,
`gfx-sandbox-fm/level-zero-conformance/ubuntu1804/0.2.2-110`).
