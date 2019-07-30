# Level-Zero Tests Contribution Guidelines

## Coding Guidelines

The code written for the Level-Zero Tests repo need to follow a series of coding
standards such that the look and feel of the tests is the same and the code
maintains a standard of C++ standards.

### Code Naming Conventions

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

[given_when_then]: https://martinfowler.com/bliki/GivenWhenThen.html

### C++ Coding standards

* C++11 maximum support
* Avoid C Arrays, replace with `std::array<>` / `std::vector<>`
* Avoid "magic numbers"
* Avoid C-style memory allocations in favor of C++
* Use `nullptr` instead of `NULL`
* Don’t add `void` to empty argument lists
* Use `std::unique_ptr` in place of `std::auto_ptr`

### Code Formatting

* Follow [LLVM code formatting][llvm_code_formatting]

[llvm_code_formatting]: https://llvm.org/docs/CodingStandards.html#source-code-formatting

### Coding Standards Enforcement

* clang-format: Code Formatting before committing changes
* clang-tidy: Naming & Coding standards verified at build time if enabled
* Manual: Code Review checking (hopefully minimal due to above)


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
verification is currently for your changes to successfully build on Ubuntu
18.04. Other distros will be added down the road, and Windows builds are still
in the process of being enabled.

There are two ways to verify your changes: automated GitLab pipelines, and
manually using the provided Dockerfiles.

### Automated GitLab Pipelines

The automated GitLab pipelines will automatically build changes for which you
open a merge request on all of the target platforms. Currently, an Ubuntu 18.04
pipeline has been enabled. Even if you aren't ready to merge your changes yet,
you can create a WIP merge request by prepending `WIP: ` to the title, which
will prevent it from being accidentally merged.

You can also manually trigger a GitLab pipeline to run on your branch from the [CI/CD][ci_cd_page]
page by clicking the **Run Pipeline** button.

### Manual Verification

Your changes can be verified manually against Ubuntu 18.04 by using the provided
[Dockerfile](./docker/ubuntu1804/Dockerfile) and [ci-build-linux.sh](./ci-build-linux.sh)
build script.

You will need these prerequisites:

- Docker installed on your development machine. The community edition is
  supported on the following platforms:
  - [Ubuntu][docker_ubuntu]
  - [Fedora][docker_fedora]
  - [Debian][docker_debian]
  - [CentOS][docker_centos]
- Configure Docker to [use the Intel proxies][docker_proxy]. Use
  `http://proxy-chain.intel.com:911` for the HTTP and HTTPS proxy servers.
- An API token for the GFX Assets FM Artifactory server. To generate one:
  - Login to the [web app][artifactory_web_login].
  - Go to your profile by clicking on your name in the upper-right hand corner.
  - Enter your login password when prompted to unlock your profile.
  - Generate an API key.
- A Gerrit GFX HTTP password (similar to an API key). To generate one:
  - Login to [gerrit][gerrit_login].
  - Click on your name in the upper-right hand corner and choose **Settings**.
  - Click **HTTP Password**.
  - If you have already previously generated one, copy it to the clipboard.
    Otherwise, generate a new one.

Once you have these prerequisites, the next step is to create the build image,
which will be the Docker image that your changes will be built in. This is the
same builder image that is used by the automated pipelines. From the root of
your checkout of this repository, run the following:

```
docker build \
  -t level_zero_tests-ubuntu1804 \
  --build-arg GFX_ASSETS_FM_JFROG_TOKEN=<ARTIFACTORY API KEY> \
  ./docker/ubuntu1804
```

This will create the image locally and tag it as `level_zero_tests-ubuntu1804`.
Be sure to replace `<ARTIFACTORY API KEY>` with the API key you generated as a
prerequisite.

Now that the build image has been created, you can perform a build of your
changes, using the following:

```
docker run \
  --rm \
  -v ${PWD}:/root/project \
  -w /root/project \
  -e GERRITGFX_HTTP_USER=<IDSID> \
  -e GERRITGFX_HTTP_PASSWORD=<GERRIT GFX HTTP PASSWORD> \
  -v ${PWD}/ccache:/root/project/ccache \
  -e CCACHE_DIR=/root/project/ccache \
  -e CCACHE_BASEDIR=/root/project \
  level_zero_tests-ubuntu1804 \
  ./ci-build-linux.sh
```

Be sure to replace `<IDSID>` with your Intel IDSID (e.g., `ledettwy`), and
`<GERRIT GFX HTTP PASSWORD>` with the gerrit HTTP password you generated as a
prerequisite. The gerrit HTTP password can sometimes contain special characters,
so you may need to quote it in the command with 'single quotes'.

The above command will map your current working directory (the root of this
repo) into the build container and perform a build in a `build` directory
(ensure it doesn't already exist). ccache is also used to speed-up subsequent
compilations, and will store the data in the `ccache/` subdirectory.

If the build succeeds, it will produce the source tarball and binary package
artifacts in the root of this repository. You can use both of these being
successfully produced as a verification of your changes.

The build image is also general-purpose enough to be usable as an interactive
development environment, as it has all of the build tools and irepo installed
and ready to go in it. To use the image for interactive development, you can
launch a container like:

```
docker run \
  -it \
  --rm \
  -v ${PWD}:/root/project \
  -w /root/project \
  level_zero_tests-ubuntu1804
```

This will launch a container with a bash shell in the root of this repository,
where you can use irepo, cmake, and git for manually building.

[ci_cd_page]: https://gitlab.devtools.intel.com/one-api/level_zero_tests/pipelines
[docker_ubuntu]: https://docs.docker.com/install/linux/docker-ce/ubuntu/
[docker_fedora]: https://docs.docker.com/install/linux/docker-ce/fedora/
[docker_debian]: https://docs.docker.com/install/linux/docker-ce/debian/
[docker_centos]: https://docs.docker.com/install/linux/docker-ce/centos/
[docker_proxy]: https://docs.docker.com/network/proxy/
[artifactory_web_login]: https://gfx-assets.fm.intel.com/artifactory/webapp/#/login
[gerrit_login]: https://gerrit-gfx.intel.com/login/

## Approvals

TODO: decide on approvers etc. for each class of tests. Maybe leverage a
CODEOWNERS file?
