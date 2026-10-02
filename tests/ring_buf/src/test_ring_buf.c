/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	zassert_equal(rb_push(99), 0, "Initial push should succeed");
	zassert_equal(rb_init(4), 0, "Reinitialization should succeed");
	zassert_true(rb_is_empty(), "Reinitialized buffer must be empty");
	zassert_equal(rb_count(), 0, "Reinitialized buffer count must be 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int value;

	zassert_equal(rb_push(42), 0, "Push should succeed");
	zassert_equal(rb_pop(&value), 0, "Pop should succeed");
	zassert_equal(value, 42, "Popped value should match pushed value");
	zassert_true(rb_is_empty(), "Buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int value;

	zassert_equal(rb_push(1), 0, "First push should succeed");
	zassert_equal(rb_push(2), 0, "Second push should succeed");
	zassert_equal(rb_push(3), 0, "Third push should succeed");

	zassert_equal(rb_pop(&value), 0, "First pop should succeed");
	zassert_equal(value, 1, "First popped value should be 1");
	zassert_equal(rb_pop(&value), 0, "Second pop should succeed");
	zassert_equal(value, 2, "Second popped value should be 2");
	zassert_equal(rb_pop(&value), 0, "Third pop should succeed");
	zassert_equal(value, 3, "Third popped value should be 3");
	zassert_true(rb_is_empty(), "Buffer must be empty after FIFO pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	zassert_equal(rb_push(1), 0, "First push should succeed");
	zassert_equal(rb_push(2), 0, "Second push should succeed");
	zassert_equal(rb_push(3), 0, "Third push should succeed");
	zassert_equal(rb_push(4), 0, "Fourth push should succeed");
	zassert_true(rb_is_full(), "Buffer must be full after four pushes");
	zassert_equal(rb_push(99), -ENOSPC, "Push past capacity must return -ENOSPC");
	zassert_equal(rb_count(), 4, "Rejected push must not change count");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int value;

	zassert_equal(rb_push(7), 0, "Push should succeed");
	zassert_equal(rb_peek(&value), 0, "First peek should succeed");
	zassert_equal(value, 7, "First peek should return 7");
	zassert_equal(rb_peek(&value), 0, "Second peek should succeed");
	zassert_equal(value, 7, "Second peek should return 7");
	zassert_equal(rb_count(), 1, "Peek must not consume the value");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	zassert_equal(rb_pop(NULL), -EINVAL, "Pop with NULL must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	zassert_equal(rb_push(1), 0, "First push should succeed");
	zassert_equal(rb_push(2), 0, "Second push should succeed");
	zassert_equal(rb_push(3), 0, "Third push should succeed");
	zassert_equal(rb_push(4), 0, "Fourth push should succeed");
	zassert_true(rb_is_full(), "Buffer must be full after four pushes");
	zassert_equal(rb_count(), 4, "Full buffer count must be 4");
}
