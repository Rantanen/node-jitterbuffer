node-jitterbuffer
=========
### NodeJS native bindings to libspeex jitter buffer

    var jitter = require('jitterbuffer').JitterBuffer;
    var JB = jitter.JitterBuffer;
    var jb = new JB();

    jb.put({ data: new Buffer(10), timestamp: 10, span: 10 });
    jb.put({ data: new Buffer(10), timestamp: 30, span: 10 });
    jb.put({ data: new Buffer(10), timestamp: 20, span: 10 });
    jb.put({ data: new Buffer(10), timestamp: 50, span: 10 });

    jb.get(10).timestamp; // 10
    jb.get(10).timestamp; // 20
    jb.get(10).timestamp; // 30
    jb.get(10)            // jitter.MISSING
    jb.get(10).timestamp; // 50


Platform support
----------------

Supported platforms:
- Linux x64

Add new supported platforms by running ./configure in deps/speex-1.2rc1 and
copying the following files:

- config.h
- include/speex/speex_config_types.h

to `deps/config/celt-0.7.1/<os>/<arch>` maintaining the relative path. See
the existing platforms for an example.

Use the following flags: --enable-static --disable-shared --with-pic

