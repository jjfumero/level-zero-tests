# Level-Zero Tests Maintainers Guide

## Synchronizing Third Party Assets

A fresh git repo checkout will require that the required third party assets are
synchronized with a tool called [irepo](https://github.intel.com/GSDI/irepo).
Ensure you install and use irepo from a unix account with the username matching
your Intel IDSID so authentication works correctly.

Currently `google-mock` is a dependency, which we pull in from the `googletest`
irepo dependency.

Once irepo is installed, synchronize the third-party assets:

```
irepo select linux.yml
irepo sync
```

This should be done every time the `linux.yml` file is updated.

## Generating a Source Distribution

Ensure the project directory is free of any extra files that aren't normally
part of the repository and aren't generated. An easy way to do this is to clone
a new copy of the repository independent from your regular development checkout.

Ensure that you have synchronized the third party assets so that they get
included in the source package.

```
mkdir build
cd build
cmake ..
make package_source
```

This will generate `.zip` and `.tar.gz` files of the source code suitable for
distributing to other users to build themselves.

## Build Automation

No pipelines have been created yet, but you can manually re-create what the
pipeline would do on your local machine. Set the following environment variables
that will be provided by GitLab:
- `GITLAB_ONEAPI_READKEY`: a personal access token that you've created in
  [GitLab](https://gitlab.devtools.intel.com/profile/personal_access_tokens).
- `GERRITGFX_HTTP_USER`: your Intel IDSID.
- `GERRITGFX_HTTP_PASSWORD`: an HTTP password you've generated in
  [Gerrit](https://gerrit-gfx.intel.com/#/settings/http-password).

```
docker build \
    -t level_zero_tests-ubuntu1804:latest \
    --build-arg GITLAB_ONEAPI_READKEY=$GITLAB_ONEAPI_READKEY \
    ./docker/ubuntu1804

docker run --rm -v \
    ${PWD}:/project -w /project \
    --env-file ./ci-env \
    level_zero_tests-ubuntu1804:latest \
    ./ci-build.sh
```