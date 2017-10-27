#include <ruby.h>
#include <ruby/debug.h>
#include <sys/shm.h>
#include <stdio.h>

// from afl config.h
#define SHM_ENV_VAR   "__AFL_SHM_ID"
#define FORKSRV_FD    198
#define MAP_SIZE_POW2 16
#define MAP_SIZE      (1 << MAP_SIZE_POW2)

unsigned int lhash(const char *str, long len, size_t offset)
{
  // 32-bit Fowler–Noll–Vo hash function
  uint32_t h = 0x811C9DC5;
  while(len > 0) {
      h ^= (unsigned char) str[0];
      h *= 0x01000193;
      len -= 1;
      str += 1;
  }

  while(offset > 0) {
      h ^= (unsigned char) offset;
      h *= 0x01000193;
      offset >>= 8;
  }

  return h;
}

void trace(VALUE tpval, void *data)
{
  static unsigned int prev_location;
  unsigned char *afl_area = (unsigned char*) data;

  rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);
  VALUE path = rb_tracearg_path(tparg);
  VALUE lineno = rb_tracearg_lineno(tparg);

  unsigned int location = lhash(
    StringValuePtr(path),
    RSTRING_LEN(path),
    FIX2LONG(lineno)
  ) % MAP_SIZE;

  unsigned int offset = location ^ prev_location;
  prev_location = location >> 1;
  afl_area[offset] += 1;
}

VALUE afl_init(VALUE self)
{
  const char *afl_shm_id = getenv(SHM_ENV_VAR);
  if(afl_shm_id == NULL) {
    return Qfalse;
  }

  unsigned char *afl_area = shmat(atoi(afl_shm_id), NULL, 0);
  if(afl_area == (void*) -1) {
    return Qfalse;
  }

  VALUE tracepoint = rb_tracepoint_new(0, RUBY_EVENT_LINE, trace, (void*) afl_area);
  rb_tracepoint_enable(tracepoint);

  return Qtrue;
}

void Init_afl(void)
{
  VALUE mAFL = rb_define_module("AFL");
  rb_define_module_function(mAFL, "init", afl_init, 0);
}
