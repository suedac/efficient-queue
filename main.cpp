#include <iostream>
#include <vector>

#define MAX_MEMORY 32
int PAGE_SIZE = 8; // This is not a #define as this will be dynamically adjusted later

unsigned char data[MAX_MEMORY]; // Total Memory
unsigned char *first_empty = &data[0];

struct Q
{
    unsigned char *start;
    unsigned char *end;
    int size = 0;
};

std::vector<Q> queue_objects;

void on_out_of_memory()
{
    std::cout << "Error! Out of memory!" << std::endl;
    abort();
}
void on_illegal_operation()
{
    std::cout << "Error! Illegal operation!" << std::endl;
    abort();
}

void defrag(){

}

void print_memory() {
    for (int i = 0; i < MAX_MEMORY / PAGE_SIZE; i++)
    {
        for (int j = 0; j < PAGE_SIZE; j++)
        {
            std::cout << data[i*j];
        }
        std::cout << " ";
    }
}

Q *create_queue()
{
    Q temp_queue = Q();
    if (queue_objects.empty())
    {
        temp_queue.start = &data[0];
        temp_queue.end = &data[0];
        queue_objects.push_back(temp_queue);
        return &temp_queue;
    }
    // This loop looks for empty space between adjacent queues
    for (int i = 0; i < queue_objects.size() - 1; i++)
    {
        // If there's more than PAGE_SIZE difference between two adjacent queues
        if ((queue_objects[i + 1].start - queue_objects[i].end) > PAGE_SIZE)
        {
            //Put the new queue right next to the left one
            temp_queue.start = queue_objects[i].end + ((uintptr_t)queue_objects[i].end & PAGE_SIZE); 
            queue_objects.insert(queue_objects.begin() + i+1, temp_queue);
            return &temp_queue;
        }
    }
    // If there's no empty space found between queues, we need to add to end
    if((MAX_MEMORY - (uintptr_t)queue_objects.back().end) > PAGE_SIZE) 
    {
        temp_queue.start = queue_objects.back().end + ((uintptr_t)queue_objects.back().end & PAGE_SIZE);
        temp_queue.end = temp_queue.start;
    }
    else {
        defrag();
    } 
}

void enqueue_byte(Q *q, unsigned char b)
{   
    // Check if we're currently within our page bounds
    if ((uintptr_t)queue_objects.back().end & PAGE_SIZE < PAGE_SIZE)
    {

    }
    else // If not, we need to check if the next byte belongs to some other queue
    {

    }
    
}
void destroy_queue(Q *q)
{
}
unsigned char dequeue_byte(Q *q)
{
}

int main()
{
    Q *q0 = create_queue();
    queue_objects[0].size = 5;
    std::cout << queue_objects[0].size << std::endl;
    q0->size = 6;
    std::cout << queue_objects[0].size << std::endl;
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
The problem is to write a set of functions to manage a variable number of byte queues, each with variable length, in a small, fixed amount of memory. You should provide implementations of the following four functions:

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
Instead, all storage (other than local variables in your functions) must be within a provided array:

unsigned char data[2048];

Memory efficiency is important. On average while your system is running, there will be about 15 queues with an average of 80 or so bytes in each queue. Your functions may be asked to create a larger number of queues with less bytes in each. Your functions may be asked to create a smaller number of queues with more bytes in each.

Execution speed is important. Worst-case performance when adding and removing bytes is more important than average-case performance.

If you are unable to satisfy a request due to lack of memory, your code should call a provided failure function, which will not return:

void on_out_of_memory();

If the caller makes an illegal request, like attempting to dequeue a byte from an empty queue, your code should call a provided failure function, which will not return:

void on_illegal_operation();

There may be spikes in the number of queues allocated, or in the size of an individual queue. Your code should not assume a maximum number of bytes in a queue (other than that imposed by the total amount of memory available, of course!) You can assume that no more than 64 queues will be created at once.*/