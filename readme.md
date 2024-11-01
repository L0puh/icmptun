simple icmp data transfer from client to server.

### usage:
run server:
```sh
make && ./build/sstream -s 127.0.0.1 &
```
and send file from client 
```sh
make && ./build/sstream -c 127.0.0.1 -f myfile.txt
```
