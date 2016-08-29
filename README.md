## libnew

Count allocations at runtime, print them at exit.

### Building

```
make
```

### Testing

```
make test
```

### Usage

On OS X:

```
DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libnew.dylib ./your-command
```

On Linux:

```
LD_PRELOAD=libnew.so ./your-command
```