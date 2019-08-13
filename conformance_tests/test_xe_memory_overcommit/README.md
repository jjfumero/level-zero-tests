# test_device_memory_overcommit

Test device memory allocation to demonstrate that
when we allocate more device memory than the
device has physically, the device
can effectively page memory onto and off of the
device, without that memory being corrupted.

A memory pattern test is used to test for
memory corruption.
