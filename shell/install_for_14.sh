#!/bin/bash

####################################################################
# Warning:
# Ubuntu 16.04.1 introduced a new X stack that isn't compatible with
# with the old glx packages.

# ------------------------------------------------------------------
# 16.04                --> 16.04.1 (*-lts-xenial)
# ------------------------------------------------------------------
# libgl1-mesa-glx:i386 --> libgl1-mesa-glx-lts-xenial:i386
# libgl1-mesa-dev      --> libgl1-mesa-dev-lts-xenial
# ------------------------------------------------------------------
#
# DON'T install libgl1-mesa-glx:i386 on 12.04.2, else it will *BREAK*
# your system.
####################################################################

# Execute an action
FA_DoExec() {
	echo "==> Executing: '${@}'"
	eval $@ || exit $?
}

# Ubuntu 12.04.2 LTS
FA_DoExec apt-get install \
	git gnupg flex bison gperf build-essential \
	zip curl libc6-dev libncurses5-dev libncurses5-dev:i386 x11proto-core-dev \
	libx11-dev:i386 libreadline6-dev:i386 \
	libgl1-mesa-glx-lts-trusty:i386 libgl1-mesa-dev-lts-trusty \
	g++-multilib mingw32 tofrodos libncurses5-dev:i386 \
	python-markdown libxml2-utils xsltproc zlib1g-dev:i386


if [ ! -h /usr/lib/i386-linux-gnu/libGL.so ]; then
	FA_DoExec ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 \
		/usr/lib/i386-linux-gnu/libGL.so
fi

# Development support
FA_DoExec apt-get install \
	vim dos2unix minicom gawk


B
B
B
B
B
A
A
A
B
B
B
B
B
B
A
A
A
B
B
B
A
A
A
B
B
B
A
A
A

