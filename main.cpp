#include <iostream>
#include <vector>

#define MAX_MEMORY 2048
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
  for (int i = 0; i < MAX_MEMORY; i++) {
    std::cout << data[i] << " ";
  }
}

void create_chunk(unsigned char *current_pointer) {
  // This function is called to an empty byte,
  // Creates the neccessary data structures for holding the chunk
  // First byte     -> Length of the chunk itself
  // Second byte    -> Length of the string of actual values within the chunk
  // (E.g. a chunk might be 16 bytes to provide some free space, but we might
  // have 5 bytes of actual data)
  // Last two bytes -> Next linked chunk's address, or FF if it's the last chunk

  // ----0000000000------------

  auto start_pointer = current_pointer;
  // Offset for the 2 starter and 2 ender bytes of chunks
  current_pointer += 4;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    if (*current_pointer == 0) {
      current_pointer++;
      continue;
    } else {
      // Set first byte -> Lenth of the chunk itself
      *start_pointer = i - 1;
      // Second byte (len of actual values) is already 0, so we can skip it
    }
  }
}

Q *create_queue() {
  unsigned char *que_ptr = data;
  // unsigned char first_2_bytes[] = {data[0], data[1]};

  // Possible undefined behaviour
  short *startIndex = reinterpret_cast<short int *>(data);

  que_ptr += (*startIndex) * sizeof(unsigned char);
  create_chunk(que_ptr);
  return que_ptr;
}

bool is_chunk_full(short chunk) {
  short *empty_space_index = reinterpret_cast<short *>(q);

  short *last_index =
      reinterpret_cast<short *>(empty_space_index + sizeof(empty_space_index));
  return *last_index == *empty_space_index;
}

// Traverse linked chunks to find last chunk
short traverse_chunk(short chunk) {
  short *ptr_to_first_chunk = reinterpret_cast<short *>(chunk);
  short *next_chunk_index =
      reinterpret_cast<short *>(data[*ptr_to_first_chunk + 2]);
  if (*next_chunk_index == 0)
    return chunk;
  else
    return traverse_chunk(*next_chunk_index);
}

void enqueue_byte(Q *q, unsigned char b) {
  // Get first chunk index 
  short *first_chunk_index = reinterpret_cast<short *>(q);
  
  // Get last chunk
  short last_chunk_index = traverse_chunk(*first_chunk_index);

  if (!is_chunk_full(last_chunk_index)) {
    short *empty_space_index = reinterpret_cast<short *>(q);
    data[*empty_space_index] = b;
    *empty_space_index += 1;
  } else {
    // Chunk is full here so we need to create a new chunk
    short *startIndex = reinterpret_cast<short int *>(data);
    unsigned char *que_ptr = data;
    que_ptr += (*startIndex) * sizeof(unsigned char);
    create_chunk(que_ptr);
    // add value to the new chunk
  }

  // check if current chunk has space
  // if chunk full, create new chunk
  // if chunk not full, add value to chunk
}
void destroy_queue(Q *q) {}
unsigned char dequeue_byte(Q *q) {
  // check the last chunk
  // pop the value out of the
}

int main() {
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
}

/*
The problem is to write a set of functions to manage a variable number of byte
queues, each with variable length, in a small, fixed amount of memory. You
should provide implementations of the following four functions:

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