build:
	make -C lib


setup:
	cd toolchains/musl-patched && \
		mkdir ./build && \
		./configure --prefix=`pwd`/build --enable-static --enable-shared && \
		make -j`nproc` && \
		make install
