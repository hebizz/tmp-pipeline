# pipeline Deployment process


# install docker  

- apt-get install docker

#Configure the Docker daemon by typing a json Docker daemon configuration file.
- touch /etc/docker/daemon.json
- echo  ‘’’{
"insecure-registries" : [
"registry.jiangxingai.com:5000"
]
}  ‘’’  > /etc/docker/daemon.json

- systemctl restart docker

### docker execute nginx-rtmp

- docker pull registry.jiangxingai.com:5000/base/nginx-rtmp:x8664-cpu-0.1.0

- docker run -d  registry.jiangxingai.com:5000/base/nginx-rtmp:x8664-cpu-0.1.0


# install  redis for c++

- wget http://download.redis.io/releases/redis-4.0.9.tar.gz

- tar xzf redis-4.0.9.tar.gz

- cd redis-4.0.9

- make

- src/redis-server

### 在执行的时候如果出现动态库无法加载
## If errors:

- Vim /etc/ld.so.conf
i- nclude /usr/local/lib

#Fixed  and execute

- /sbin/ldconfig

- Install  lhiredis

- git clone https://github.com/redis/hiredis

- make install  

### compile
- g++ redis.c -o redis -lhiredis


# install  redis  for python

- Pip  install redis


# install   supervisor

- apt-get install supervisor

- supervisor.conf  default path：/etc/supervisor   ==>>>   modify【include】

#run start supervisor

- supervisord -c /etc/supervisor/supervisord.conf      

### create daemon file 

- touch rkmpp.ini  

*********************
[program:rkmpp]
command=bash run.sh
directory=/usr/local/rkmpp/bin
autostart=true 
autorestart=true  
startretries=20 
stdout_logfile=/usr/local/rkmpp/bin/rkmpp.log 

********************

### start rkmpp.ini
- Supervisorctl start rkmpp

# install opencv
- apt-get install libopencv-dev

### execute opencv file 
- ./a.out  

# install bluetooth  - - only support Ubuntu systems

- apt-get install libbluetooth-dev
#execute bluetooth file
- ./sender 
- ./controller


