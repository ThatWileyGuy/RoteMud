FROM greyltc/archlinux-aur:yay

RUN set -ex; \
    pacman -Sy --noconfirm base-devel cmake ninja boost; \
    pacman -S --noconfirm zlib openssl cmocka doxygen python openssh; \
    aur-install libssh-git

COPY . /usr/src/rote

WORKDIR /usr/src/rote/build

RUN set -ex;              \
    cmake -GNinja -S .. -B .; ninja

WORKDIR /opt/rote

RUN cp /usr/src/rote/build/rote_mud /opt/rote

WORKDIR /opt/rote/data

ENTRYPOINT ["../rote_mud"]

