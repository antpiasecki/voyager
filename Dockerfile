FROM alpine:3.19.0 as builder
WORKDIR /app
COPY . .
RUN apk add g++ cmake make openssl-dev
WORKDIR /app/build
RUN cmake ..
RUN make

FROM alpine:3.19.0 as runner
WORKDIR /voyager
COPY --from=builder /app/build/voyager ./voyager
RUN apk add libssl3 libstdc++
ENTRYPOINT ["./voyager"]
EXPOSE 1965
