NAME = webserv

CXX = c++

CXXFLAGS = -std=c++98  -g3 #-fsanitize=address #-Wall -Wextra -Werror

SRC  = srcs/main.cpp srcs/parsing/request/request.cpp

INC = -Iincludes

OBJ = $(SRC:.cpp=.o)

.SILENT:

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(INC) $(OBJ) -o $(NAME)
	@rm -rf $(OBJ)
	@echo -e "\033[0;32mDone    \033[0m"
	# @clear
	# @./$(NAME) | cat -e

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@
	@echo -e "\033[0;36mCompiling \033[0m" $<

clean:
	@rm -f $(OBJ)
	@echo -e "\033[0;33mCleaned \033[0m"

fclean:
	@rm -f $(NAME) $(OBJ)
	@echo -e "\033[0;31mFcleaned \033[0m"

re: fclean all clean

.PHONY: all clean fclean re