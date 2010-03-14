#!/bin/sh

PATH=/bin:/sbin:/usr/bin:/usr/sbin
export PATH

SHELL=/bin/sh
RUNCOM=/libexec/rc

mkdir /servers/
mkdir /servers/socket
settrans -c /servers/socket/1 /hurd/pflocal
settrans -c /servers/socket/2 /hurd/pfinet
settrans -ck /servers/exec /hurd/exec
settrans -c /servers/crash-suspend /hurd/crash --suspend
settrans -c /servers/crash-kill /hurd/crash --kill
settrans -c /servers/password /hurd/password
settrans -c /servers/crash-suspend /hurd/crash --suspend
settrans -c /servers/crash-dump-core /hurd/crash --dump-core
settrans -c /servers/default-pager /hurd/proxy-defpager
settrans /proc /hurd/procfs
ln -s crash-kill /servers/crash
ln -s 1 /servers/socket/local
ln -s 2 /servers/socket/inet

# would be better than the -N parameter given to genext2fs
#settrans /dev /hurd/tmpfs 1M
#ln -s /sbin/MAKEDEV /dev

echo "setting up /dev"
cd /dev
rm -f null
rm -f console
./MAKEDEV fd
./MAKEDEV std com0 vcs tty1 tty2 tty3 tty4 tty5 tty6 hd0 hd1 hd2 hd3 loop0 loop1
echo "setting up /dev/pty*"
for i in 0 1 2 3 4
do
	./MAKEDEV ptyp$i ptyq$i
done
echo "setting up /dev/hd*"
for i in 0 1
do
	for j in 1 2 3 4 5 6 7 8
	do
		./MAKEDEV hd${i}s${j}
	done
done
cd /

echo "running rc"
# Run the rc script.  As long as it exits nonzero, punt to single-user.
# After the single-user shell exits, we will start over attempting to
# run rc; but later invocations strip the `autoboot' argument.
until $rc; do
  rc=${RUNCOM}

  # Run single-user shell and repeat as long as it dies with a signal.
  until ${SHELL} || test $? -lt 128; do
    :
  done
done

# Touch the first tty so that the Hurd console is certain to pick it
# and not some random other tty.
touch /dev/tty1

# Startup the Hurd console.
console -d vga -d pc_kbd -c /dev/vcs &

sleep 1

exec < /dev/tty1 > /dev/tty1 2>&1

echo here is the console
echo "Starting d-i's init in 2s, hoping for the best"
sleep 2

/bin/busybox init

echo "d-i init died, please press ctrl-alt-backspace to get back to the Mach console"

# Kill our own console so it doesn't interfere with the emergency shell
kill 7

# Shouldn't even be reached
exit 1
