FROM gcc:13
WORKDIR /app
COPY . .
RUN gcc -Wall -pthread -o server \
    main.c \
    clientHandler.c \
    jsonUtils.c \
    tipi.c \
    errors.c \
    cJSON.c
EXPOSE 8080

CMD ["./server"]