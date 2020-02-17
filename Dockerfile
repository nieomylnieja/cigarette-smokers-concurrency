FROM alpine:3.11.3

RUN apk add --no-cache gcc g++ make musl-dev

WORKDIR /smokers

COPY . .

RUN make all

CMD ["./main"