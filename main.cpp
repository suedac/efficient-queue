#include <iostream>
#include <vector>

#define MAX_MEMORY 128
#define CHUNK_SIZE 8
unsigned char data[MAX_MEMORY] = {0}; // Total Memory
typedef void Q;

void on_out_of_memory() {
  std::cout << "Error! Out of memory!" << std::endl;
  abort();
}
void on_illegal_operation() {
  std::cout << "Error! Illegal operation!" << std::endl;
  abort();
}

void initialize_memory() {
  // Index 0 and 1 are reserved to hold the next available free space
  // Therefore we use short int (2 byte size) type to store the value
  // Therefore our first available free space is at index 2
  short int *val = reinterpret_cast<short int *>(data);
  *val = 2;
}

void print_memory() {
  // For debugging purposes, use when MAX_MEMORY is something low like 32
  uint16_t *first_empty_index = reinterpret_cast<uint16_t *>(data);
  printf("\n %d\n", *first_empty_index);
  for (int i = 0; i < MAX_MEMORY; i++) {
    printf("%d /", data[i]);
  }

  printf("\n----------------\n");
}

uint16_t create_chunk() {
  // This function is called to an empty byte,
  // Creates the neccessary data structures for holding the chunk
  // First byte     -> Length of the chunk itself, including control bytes
  // Second byte    -> Length of the string of actual values within the ch  unk
  // (E.g. a chunk might be 16 bytes to provide some free space, but we might
  // have 5 bytes of actual data)
  // Last two bytes -> Next linked chunk's address, or FF if it's the last chunk
  // Returns the index of the first byte of the chunk

  // TODO: Invert the bits of first bytes so searching for 0 doesn't stumble
  // upon them

  // Get start index by converting the first two bytes of the array to short int
  uint16_t *first_empty_index = reinterpret_cast<uint16_t *>(&data);
  uint16_t start_index = *first_empty_index;

  for (int i = 0; i < CHUNK_SIZE; i++) {
    if (data[start_index + i] == 0) {
      continue;
    } else {
      // Set first byte -> Lenth of the chunk itself
      // Second byte (len of actual values) is already 0 for new chunks, so we
      // can skip it
      data[start_index] = i;
      // Assuming this is the last chunk for now
      data[start_index + i - 2] = 0xFF;
      data[start_index + i - 1] = 0xFF;
      return start_index;
      // TODO handle finding next free space
    }
  }
  // If we didn't return in the previous loop
  // that means we had an 16 byte free chunk
  // Still, check the next 8 just in case
  for (int i = 0; i < 8; i++) {
    if (data[start_index + CHUNK_SIZE + i] != 0) {
      // TODO handle if there's not enough space in front
      exit(-31);
    }
  }
  data[start_index + CHUNK_SIZE - 2] = 0xFF;
  data[start_index + CHUNK_SIZE - 1] = 0xFF;
  data[start_index] = CHUNK_SIZE;
  *first_empty_index = start_index + CHUNK_SIZE;
  std::cout << std::endl;

  return start_index;
}

Q *create_queue() {

  // Possible undefined behaviour
  // get the first empty index from first 2 bytes of the data;
  uint16_t *first_empty_index = reinterpret_cast<uint16_t *>(data);

  uint16_t index = *first_empty_index;
  // Create queue (store index of first related chunk)
  uint16_t *queue = reinterpret_cast<uint16_t *>(data + index);

  // Update the firts empty index

  *first_empty_index = *first_empty_index + 2 * sizeof(unsigned char);

  uint16_t x = create_chunk();
  *queue = x;

  return queue;
}

bool is_chunk_full(uint16_t chunk_index) {
  return data[chunk_index + 1] >= data[chunk_index] - 4;
}

// Traverse linked chunks to find last chunk

uint16_t traverse_chunk(uint16_t chunk_index) {

  const unsigned char chunkLength = data[chunk_index];

  const uint16_t index = chunk_index + chunkLength - 2;
  const uint16_t next_chunk_index = *reinterpret_cast<uint16_t *>(&data[index]);
  if (next_chunk_index == 65535)
    return chunk_index;
  else
    return traverse_chunk(next_chunk_index);
}

