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

## Build Automation

No build automation has been setup yet.