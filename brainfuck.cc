#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <iostream>
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
      std::cout << "cur: " << *p << std::endl;
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
              starts.push(OFFSET(p));
            else
              while(p != 0 && *p != '\0' && *p != ']') ++p;
          }
          break;
        case ']':
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
            std::cout << s << std::endl;
            starts.pop();
            p = START_P + s;
            std::cout << "next: " << *p << std::endl;
            continue;
          }
          break;
        default:
          std::cerr << "Invalid operation (unknown)" << std::endl;
        }
      ++p;
    }

#undef OFFSET
#undef GUARD
}

int main(int argc, char* argv[])
{
  std::vector<char> p;
  char c;
  while (std::cin.get(c))
    {
      if (c == '\n')
        break;
      p.push_back(c);
    }
  p.push_back('\0');
  
  interpret(&p[0]);

  return EXIT_SUCCESS;
}
