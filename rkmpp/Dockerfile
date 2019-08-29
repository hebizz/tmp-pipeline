FROM arm64v8/ubuntu

RUN apt-get update \
  && apt-get install --no-install-recommends -y \
    gtk-doc-tools \
    libjpeg-dev 

COPY ./rkmpp /usr/local/rkmpp

RUN cd /usr/local/rkmpp \
  && cp mpp.conf /etc/ld.so.conf.d/mpp.conf \
  && rm -f /usr/local/rkmpp/lib/*.so.0 \
  && ldconfig -v 


ENTRYPOINT [ "bash", "/usr/local/rkmpp/bin/run.sh" ]

