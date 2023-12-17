# voyager

[Gemini](https://geminiprotocol.net/) server written in C++

## Setup

```
mkdir build
cd build
cmake ..
make
openssl req -nodes -x509 -new -keyout key.pem -out cert.pem -days 365
./voyager
```

## TODO
- [x] Concurrency
- [ ] Static file hosting