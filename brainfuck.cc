#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>

void usage()
{
  std::cout << "Small brainfuck interpreter. Usage:" << std::endl;
  std::cout << "\tbf <brainfuck code>" << std::endl;
  std::cout << std::endl;
}

const uint16_t MEMORY_SIZE = 30000;
struct State {
  int8_t memory[MEMORY_SIZE];
  uint16_t tp;
} _state = {0};

enum Operations
  {
    OP_FORWARD = '>',
    OP_BACKWARD = '<',
    OP_INCR = '+',
    OP_DECR = '-',
    OP_COUT = '.',
    OP_CIN = ',',
    OP_WHILE = '[',
    OP_END_WHILE = ']'
  };

void interpret(const char* p)
{
#define GUARD(p)                                                    \
  do {                                                              \
    if (p >= MEMORY_SIZE)                                           \
    {                                                               \
      std::cerr << "Invalid operation (out of bound)" << std::endl; \
      exit(1);                                                      \
    }                                                               \
  } while(0)

  const char* const START_P = p;
#define OFFSET(v) ((std::uintptr_t)(v) - (std::uintptr_t)START_P)

  static void* dispatch_table[127] {
    /* 0 */
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop,
    /* 43 */
        &&op_incr,
	&&op_cin,
	&&op_decr,
	&&op_cout,
    /* 47 */
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
    /* 57 */
        &&op_noop, &&op_noop, &&op_noop,
    /* 60 */
	&&op_backward,
	&&op_noop,    
	&&op_forward,
    /* 63 */
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
    /* 91 */
	&&op_while,
	&&op_noop,
	&&op_end_while,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop, &&op_noop,
	&&op_noop, &&op_noop, &&op_noop
      };
#define NEXT() { goto *dispatch_table[*p++]; }

  std::stack<std::ptrdiff_t> starts;

  // FIXME: should only be a valid char
  NEXT();
  while(1)
    {
    op_forward:
      ++_state.tp;
      NEXT();

    op_backward:
      --_state.tp;
      NEXT();
      
    op_incr:
      GUARD(_state.tp);
      _state.memory[_state.tp]++;
      NEXT();
      
    op_decr:
      GUARD(_state.tp);
      _state.memory[_state.tp]--;
      NEXT();
      
    op_cout:
      std::cout << (int8_t) _state.memory[_state.tp];
      NEXT();
	  
    op_cin:
      std::cin >> _state.memory[_state.tp];
      NEXT();

    op_while:
      {
	if (_state.memory[_state.tp])
	  {
	    starts.push(OFFSET(p));
	  }
	else
	  {
	    int level = 0;
	    ++p;
	    while (p != 0 && *p != '\0')
	      {
		if (*p == '[')
		  {
		    ++level;
		  }
		else if (*p == ']')
		  {
		    if (level == 0)
		      {
			break;
		      }
		    else
		      {
			--level;
		      }
		  }
		++p;
	      }
	    ++p;
	  }
      }
      NEXT();

    op_end_while:
      {
	if (_state.memory[_state.tp])
	  {
	    if (starts.empty())
	      {
		std::cerr << "Invalid operation (empty '[')"
			  << std::endl;
		exit(1);
	      }
	    if (starts.top() < 0
		|| starts.top() > (OFFSET(p)-1))
	      {
		std::cerr << "Invalid operation (invalid initial ']'): "
			  << starts.top()
			  << std::endl;
		exit(1);
	      }
	    std::ptrdiff_t s = starts.top();
	    p = START_P + s;
	  }
	else 
	  {
	    starts.pop();
	  }
      }
      NEXT();

    op_noop:
      NEXT();

    } // while() {

#undef NEXT
#undef OFFSET
#undef GUARD
}

int main(int argc, char* argv[])
{
  std::string file;
  bool debugger = false;

  int idx = 0;
  struct option long_options[] = {
    {"file", required_argument, 0,  'f'},
    {"debugger", no_argument, 0,  'd'},
    {0, 0, 0, 0}
  };
  while (-1 != (idx = getopt_long(
      argc,
      argv,
      "f:d",
      long_options,
      &idx)))
    {
      switch(idx)
	{
	case 'f':
	  file = optarg;
	  break;
	case 'd':
	  debugger = true;
	  break;
	}
    }

  std::vector<std::string::value_type> p;
  if (!file.empty())
    {
      std::ifstream ifs(file);
      std::stringstream ss;
      ss << ifs.rdbuf();
      std::string s = ss.str();
      p = std::vector<std::string::value_type>(
          s.begin(),
	  s.end());
    }
  else
    {
      char c;
      while (std::cin.get(c))
	{
	  if (c == '\n')
	    break;
	  p.push_back(c);
	}
      p.push_back('\0');
    }

  interpret(&p[0]);

  return EXIT_SUCCESS;
}
