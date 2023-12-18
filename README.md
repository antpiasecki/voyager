# voyager

[Gemini](https://geminiprotocol.net/) server written in C++

## Setup

```bash
# Build
mkdir build
cd build
cmake ..
make

# Generate certificate
mkdir data
openssl req -nodes -x509 -new -keyout data/key.pem -out data/cert.pem -days 365

./voyager
```

### Docker
```bash
docker build -t voyager .
mkdir data
openssl req -nodes -x509 -new -keyout data/key.pem -out data/cert.pem -days 365
docker run --name=voyager -d --restart=always -v ./data:/voyager/data -p 1965:1965 voyager
```

## TODO
- [x] Concurrency
- [ ] Static file hosting