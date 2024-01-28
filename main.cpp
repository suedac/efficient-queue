#include <iostream>

using namespace std;

class Q
{
public:
    unsigned char data[2048];
    unsigned char *start;
    unsigned char *end;
    unsigned char *start_of_array = &data[0];
    unsigned char *end_of_array = &data[2047];
    unsigned int length;
};

void on_out_of_memory()
{
    cout << "Error! Out of memory!" << endl;
    abort();
}
void on_illegal_operation()
{
    cout << "Error! Illegal operation!" << endl;
    abort();
}

Q *create_queue()
{
    Q *temp_queue = new Q;
    temp_queue->start = &temp_queue->data[0];
    temp_queue->end = &temp_queue->data[0];
    temp_queue->length = 0;
    return temp_queue;
}

void enqueue_byte(Q *q, unsigned char b)
{
    if (q->length >= 2048)
    {
        on_out_of_memory();
    }
    else
    {
        *q->end = b;
        if (q->end == q->end_of_array)
        {
            q->end = q->start_of_array;
        }
        else
        {
            q->end += sizeof(unsigned char);
        }
        q->length += 1;
    }
}
void destroy_queue(Q *q)
{
    q->~Q();
}
unsigned char dequeue_byte(Q *q)
{
    if (q->length == 0)
    {
        on_illegal_operation();
        return *q->start;
    }
    else
    {

        unsigned char temp;
        temp = *q->start;
        if (q->start == q->end_of_array)
        {
            q->start = q->start_of_array;
        }
        else
        {
            q->start += sizeof(unsigned char);
        }

        q->length -= 1;
        return temp;
    }
}

int main()
{
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