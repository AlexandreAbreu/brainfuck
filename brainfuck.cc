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

  std::stack<std::ptrdiff_t> starts;
  while(*p != '\0')
    {
      if(strstr("+-,[].]><", std::string(1, *p).c_str()))
	{
	  //	  std::cout << "C: " << OFFSET(p) << ", cur: " << *p << std::endl;
	}

      switch(*p)
        {
        case '>':
          ++_state.tp;
          break;
        case '<':
          --_state.tp;
          break;
        case '+':
          GUARD(_state.tp);
          _state.memory[_state.tp]++;
          break;
        case '-':
          GUARD(_state.tp);
          _state.memory[_state.tp]--;
          break;
        case '.':
          std::cout << (int8_t) _state.memory[_state.tp];
          break;
        case ',':
          std::cin >> _state.memory[_state.tp];
          break;
        case '[':
          {
            if (_state.memory[_state.tp])
	      {
		starts.push(OFFSET(p) + 1);
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
	      }
          }
          break;
        case ']':
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
		continue;
	      }
	    else 
	      {
		std::ptrdiff_t s = starts.top();
		starts.pop();
	      }
          }
          break;
        default:
	  break;
        }
      ++p;
    }

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
