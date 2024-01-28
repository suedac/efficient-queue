#include <iostream>
#define MEMORY_LIMIT 2048

using namespace std;

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

class Q
{
public:
    unsigned char data[MEMORY_LIMIT];
    unsigned char *start;
    unsigned char *end;
    unsigned char *start_of_array = &data[0];
    unsigned char *end_of_array = &data[MEMORY_LIMIT - 1];
    unsigned int length = 0;

    Q() // This replaces create_queue()
    {
        this->start = this->start_of_array;
        this->end = this->start_of_array;
    }

    void enqueue_byte(unsigned char b) // This replaces enqueue_bye(Q* q, unsigned char b)
    {
        if (this->length >= MEMORY_LIMIT)
        {
            on_out_of_memory();
        }
        else
        {
            *this->end = b;
            if (this->end == this->end_of_array)
            {
                this->end = this->start_of_array;
            }
            else
            {
                this->end += sizeof(unsigned char);
            }
            this->length += 1;
        }
    }

    unsigned char dequeue_byte() // This replaces dequeue_bye(Q* q)
    {
        if (this->length == 0)
        {
            on_illegal_operation();
            return 0; //Previous function aborts anyway, this is only to make compiler happy
        }
        else
        {

            unsigned char temp;
            temp = *this->start;
            if (this->start == this->end_of_array)
            {
                this->start = this->start_of_array;
            }
            else
            {
                this->start += sizeof(unsigned char);
            }

            this->length -= 1;
            return temp;
        }
    }

    ~Q() // This replaces destroy_queue(Q* q);
    {
        cout << "Queue destroyed" << endl;
    }
};




int main()
{
    Q q0 = Q();
    q0.enqueue_byte(0);
    q0.enqueue_byte(1);
    Q q1 = Q();
    q1.enqueue_byte(3);
    q0.enqueue_byte(2);
    q1.enqueue_byte(4);
    printf("%d ", q0.dequeue_byte());
    printf("%d\n", q0.dequeue_byte());
    q0.enqueue_byte(5);
    q1.enqueue_byte(6);
    printf("%d ", q0.dequeue_byte());
    printf("%d\n", q0.dequeue_byte());
    printf("%d ", q1.dequeue_byte());
    printf("%d ", q1.dequeue_byte());
    printf("%d\n", q1.dequeue_byte());
}

/*
The problem is to write a set of functions to manage a variable number of byte queues, each with variable length, in a small, fixed amount of memory. You should provide implementations of the following four functions:

Q *new Q(); // Creates a FIFO byte queue, returning a handle to it.
void destroy_queue(Q *q); // Destroy an earlier created byte queue.
void enqueue_byte(Q *q, unsigned char b); // Adds a new byte to a queue.
unsigned char dequeue_byte(Q *q); // Pops the next byte off the FIFO queue.

So, the output from the following set of calls:

Q *q0 = new Q();
enqueue_byte(q0, 0);
enqueue_byte(q0, 1);
Q *q1 = new Q();
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

unsigned char data[MEMORY_LIMIT];

Memory efficiency is important. On average while your system is running, there will be about 15 queues with an average of 80 or so bytes in each queue. Your functions may be asked to create a larger number of queues with less bytes in each. Your functions may be asked to create a smaller number of queues with more bytes in each.

Execution speed is important. Worst-case performance when adding and removing bytes is more important than average-case performance.

If you are unable to satisfy a request due to lack of memory, your code should call a provided failure function, which will not return:

void on_out_of_memory();

If the caller makes an illegal request, like attempting to dequeue a byte from an empty queue, your code should call a provided failure function, which will not return:

void on_illegal_operation();

There may be spikes in the number of queues allocated, or in the size of an individual queue. Your code should not assume a maximum number of bytes in a queue (other than that imposed by the total amount of memory available, of course!) You can assume that no more than 64 queues will be created at once.*/