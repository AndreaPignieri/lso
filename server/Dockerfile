FROM gcc:13
WORKDIR /app
COPY . .
RUN gcc -Wall -pthread -o server functions.c server.c 
EXPOSE 8080
CMD ["./server"]