NAME = RPN

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp RPN.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp RPN.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-cmd /C del /Q $(OBJ) 2>NUL

fclean: clean
	-cmd /C del /Q $(NAME).exe $(NAME) 2>NUL

re: fclean all

.PHONY: all clean fclean re
