#!/bin/sh
# If you run a dynamically linked binary under strace you'll notice:
# access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
# which is a file that defines system wide LD_PRELOAD values.
# Normally this probably doesn't exist for you, but if you use namespaces
# you create this for processes under your normal user, and bypass having to
# directly use LD_PRELOAD.
# This script uses this by creating a memfd, making a symlink to it under /tmp
# and performing a bind mount to /tmp to /etc in a mount namespace, and then
# running a random dynamicly linked binary to load whatever shared library you
# defined in the file.

# On ubuntu you need to run this under `busybox sh` (or use another unpriv
# namespace bypass).
# https://www.qualys.com/2025/three-bypasses-of-Ubuntu-unprivileged-user-namespace-restrictions.txt

# Using perl to create memfds that exist for 5 seconds before automatically
# deleting themselves.

# Create a memfd with the name ld.so.preload in tmp
echo '$name = "a"; $fd = syscall(319, $name, 1); $path = "/proc/" . $$ . "/fd/" . $fd; symlink($path, "/tmp/ld.so.preload"); chmod 0666, $path; sleep 5; unlink "/tmp/ld.so.preload"' | perl &

# Create a memfd for the payload
echo '$name = "egg"; $fd = syscall(319, $name, 1); $path = "/proc/" . $$ . "/fd/" . $fd; symlink($path, "/tmp/egg"); chmod 0666, $path; sleep 5; unlink "/tmp/egg"' | perl &

# copy our contents to the memfds
echo /tmp/egg > /tmp/ld.so.preload
cat ./lib/libfoo.so > /tmp/egg
sleep 0.5

# mount /tmp as /etc in the namespace, so dynamically linked binaries use our
# fake ld.so.preload
/usr/bin/unshare -r -m /bin/sh << EOF
mount --bind /tmp/ /etc/
tail -f /dev/null
EOF
