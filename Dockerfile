FROM alpine:3.11.3

RUN apk add --no-cache gcc g++ make musl-dev

WORKDIR /project

COPY . .

RUN make all

CMD ["./smokers"]