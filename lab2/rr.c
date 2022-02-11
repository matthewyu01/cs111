#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process)
  pointers;

  /* Additional fields here */

  bool response_time;
  u32 time_left;

  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
    (*process_data)[i].response_time = false;
    (*process_data)[i].time_left = (*process_data)[i].burst_time;
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list; // HEADNAME, head
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  struct process *curr;
  u32 time = 0;

  // Sort by arrival time + PID
  for (u32 i = 0; i < size; i++)
  {
    for (u32 j = i + 1; j < size; j++)
    {
      if (data[i].arrival_time > data[j].arrival_time)
      {
        struct process temp_data = data[i];
        data[i] = data[j];
        data[j] = temp_data;
      }
      else if (data[j].arrival_time == data[i].arrival_time)
      {
        if (data[i].pid > data[j].pid)
        {
          struct process temp_data = data[i];
          data[i] = data[j];
          data[j] = temp_data;
        }
      }
    }
  }

  u32 left = 0;                // number of processes that have been added to list
  time = data[0].arrival_time; // set to first arrival time

  u32 list_length = 0;

  // Add first nodes to linked list
  for (u32 i = 0; i < size; i++)
  {
    if (data[i].arrival_time == time)
    {
      TAILQ_INSERT_TAIL(&list, &data[i], pointers);
      list_length++;
      left++;
    }
  }

  u32 time_slice;

  while (!TAILQ_EMPTY(&list))
  {
    curr = TAILQ_FIRST(&list);

    if (curr->response_time == false)
    { // if response time for a node hasn't been calculated already
      curr->response_time = true;
      total_response_time += time - curr->arrival_time;
    }

    time_slice = quantum_length; // quantum length or remaining time left
    if (curr->time_left <= time_slice)
    {
      time_slice = curr->time_left;
    }
    curr->time_left -= time_slice;
    time += time_slice;

    // Add new nodes to linked list
    for (u32 i = left; i < size; i++)
    {
      if (data[i].arrival_time <= time)
      {
        TAILQ_INSERT_TAIL(&list, &data[i], pointers);
        list_length++;
        left++;
      }
    }

    if (curr->time_left > 0)
    {
      // Move node to end of linked list
      struct process *temp = curr;
      TAILQ_REMOVE(&list, curr, pointers);
      TAILQ_INSERT_TAIL(&list, temp, pointers);
    }
    else
    {
      u32 waiting_time = time - curr->arrival_time - curr->burst_time;
      total_waiting_time += waiting_time;
      // Remove node
      TAILQ_REMOVE(&list, curr, pointers);
    }

    curr = TAILQ_NEXT(curr, pointers);
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
