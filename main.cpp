#include <iostream>
#include <vector>

#define MAX_MEMORY 2048
#define CHUNK_SIZE 16
#define MIN_CONTINUOUS_SPACE 6
unsigned char data[MAX_MEMORY] = {0}; // Total Memory

typedef void Q;
void on_out_of_memory();
void on_illegal_operation();
void is_q_pointer_valid(Q *q);
void initialize_memory();
void print_memory();
uint16_t create_chunk();
Q *create_queue();
bool is_chunk_full(uint16_t chunk_index);
bool can_chunk_expand(uint16_t chunk_index);
bool expand_chunk(uint16_t chunk_index);
void enqueue_byte(Q *q, unsigned char b);
void delete_chunks_recursive(uint16_t chunk_index);
void destroy_queue(Q *q);
unsigned char dequeue_byte(Q *q);
void find_next_free_space();

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

void on_out_of_memory() {
  std::cout << "Error! Out of memory!" << std::endl;
  abort();
}
void on_illegal_operation() {
  std::cout << "Error! Illegal operation!" << std::endl;
  abort();
}

void is_q_pointer_valid(Q *q) {
  // Reminder - A Q pointer is just a 16 bit address to the
  // first chunk of the queue. If it's 0, the Q was deletrd.
  uint16_t *first_chunk_index = reinterpret_cast<uint16_t *>(q);
  if (*first_chunk_index == 0) {
    on_illegal_operation();
  }
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

  // TODO: Invert the bits of first bytes so  searching for 0 doesn't stumble
  // upon them
  // NVM - The uin16_t type is big-endian bit-wise, so we don't need to do
  // anything As the first bit will probably hold some value other than 0

  // Get start index by converting the first two bytes of the array t`o short
  // int
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
      find_next_free_space();
      return start_index;
      // TODO handle finding next free space
    }
  }
  find_next_free_space();
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

bool expand_chunk(uint16_t chunk_index) {
  uint16_t chunk_size = data[chunk_index];

  // Check if there is enough space in the data array to expand
  if (chunk_index + chunk_size < MAX_MEMORY) {
    // Check if the chunk is not already at the maximum size
    if (chunk_size < 255) {
      data[chunk_index]++;
      data[chunk_index + chunk_size] = data[chunk_index + chunk_size - 1];
      data[chunk_index + chunk_size - 1] = data[chunk_index + chunk_size - 2];
      return true; // Expansion successful
    } else {
      // Handle the case when the chunk is already at the maximum size
      std::cout << "Error! Cannot expand chunk beyond maximum size."
                << std::endl;
      return false; // Expansion failed
    }
  } else {
    // Handle the case when expanding would exceed the boundaries of the data
    // array
    std::cout << "Error! Cannot expand chunk beyond data array boundaries."
              << std::endl;
    return false; // Expansion failed
  }
}

void enqueue_byte(Q *q, unsigned char b) {
  is_q_pointer_valid(q);

  uint16_t *first_chunk_index = reinterpret_cast<uint16_t *>(q);
  uint16_t last_chunk_index = traverse_chunk(*first_chunk_index);

  if (!is_chunk_full(last_chunk_index)) {
    // Add the value to the current chunk
    unsigned char *empty_space_index =
        reinterpret_cast<unsigned char *>(&data[last_chunk_index]);
    uint16_t temp = *(empty_space_index + 1) + 2 + *first_chunk_index;
    data[temp] = b;
    *(empty_space_index + 1) += 1;
  } else {
    // Try to expand the current chunk
    if (can_chunk_expand(last_chunk_index) && expand_chunk(last_chunk_index)) {
      // If expansion is successful, add the value to the expanded chunk
      unsigned char *empty_space_index =
          reinterpret_cast<unsigned char *>(&data[last_chunk_index]);
      uint16_t temp = *(empty_space_index + 1) + 2 + *first_chunk_index;
      data[temp] = b;
      *(empty_space_index + 1) += 1;
    } else {
      // Chunk was full and it couldn't be expanded, create new chunk
      // This create_chunk() function returns the adress of the chunk it created
      // We need to add it to the last two bytes of the last chunk
      uint16_t new_chunk_address = create_chunk();
      unsigned char chunk_length = data[last_chunk_index];
      uint16_t *next_chunk_pointer = reinterpret_cast<uint16_t *>(
          &data[last_chunk_index + chunk_length - 2]);
      *next_chunk_pointer = new_chunk_address;

      enqueue_byte(q, b);
    }
  }
}

