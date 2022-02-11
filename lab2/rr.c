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

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */

  u32 wait_time;
  u32 response_time;

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

  // struct process *new_node = malloc(sizeof(struct process));
  // new_node->pid = data[0].pid;

  // TAILQ_INSERT_TAIL(&list, new_node, pointers);

  // TAILQ_REMOVE(&list, new_node, pointers);
  // free(new_node);
  // int time = 0;

  // TAILQ_FIRST(&list);
  // // last
  // TAILQ_LAST(&list, process_list);

  struct process *curr;
  struct process *next;

  int time = 0;

  int first_arrival_time = data[0].arrival_time;


  // Sort by arrival time + PID
  for (int i = 0; i < size; i++)
  {
    for (int j = i + 1; j < size; j++)
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

  for (int i = 0; i < size; i++)
  {
    printf("%d ", data[i].pid);
  }

  // Iterate through data to find first arrival time
  for (int i = 1; i < size; i++)
  {
    if (data[i].arrival_time < first_arrival_time)
    {
      first_arrival_time = data[i].arrival_time;
    }
  }

  int left = 0;
  time = first_arrival_time; // set time to first time

  // Add first nodes to linked list
  for (int i = 0; i < size; i++)
  {
    if (data[i].arrival_time == time)
    {
      struct process *new_node = malloc(sizeof(struct process));
      new_node->pid = data[i].pid;
      new_node->burst_time = data[i].burst_time;
      new_node->arrival_time = data[i].arrival_time;
      new_node->wait_time = 0;
      new_node->response_time = 0;

      TAILQ_INSERT_TAIL(&list, new_node, pointers);
    }
  }

  int time_slice = quantum_length; // or remaining time
  time += time_slice;
  while (false && !TAILQ_EMPTY(&list))
  {
    // add nodes to linked list
    for (int i = left; i < size; i++)
    {
      if (data[i].arrival_time == time)
      {
        struct process *new_node = malloc(sizeof(struct process));
        new_node->pid = data[i].pid;
        new_node->burst_time = data[i].burst_time;
        new_node->arrival_time = data[i].arrival_time;
        new_node->wait_time = 0;
        new_node->response_time = 0;

        TAILQ_INSERT_TAIL(&list, new_node, pointers);
      }
    }
    for (curr = TAILQ_FIRST(&list); curr != NULL; curr = next)
    {
      time += quantum_length;
      next = TAILQ_NEXT(curr, pointers);
      // TAILQ_REMOVE(&list, curr, pointers);
      // free(curr);
    }
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
