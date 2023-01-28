#!/bin/bash  

if [ ! -d "libmysofa" ]; then
	git clone https://github.com/hoene/libmysofa.git

	# Really hard to patch this - patch tools are really annoying. Something with linefeeds
	patch -R -p0 --binary < patch-src.patch
	patch -R -p0 --binary < patch-cml.patch
	
	cp -r libs/zlib libmysofa/windows/third-party
fi
