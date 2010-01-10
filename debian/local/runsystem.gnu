#!/bin/sh

PATH=/bin:/sbin:/usr/bin:/usr/sbin
export PATH

SHELL=/bin/sh
RUNCOM=/libexec/rc

# See whether pflocal is setup already, and do so if not (install case)

if ! test -e /servers/socket/1 && test -x /bin/settrans >/dev/null ; then
  /bin/settrans -c /servers/socket/1 /hurd/pflocal
fi

# TODO:
# set up translators, /dev, etc. (./native-install, roughly)

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
console -d vga -d pc_kbd -c /dev/vcs

echo "Starting d-i's init in 2s, hoping for the best"
sleep 2

/init

echo "d-i init died"

exit 1
