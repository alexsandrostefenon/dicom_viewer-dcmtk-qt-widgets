FROM debian:trixie

RUN apt-get update -y \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y locales tzdata ca-certificates build-essential qt6-tools-dev libdcmtk-dev \
	&& apt-get clean -y \
	&& rm -rf /var/lib/apt/lists/*

RUN sed -i -e 's/# en_US.UTF-8 UTF-8/pt_BR.UTF-8 UTF-8/' /etc/locale.gen && locale-gen

ENV TZ="America/Sao_Paulo"
ENV LANG pt_BR.UTF-8
ENV LANGUAGE pt_BR:pt
ENV LC_ALL pt_BR.UTF-8
