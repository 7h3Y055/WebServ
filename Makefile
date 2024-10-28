NAME = webserv

CXX = c++

CXXFLAGS = -std=c++98  #-Wall -Wextra -Werror #-fsanitize=address -g3

SRC  = srcs/main.cpp srcs/parsing/request/request.cpp

INC = -Iincludes

OBJ = $(SRC:.cpp=.o)

.SILENT:

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(INC) $(OBJ) -o $(NAME)
	@rm -rf $(OBJ)
	@echo -e "\033[0;32mDone    \033[0m"
	@clear
	@./$(NAME) |  less
	# @nc -l 80 | ./$(NAME)

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