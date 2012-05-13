#include <cutter.h>

#include "libmp3splt/mp3splt.h"
#include "socket_manager.h"

static int error = SPLT_OK;
static splt_state *state = NULL;

int process_functor(const char *received_line, int line_number, void *user_data);
int process_functor_with_skip_line(const char *received_line, int line_number, void *user_data);
int processor_with_continue_false(const char *received_line, int line_number, void *user_data);
ssize_t recv_without_headers(int descriptor, void *buffer, size_t buffer_size, int flags);
ssize_t recv_with_headers(int descriptor, void *buffer, size_t buffer_size, int flags);

static int counter = 10;

void cut_setup()
{
  state = mp3splt_new_state(NULL);
  error = SPLT_OK;
}

void cut_teardown()
{
  mp3splt_free_state(state, NULL);
}

void test_sm_receive_and_process()
{
  counter = 10;

  splt_socket_handler *sh = splt_sm_socket_handler_new(&error);
  cut_assert_equal_int(SPLT_OK, error);

  splt_sm_receive_and_process_with_recv(sh, state, &recv_without_headers, &process_functor, NULL);

  splt_sm_socket_handler_free(&sh);
}

void test_sm_receive_and_process_with_headers_skipped()
{
  counter = 10;

  splt_socket_handler *sh = splt_sm_socket_handler_new(&error);
  cut_assert_equal_int(SPLT_OK, error);

  splt_sm_receive_and_process_without_headers_with_recv(sh, state, &recv_with_headers,
      &process_functor_with_skip_line, NULL, 1); 

  splt_sm_socket_handler_free(&sh);
}

void test_sm_receive_and_process_with_continue_processor_false()
{
  splt_socket_handler *sh = splt_sm_socket_handler_new(&error);
  cut_assert_equal_int(SPLT_OK, error);

  splt_sm_receive_and_process_without_headers_with_recv(sh, state, &recv_with_headers,
      &processor_with_continue_false, NULL, 1); 

  splt_sm_socket_handler_free(&sh);
}

int processor_with_continue_false(const char *received_line, int line_number, void *user_data)
{
  cut_assert_not_equal_int(41, counter);

  if (counter == 31)
  {
    cut_assert_equal_int(1, line_number);
    cut_assert_equal_string("212223242526272829", received_line);
    return SPLT_FALSE;
  }

  return SPLT_TRUE;
}

int process_functor_with_skip_line(const char *received_line, int line_number, void *user_data)
{
  cut_assert_not_equal_int(21, counter);

  if (counter == 31)
  {
    cut_assert_equal_int(1, line_number);
    cut_assert_equal_string("212223242526272829", received_line);
  }
  else if (counter == 41)
  {
    cut_assert_equal_int(2, line_number);
    cut_assert_equal_string("313233343536373839", received_line);
  }

  return SPLT_TRUE;
}

int process_functor(const char *received_line, int line_number, void *user_data)
{
  if (counter == 11)
  {
    cut_assert_equal_int(1, line_number);
    cut_assert_equal_string("", received_line);
  }
  if (counter == 21)
  {
    cut_assert_equal_int(2, line_number);
    cut_assert_equal_string("111213141516171819", received_line);
  }
  else if (counter == 31)
  {
    cut_assert_equal_int(3, line_number);
    cut_assert_equal_string("212223242526272829", received_line);
  }
  else if (counter == 41)
  {
    cut_assert_equal_int(4, line_number);
    cut_assert_equal_string("313233343536373839", received_line);
  }

  return SPLT_TRUE;
}

ssize_t recv_with_headers(int descriptor, void *buffer, size_t buffer_size, int flags)
{
  if (counter == 41) {
    return 0;
  }

  if (counter < 10)
  {
    snprintf(buffer, buffer_size, "AA");
    counter++;
    return 2;
  }

  if (counter == 10)
  {
    snprintf(buffer, buffer_size, "\r\n");
    counter++;
    return 2;
  }

  if (counter % 10 != 0)
  {
    snprintf(buffer, buffer_size, "%d", counter);
  }
  else
  {
    snprintf(buffer, buffer_size, "\n");
  }

  counter++;

  return 2;
}

ssize_t recv_without_headers(int descriptor, void *buffer, size_t buffer_size, int flags)
{
  if (counter == 41) {
    return 0;
  }

  if (counter % 10 != 0)
  {
    snprintf(buffer, buffer_size, "%d", counter);
  }
  else
  {
    snprintf(buffer, buffer_size, "\n");
  }

  counter++;

  return 2;
}

