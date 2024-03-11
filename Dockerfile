FROM archlinux:latest

RUN mkdir /home/user
WORKDIR /home/user
ENV HOME "/home/user"

# install dependecies:
RUN pacman -Syu --noconfirm && pacman -S --noconfirm gcc make ncurses libyaml curl && rm -rf /var/cache/pacman/pkg/*

# libcyaml
RUN curl -LO https://github.com/tlsa/libcyaml/archive/refs/tags/v1.4.1.tar.gz

RUN gunzip v1.4.1.tar.gz && tar -xf v1.4.1.tar && cd libcyaml-1.4.1 && make && make install && cd .. && echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf && ldconfig

# copy source files and compile
COPY . termtypetest
RUN cd termtypetest && make

# Run on image startup
ENTRYPOINT ["sh", "-c", "typetest"]
