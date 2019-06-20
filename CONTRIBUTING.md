# Level-Zero Performance Tests Contributing Guide

## Synchronizing Third Party Assets

A fresh git repo checkout will require that the required third party assets are
synchronized with a tool called [irepo](https://github.intel.com/GSDI/irepo).
Ensure you install and use irepo from a unix account with the username matching
your Intel IDSID so authentication works correctly.

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

No build automation has been setup yet.