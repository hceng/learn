#!/bin/bash

####################################################################
# Warning:
# Ubuntu 12.04.2 introduced a new X stack that isn't compatible with
# with the old glx packages.

# ------------------------------------------------------------------
# 12.04                --> 12.04.2 (*-lts-quantal)
# ------------------------------------------------------------------
# libgl1-mesa-glx:i386 --> libgl1-mesa-glx-lts-quantal:i386
# libgl1-mesa-dev      --> libgl1-mesa-dev-lts-quantal
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
	libgl1-mesa-glx-lts-quantal:i386 libgl1-mesa-dev-lts-quantal \
	g++-multilib mingw32 tofrodos libncurses5-dev:i386 \
	python-markdown libxml2-utils xsltproc zlib1g-dev:i386


if [ ! -h /usr/lib/i386-linux-gnu/libGL.so ]; then
	FA_DoExec ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 \
		/usr/lib/i386-linux-gnu/libGL.so
fi

# Development support
FA_DoExec apt-get install \
	vim dos2unix minicom gawk