bool can_chunk_expand(uint16_t chunk_index) {
  uint16_t chunk_size = data[chunk_index];
  return data[chunk_index + chunk_size + 1] == 0 && chunk_size < 255;
}
void expand_chunk(uint16_t chunk_index) {
  uint16_t chunk_size = data[chunk_index];
  data[chunk_index]++;
  data[chunk_index + chunk_size] = data[chunk_index + chunk_size - 1];
  data[chunk_index + chunk_size - 1] = data[chunk_index + chunk_size - 2];
}
void enqueue_byte(Q *q, unsigned char b) {

  // Get first chunk inde
  uint16_t *first_chunk_index = reinterpret_cast<uint16_t *>(q);

  // Get last chunk
  uint16_t last_chunk_index = traverse_chunk(*first_chunk_index);

  if (!is_chunk_full(last_chunk_index)) {
    unsigned char *empty_space_index =
        reinterpret_cast<unsigned char *>(&data[last_chunk_index]);
    uint16_t temp = *(empty_space_index + 1) + 2 + *first_chunk_index;
    data[temp] = b;
    *(empty_space_index + 1) += 1;
  } else {
    if (can_chunk_expand(last_chunk_index)) {
      expand_chunk(last_chunk_index);
      unsigned char *empty_space_index =
          reinterpret_cast<unsigned char *>(&data[last_chunk_index]);
      uint16_t temp = *(empty_space_index + 1) + 2 + *first_chunk_index;
      data[temp] = b;
      *(empty_space_index + 1) += 1;
    } else {
      // Chunk is full here so we need to create a new chunk
      // TODO
      exit(-31);
      // uint16_t next_chunk_index = create_chunk();
      // add value to the new chunk
    }
  }

  // check if current chunk has space
  // if chunk full, create new chunk
  // if chunk not full, add value to chunk
}
void destroy_queue(Q *q) {}
unsigned char dequeue_byte(Q *q) {
  uint16_t first_chunk_index = *reinterpret_cast<uint16_t *>(q);
  unsigned char value = data[first_chunk_index + 2];
  data[first_chunk_index + 2] = data[first_chunk_index + 1];
  data[first_chunk_index + 1] = data[first_chunk_index];
  data[first_chunk_index] = 0;
  data[first_chunk_index + 1]--;
  data[first_chunk_index + 2]--;
  uint16_t *ref = reinterpret_cast<uint16_t *>(q);
  *ref += 1;

  // check the first chunk
  // pop the value out of the
  return value;
}

int main() {
  initialize_memory();
  Q *q0 = create_queue();
  enqueue_byte(q0, 0);
  enqueue_byte(q0, 1);
  Q *q1 = create_queue();
  enqueue_byte(q1, 3);
  enqueue_byte(q0, 2);
  enqueue_byte(q1, 4);
  printf("%d ", dequeue_byte(q0));
  printf("%d\n", dequeue_byte(q0));
  enqueue_byte(q0, 5);
  enqueue_byte(q1, 6);
  printf("%d ", dequeue_byte(q0));
  printf("%d\n", dequeue_byte(q0));
  destroy_queue(q0);
  printf("%d ", dequeue_byte(q1));
  printf("%d ", dequeue_byte(q1));
  printf("%d\n", dequeue_byte(q1));
  destroy_queue(q1);
}

/*
The problem is to write a set of functions to manage a variable number of byte
queues, each with variable length, in a small, fixed amount of memory. You
should provide implementations of the following four function        s:

Q *create_queue(); // Creates a FIFO byte queue, returning a handle to it.
void destroy_queue(Q *q); // Destroy an earlier created byte queue.
void enqueue_byte(Q *q, unsigned char b); // Adds a new byte to a queue.
unsigned char dequeue_byte(Q *q); // Pops the next byte off the FIFO queue.

So, the output from the following set of calls:

Q *q0 = create_queue();
enqueue_byte(q0, 0);
enqueue_byte(q0, 1);
Q *q1 = create_queue();
enqueue_byte(q1, 3);
enqueue_byte(q0, 2);
enqueue_byte(q1, 4);
printf("%d", dequeue_byte(q0));
printf("%d\n", dequeue_byte(q0));
enqueue_byte(q0, 5);
enqueue_byte(q1, 6);
printf("%d", dequeue_byte(q0));
printf("%d\n", dequeue_byte(q0));
destroy_queue(q0);
printf("%d", dequeue_byte(q1));
printf("%d", dequeue_byte(q1));
printf("%d\n", dequeue_byte(q1));
destroy_queue(q1);

should be:

0 1
2 5
3 4 6

You can define the type Q to be whatever you want.

Your code is not allowed to call malloc() or other heap management routines.
Instead, all storage (other than local variables in your functions) must be
within a provided array:

unsigned char data[2048];

Memory efficiency is important. On average while your system is running, there
will be about 15 queues with an average of 80 or so bytes in each queue. Your
functions may be asked to create a larger number of queues with less bytes in
each. Your functions may be asked to create a smaller number of queues with more
bytes in each.

Execution speed is important. Worst-case performance when adding and removing
bytes is more important than average-case performance.

If you are unable to satisfy a request due to lack of memory, your code should
call a provided failure function, which will not return:

void on_out_of_memory();

If the caller makes an illegal request, like attempting to dequeue a byte from
an empty queue, your code should call a provided failure function, which will
not return:

void on_illegal_operation();

There may be spikes in the number of queues allocated, or in the size of an
individual queue. Your code should not assume a maximum number of bytes in a
queue (other than that imposed by the total amount of memory available, of
course!) You can assume that no more than 64 queues will be created at once.*/