void delete_chunks_recursive(uint16_t chunk_index) {
  const unsigned char chunkLength = data[chunk_index];

  // Last two bytes holds the next chunk's address
  const uint16_t last_two_bytes_index = chunk_index + chunkLength - 2;
  const uint16_t next_chunk_index =
      *reinterpret_cast<uint16_t *>(&data[last_two_bytes_index]);
  if (next_chunk_index == 65535) {
    // Delete the last chunk
    for (int i = 0; i < chunkLength; i++) {
      data[chunk_index + i] = 0;
    }
    return;
  } else {
    // Delete the current chunk, then move on to the next one
    for (int i = 0; i < chunkLength; i++) {
      data[chunk_index + i] = 0;
    }
    delete_chunks_recursive(next_chunk_index);
  }
}

void destroy_queue(Q *q) {
  is_q_pointer_valid(q);
  // Reminder - A Q pointer is just a 16 bit address to the
  // first chunk of the queue
  uint16_t *first_chunk_index = reinterpret_cast<uint16_t *>(q);

  // This will fill all the chunks with 0
  delete_chunks_recursive(*first_chunk_index);

  // And this just zeroes our q pointer thingy
  *first_chunk_index = 0;
}

unsigned char dequeue_byte(Q *q) {
  is_q_pointer_valid(q);

  uint16_t first_chunk_index = *reinterpret_cast<uint16_t *>(q);
  unsigned char byte = data[first_chunk_index + 2];

  unsigned char byte_in_queue_count = data[first_chunk_index + 1];
  // Last two bytes holds the next chunk's address
  const unsigned char chunkLength = data[first_chunk_index];
  const uint16_t last_two_bytes_index = first_chunk_index + chunkLength - 2;
  const uint16_t next_chunk_index =
      *reinterpret_cast<uint16_t *>(&data[last_two_bytes_index]);
  if (byte_in_queue_count == 0)
    on_illegal_operation();
  if (byte_in_queue_count == 1 && next_chunk_index != 65535) {
    // If there's one byte left in the chunk, we might as well delete the chunk
    // Set the Q pointer to the next chunk
    uint16_t *next_chunk_address =
        reinterpret_cast<uint16_t *>(&data[next_chunk_index]);
    q = next_chunk_address;
    for (int i = 0; i < chunkLength; i++) {
      data[first_chunk_index + i] = 0;
    }
    return byte;
  }

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
  return byte;
}

void find_next_free_space() {
  // Find the next available free space index
  uint16_t current_index = 0;
  uint16_t continuous_zeroes_count = 0;

  while (current_index < MAX_MEMORY) {
    // Check if the current byte is empty (contains 0)
    if (data[current_index] == 0) {
      continuous_zeroes_count++;

      // Check if we have found a sequence of continuous zeroes
      if (continuous_zeroes_count >= MIN_CONTINUOUS_SPACE) {
        // Update the first two bytes of the data array with the next free space
        // index
        uint16_t *first_empty_index = reinterpret_cast<uint16_t *>(data);
        *first_empty_index = current_index - continuous_zeroes_count + 1;

        return;
      }
    } else {
      continuous_zeroes_count = 0;
    }

    // Move to the next chunk
    uint16_t chunk_length = data[current_index];
    current_index += chunk_length;

    // Skip the last two bytes of the chunk (next linked chunk's address)
    current_index += 2;
  }

  // If no free space is found, update the first two bytes with MAX_MEMORY
  uint16_t *first_empty_index = reinterpret_cast<uint16_t *>(data);
  *first_empty_index = MAX_MEMORY;

  on_out_of_memory();
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
each. Your functions may be asked to create a smaller number of queues with
more bytes in each.

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